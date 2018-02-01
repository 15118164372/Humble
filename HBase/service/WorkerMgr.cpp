
#include "WorkerMgr.h"
#include "HThread.h"
#include "Utils.h"

H_BNAMSP

CWorkerMgr::CWorkerMgr(CMsgTrigger *pMsgTrigger) : m_usRunnerNum(H_INIT_NUMBER), m_uiAdjustTime(H_INIT_NUMBER),
    m_pAllRunner(NULL), m_pMsgTrigger(pMsgTrigger), m_objWorkerPool(H_QULENS_WORKERPOOL)
{
}
CWorkerMgr::~CWorkerMgr(void)
{
    //释放任务
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(itTask->second);
    }
    m_mapTask.clear();

    CWorker *pWorker(NULL);
    while (NULL != (pWorker = popPool()))
    {
        H_SafeDelete(pWorker);
    }

    H_SafeDelArray(m_pAllRunner);
}

CWorker *CWorkerMgr::popPool(void)
{
    return (CWorker *)m_objWorkerPool.Pop();
}
void CWorkerMgr::pushPool(CWorker *pWorker)
{
    H_ASSERT(m_objWorkerPool.Push(pWorker), "worker pool is full!");
}

CWorker *CWorkerMgr::getWorker(const char *pszName)
{
    CWorker *pWorker(NULL);

    m_objTaskLock.rLock();
    taskit itTask(m_mapTask.find(pszName));
    if (m_mapTask.end() != itTask)
    {
        pWorker = itTask->second;
    }
    m_objTaskLock.runLock();

    if (NULL == pWorker)
    {
        H_LOG(LOGLV_WARN, "not find task by name %s", pszName);
    }

    return pWorker;
}

void CWorkerMgr::regTask(CWorker *pWorker)
{
    pWorker->setFreeze(false);
    pWorker->setWorkerMgr(this);
    pWorker->setIndex(calIndex(pWorker->getName()));

    m_objTaskLock.wLock();
    taskit itTask(m_mapTask.find(pWorker->getName()));
    H_ASSERT(m_mapTask.end() == itTask, "task repeat register.");
    m_mapTask.insert(std::make_pair(pWorker->getName(), pWorker));
    m_objTaskLock.wunLock();

    CTaskInitAdjure *pInit = new(std::nothrow) CTaskInitAdjure;
    H_ASSERT(NULL != pInit, H_ERR_MEMORY);
    addAdjureToTask(pWorker, pInit);

    H_LOG(LOGLV_SYS, "register task %s.", pWorker->getName());
}

void CWorkerMgr::unRegTask(const char *pszName)
{
    CWorker *pWorker(NULL);

    m_objTaskLock.wLock();
    taskit itTask(m_mapTask.find(pszName));
    if (m_mapTask.end() != itTask)
    {
        pWorker = itTask->second;
        m_mapTask.erase(itTask);
    }
    m_objTaskLock.wunLock();

    if (NULL == pWorker)
    {
        H_LOG(LOGLV_WARN, "not find task by name %s", pszName);
        return;
    }

    m_pMsgTrigger->removeEvent(pWorker);

    CTaskDelAdjure *pDel = new(std::nothrow) CTaskDelAdjure;
    H_ASSERT(NULL != pDel, H_ERR_MEMORY);
    addAdjureToTask(pWorker, pDel);

    pWorker->setFreeze(true);
    H_LOG(LOGLV_SYS, "unregister task %s.", pszName);
}
void CWorkerMgr::destroyAll(void)
{
    for (taskit it = m_mapTask.begin(); m_mapTask.end() != it; ++it)
    {
        it->second->destroyTask();
    }
}

void CWorkerMgr::getAllName(std::list<std::string> *pvcName)
{
    pvcName->clear();

    m_objTaskLock.rLock();
    for (taskit it = m_mapTask.begin(); m_mapTask.end() != it; ++it)
    {
        pvcName->push_back(it->first);
    }
    m_objTaskLock.runLock();
}

void CWorkerMgr::Stop(void)
{
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usRunnerNum; ++usI)
    {
        m_pAllRunner[usI].Stop();
    }

    destroyAll();
}

void CWorkerMgr::Start(const unsigned short &usRunnerNum, const unsigned int &uiAlarmTime, const unsigned int &uiAdjustTime)
{
    m_usRunnerNum = usRunnerNum;
    m_uiAdjustTime = uiAdjustTime;
    m_pAllRunner = new(std::nothrow) CRunner[m_usRunnerNum];
    H_ASSERT(NULL != m_pAllRunner, H_ERR_MEMORY);
    //启动工作线程
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usRunnerNum; ++usI)
    {
        m_pAllRunner[usI].setIndex(usI);
        m_pAllRunner[usI].setAllRunner(m_pAllRunner);
        m_pAllRunner[usI].setAlarmTime(uiAlarmTime);

        (void)CThread::Creat(&m_pAllRunner[usI]);
        m_pAllRunner[usI].waitStart();
    }
}

unsigned short CWorkerMgr::calIndex(const char *pszName)
{
    return (unsigned short)(1 == m_usRunnerNum ? H_INIT_NUMBER : CUtils::hashStr(pszName) % m_usRunnerNum);
}

void CWorkerMgr::adjustLoad(void)
{
    if (1 == m_usRunnerNum)
    {
        return;
    }

    unsigned int uiUseTime(H_INIT_NUMBER);
    unsigned int uiMax(H_INIT_NUMBER);
    unsigned int uiMin(H_INIT_NUMBER);
    unsigned short usMax(H_INIT_NUMBER);
    unsigned short usMin(H_INIT_NUMBER);
    //找出最大，最小执行时间线程序号
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usRunnerNum; ++usI)
    {
        uiUseTime = m_pAllRunner[usI].getRunTime();
        m_pAllRunner[usI].resetRunTime();
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
    unsigned int uiDiffer((uiMax - uiMin) / 1000);
    if (uiDiffer < m_uiAdjustTime)
    {
        return;
    }

    m_pAllRunner[usMax].adjustLoad(usMin);
}

H_ENAMSP
