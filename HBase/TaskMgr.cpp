
#include "TaskMgr.h"
#include "TaskWorker.h"
#include "TaskGlobleQu.h"
#include "Funcs.h"
#include "Thread.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CTaskMgr)
CTaskMgr objTaskMgr;

CTaskMgr::CTaskMgr(void) : CTaskLazy<unsigned int>(H_ONEK/16), 
    m_usThreadNum(H_INIT_NUMBER), m_uiDiffer(H_INIT_NUMBER), m_pRunner(NULL)
{
    setDel(false);
}

CTaskMgr::~CTaskMgr(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(itTask->second);
    }
    m_mapTask.clear();

    H_SafeDelArray(m_pRunner);
}

void CTaskMgr::setDiffer(unsigned int uiDiff)
{
    H_ASSERT(uiDiff > H_INIT_NUMBER, "load differ must big than zero.");
    m_uiDiffer = uiDiff;
}

void CTaskMgr::adjustLoad(unsigned int &uiTick)
{
    (void)addTask(&uiTick);
}

void CTaskMgr::runTask(unsigned int *)
{
    //打印任务队列任务数
    size_t iSize;
    bool bPrint(false);
    std::string strInfo("task queue size:\ntask name\tsize\n");
    std::vector<std::string> vcAllTask = getAllName();
    for (std::vector<std::string>::iterator itTask = vcAllTask.begin(); vcAllTask.end() != itTask; ++itTask)
    {
        iSize = getQueueSize(itTask->c_str());
        if (iSize > 0)
        {
            bPrint = true;
        }
        strInfo += H_FormatStr("%s\t%d\n", itTask->c_str(), iSize);
    }
    if (bPrint)
    {
        H_LOG(LOGLV_SYS, "%s", strInfo.c_str());
    }    

    //只有一个线程则不调整
    if (1 == m_usThreadNum)
    {
        return;
    }

    TaskQueue *pTaskQueue;
    unsigned int uiUseTime(H_INIT_NUMBER);
    unsigned int uiMax(H_INIT_NUMBER);
    unsigned int uiMin(H_INIT_NUMBER);
    unsigned short usMax(H_INIT_NUMBER);
    unsigned short usMin(H_INIT_NUMBER);

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        pTaskQueue = CTaskGlobleQu::getSingletonPtr()->getQueue(usI);
        uiUseTime = H_AtomicGet(&(pTaskQueue->uiTime));
        H_AtomicSet(&(pTaskQueue->uiTime), H_INIT_NUMBER);

        if (H_INIT_NUMBER == usI)
        {
            uiMax = uiUseTime;
            uiMin = uiUseTime;
            usMin = usI;
            usMax = usI;

            continue;
        }

        if (uiMin > uiUseTime)
        {
            uiMin = uiUseTime;
            usMin = usI;
        }
        if (uiMax < uiUseTime)
        {
            uiMax = uiUseTime;
            usMax = usI;
        }
    }

    if (usMin == usMax)
    {
        return;
    }

    unsigned int uiDiffer((uiMax - uiMin)/1000);
    if (uiDiffer < m_uiDiffer)
    {
        return;
    }

    m_pRunner[usMax].setAdjustLoad(usMin);
}

void CTaskMgr::setThreadNum(const unsigned short usNum)
{
    m_usThreadNum = usNum;
    m_pRunner = new(std::nothrow) CTaskRunner[m_usThreadNum];
    H_ASSERT(NULL != m_pRunner, "malloc memory error.");

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pRunner[usI].setIndex(usI);
        CThread::Creat(&m_pRunner[usI]);
        m_pRunner[usI].waitStart();
    }
}

CChan *CTaskMgr::getChan(const char *pszTaskName)
{
    H_ASSERT(NULL != pszTaskName, "got null pointer.");
    CChan *pChan(NULL);

    m_objTaskLock.rLock();
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        pChan = itTask->second->getChan();
    }
    m_objTaskLock.runLock();

    return pChan;
}

