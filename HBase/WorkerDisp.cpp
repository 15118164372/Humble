
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

    m_objTaskLock.Lock();
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        pChan = itTask->second->getChan();
    }
    m_objTaskLock.unLock();

    return pChan;
}

void CWorkerDisp::regTask(CWorkerTask *pTask)
{
    std::string strName(*pTask->getName());

    m_objTaskLock.Lock();
    taskit itTask = m_mapTask.find(strName);
    H_ASSERT(m_mapTask.end() == itTask, H_FormatStr("task %s repeat register.", strName.c_str()).c_str());
    m_mapTask.insert(std::make_pair(strName, pTask));
    m_objTaskLock.unLock();

    notifyInit(pTask);

    m_objAllNamLock.Lock();
    m_lstAllName.push_back(strName);
    m_objAllNamLock.unLock();
}

void CWorkerDisp::unregTask(const char *pszName)
{
    CWorkerTask *pTask(NULL);
    std::string strName(pszName);

    m_objTaskLock.Lock();
    taskit itTask = m_mapTask.find(strName);
    if (m_mapTask.end() != itTask)
    {
        pTask = itTask->second;
        m_mapTask.erase(itTask);
    }
    m_objTaskLock.unLock();

    if (NULL != pTask)
    {
        pTask->setDestroy();
        notifyDestroy(pTask);
    }

    m_objAllNamLock.Lock();
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

    CCirQueue *pCMDQu = pTask->getCMDQu();
    CAtomic *pCMDLock = pTask->getCMDLock();

    pCMDLock->Lock();
    unsigned int * pCMD = (unsigned int *)pCMDQu->Pop();
    pCMDLock->unLock();
    if (NULL == pCMD)
    {
        return;
    }

    CWorker *pWorker = getFreeWorker();
    pTask->setCMD(pCMD);
    pWorker->addWorker(pTask);
}

void CWorkerDisp::stopRun(void)
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
    std::string strNames = "[";
    bool bHave(false);

    m_objAllNamLock.Lock();
    if (!m_lstAllName.empty())
    {
        bHave = true;
    }
    for (std::list<std::string>::iterator itName = m_lstAllName.begin(); m_lstAllName.end() != itName; itName++)
    {        
        strNames += "\"" + *itName + "\",";
    }
    m_objAllNamLock.unLock();

    if (bHave)
    {
        strNames = strNames.substr(0, strNames.size() - 1);       
    }

    strNames += "]";

    return strNames;
}

H_ENAMSP
