
#include "WorkerDisp.h"
#include "Thread.h"
#include "NetWorker.h"

H_BNAMSP

SINGLETON_INIT(CWorkerDisp)
CWorkerDisp objWorker;

CWorkerDisp::CWorkerDisp(void) : CRecvTask<CWorkerTask>(H_QULENS_WORKERDISP), 
    m_usThreadNum(H_INIT_NUMBER), m_uiInitCMD(TCMD_INIT), m_uiRunCMD(TCMD_RUN), 
    m_uiDestroyCMD(TCMD_DEL), m_pWorker(NULL)
{
    setDel(false);
}

CWorkerDisp::~CWorkerDisp(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(itTask->second);
    }
    m_mapTask.clear();

    H_SafeDelArray(m_pWorker);
}

void CWorkerDisp::setThreadNum(const unsigned short usNum)
{
    m_usThreadNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() * 2 : usNum);
    m_pWorker = new(std::nothrow) CWorker[m_usThreadNum];    
    H_ASSERT(NULL != m_pWorker, "malloc memory error.");

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pWorker[usI].setIndex(usI);
        CThread::Creat(&m_pWorker[usI]);
        m_pWorker[usI].waitStart();
    }
}

CChan *CWorkerDisp::getChan(const char *pszTaskName)
{   
    CChan *pChan(NULL);

    m_objTaskLock.rLock();
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        pChan = itTask->second->getChan();
    }
    m_objTaskLock.unLock();

    return pChan;
}

CChan *CWorkerDisp::regTask(CWorkerTask *pTask)
{
    m_objTaskLock.wLock();
    taskit itTask = m_mapTask.find(*pTask->getName());
    H_ASSERT(m_mapTask.end() == itTask, H_FormatStr("task %s repeat register.", pTask->getName()->c_str()).c_str());
    m_mapTask.insert(std::make_pair(*pTask->getName(), pTask));
    m_objTaskLock.unLock();

    notifyInit(pTask);

    m_objAllNamLock.wLock();
    m_lstAllName.push_back(*pTask->getName());
    m_objAllNamLock.unLock();

    return pTask->getChan();
}

void CWorkerDisp::unregTask(const char *pszName)
{
    CWorkerTask *pTask(NULL);
    std::string strName(pszName);

    m_objTaskLock.wLock();
    taskit itTask = m_mapTask.find(strName);
    if (m_mapTask.end() != itTask)
    {
        pTask = itTask->second;
        m_mapTask.erase(itTask);
    }
    m_objTaskLock.unLock();

    if (NULL == pTask)
    {
        H_Printf("not find task by name %s", pszName);
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
    m_objAllNamLock.unLock();
}

CWorker *CWorkerDisp::getFreeWorker(void)
{
    while (true)
    {
        for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
        {
            if (H_INIT_NUMBER == m_pWorker[usI].getStatus())
            {
                m_pWorker[usI].setBusy();
                return &m_pWorker[usI];
            }
        }

        H_Sleep(0);
    }

    return NULL;
}

void CWorkerDisp::stopNet(void)
{
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    pNet->readyStop();
    while (RSTOP_RAN != pNet->getReadyStop())
    {
        H_Sleep(10);
    }
}

void CWorkerDisp::stopWorker(void)
{
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pWorker[usI].Join();
    }
}

void CWorkerDisp::runTask(CWorkerTask *pTask)
{
    if (H_INIT_NUMBER != pTask->getStatus())
    {
        addTask(pTask);
        return;
    }

    pTask->getCMDLock()->Lock();
    unsigned int * pCMD((unsigned int *)pTask->getCMDQu()->Pop());
    pTask->getCMDLock()->unLock();
    if (NULL == pCMD)
    {
        return;
    }

    CWorker *pWorker(getFreeWorker());
    pTask->setCMD(pCMD);
    pWorker->addWorker(pTask);
}

void CWorkerDisp::onLoopBreak(void)
{
    //停止网络
    stopNet();
    //停止工作线程
    stopWorker();
    //未执行完的加到队列
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        for (size_t i = H_INIT_NUMBER; i < itTask->second->getChan()->getSize(); ++i)
        {
            addTask(itTask->second);
        }
    }
}

void CWorkerDisp::runSurplusTask(CWorkerTask *pTask)
{
    pTask->runTask();
}

void CWorkerDisp::destroyRun(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }
}

std::string CWorkerDisp::getAllName(void) 
{
    std::string strNames("[");
    size_t iIndex(H_INIT_NUMBER);
    size_t iLens(H_INIT_NUMBER);

    m_objAllNamLock.rLock();
    iLens = m_lstAllName.size();
    for (std::list<std::string>::iterator itName = m_lstAllName.begin(); m_lstAllName.end() != itName; itName++)
    {
        iIndex++;
        if (iLens == iIndex)
        {
            strNames += "\"" + *itName + "\"";
        }
        else
        {
            strNames += "\"" + *itName + "\",";
        }        
    }
    m_objAllNamLock.unLock();

    strNames += "]";

    return strNames;
}

H_ENAMSP