size_t CTaskMgr::getQueueSize(const char *pszTaskName)
{
    H_ASSERT(NULL != pszTaskName, "got null pointer.");
    size_t iSize(H_INIT_NUMBER);

    m_objTaskLock.rLock();
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        iSize = itTask->second->getChan()->getSize();
    }
    m_objTaskLock.runLock();

    return iSize;
}

void CTaskMgr::taskRPCCall(unsigned int &uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask,
    const char *pMsg, const size_t &iLens)
{
    H_ASSERT(NULL != pszRPCName 
        && NULL != pszToTask 
        && NULL != pszSrcTask, "got null pointer.");
    CChan *pChan(getChan(pszToTask));
    if (NULL == pChan)
    {
        return;
    }

    H_RPC stRPC;
    H_Zero(&stRPC, sizeof(stRPC));
    stRPC.uiId = uiId;
    stRPC.uiMsgLens = (unsigned int)iLens;
    memcpy(stRPC.acRPC, pszRPCName, strlen(pszRPCName));
    memcpy(stRPC.acSrcTask, pszSrcTask, strlen(pszSrcTask));
    memcpy(stRPC.acToTask, pszToTask, strlen(pszToTask));

    H_MSG *pChanMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pChanMsg, "malloc memory error.");
    pChanMsg->usEnevt = MSG_TASK_RPCCALL;
    pChanMsg->pEvent = new(std::nothrow) char[sizeof(H_RPC) + iLens];
    H_ASSERT(NULL != pChanMsg->pEvent, "malloc memory error.");
    memcpy(pChanMsg->pEvent, &stRPC, sizeof(stRPC));
    if (NULL != pMsg)
    {
        memcpy(pChanMsg->pEvent + sizeof(stRPC), pMsg, iLens);
    }    

    if (!pChan->Send(pChanMsg))
    {
        H_SafeDelArray(pChanMsg->pEvent);
        H_SafeDelete(pChanMsg);

        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

void CTaskMgr::regTask(CTaskWorker *pTask)
{
    unsigned short usIndex(H_HashStr(pTask->getName()->c_str()) % m_usThreadNum);
    pTask->setIndex(usIndex);
    //工作线程任务数加一
    TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(usIndex));
    H_AtomicAdd(&(pTaskQueue->uiTaskNum), 1);

    m_objTaskLock.wLock();
    taskit itTask = m_mapTask.find(*pTask->getName());
    H_ASSERT(m_mapTask.end() == itTask, "task repeat register.");
    m_mapTask.insert(std::make_pair(*pTask->getName(), pTask));
    m_objTaskLock.wunLock();

    notifyInit(pTask);

    m_objAllNamLock.wLock();
    m_lstAllName.push_back(*pTask->getName());
    m_objAllNamLock.wunLock();
}

void CTaskMgr::unregTask(const char *pszName)
{
    H_ASSERT(NULL != pszName, "got null pointer.");
    CTaskWorker *pTask(NULL);
    std::string strName(pszName);

    m_objTaskLock.wLock();
    taskit itTask = m_mapTask.find(strName);
    if (m_mapTask.end() != itTask)
    {
        pTask = itTask->second;
        m_mapTask.erase(itTask);
    }
    m_objTaskLock.wunLock();

    if (NULL == pTask)
    {
        H_LOG(LOGLV_WARN, "not find task by name %s", pszName);
        return;
    }

    notifyDestroy(pTask);

    m_objAllNamLock.wLock();
    for (std::list<std::string>::iterator itName = m_lstAllName.begin(); m_lstAllName.end() != itName; itName++)
    {
        if (*itName == strName)
        {
            m_lstAllName.erase(itName);
            break;
        }
    }
    m_objAllNamLock.wunLock();
}

void CTaskMgr::stopWorker(void)
{
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pRunner[usI].Join();
    }

    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }
}

std::vector<std::string> CTaskMgr::getAllName(void)
{
    std::vector<std::string> vcTmp;

    m_objAllNamLock.rLock();
    for (std::list<std::string>::iterator itName = m_lstAllName.begin(); m_lstAllName.end() != itName; itName++)
    {
        vcTmp.push_back(*itName);
    }
    m_objAllNamLock.runLock();

    return vcTmp;
}

H_ENAMSP
