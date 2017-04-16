
#include "WorkerDisp.h"
#include "Thread.h"
#include "LockThis.h"
#include "NetWorker.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CWorkerDisp)
CWorkerDisp objWorker;

CWorkerDisp::CWorkerDisp(void) : m_usThreadNum(H_INIT_NUMBER), m_pWorker(NULL)
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
    m_usThreadNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() : usNum);
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
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        return itTask->second->getChan();
    }

    return NULL;
}

void CWorkerDisp::regTask(const char *pszName, CWorkerTask *pTask)
{
    std::string strName(pszName);

    taskit itTask = m_mapTask.find(strName);
    H_ASSERT(m_mapTask.end() == itTask, H_FormatStr("task %s already exist.", pszName).c_str());
            
    pTask->setName(pszName);
    m_mapTask.insert(std::make_pair(strName, pTask));
}

CWorker *CWorkerDisp::getFreeWorker(unsigned short &usIndex)
{
    while (true)
    {
        for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
        {
            if (WS_FREE == m_pWorker[usI].getStatus())
            {
                usIndex = usI;
                return &m_pWorker[usI];
            }
        }

        H_Sleep(0);
    }

    return NULL;
}

CWorkerTask* CWorkerDisp::getTask(std::string *pstrName)
{
    taskit itTask = m_mapTask.find(*pstrName);
    if (m_mapTask.end() != itTask)
    {
        return itTask->second;
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

void CWorkerDisp::initRun(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->initTask();
    }
}

void CWorkerDisp::runTask(std::string *pszTask)
{
    unsigned short usIndex(H_INIT_NUMBER);
    CWorkerTask* pWorkerTask = getTask(pszTask);
    if (NULL == pWorkerTask)
    {
        return;
    }
    if (H_INIT_NUMBER != pWorkerTask->getRef())
    {
        addTask(pszTask);
        return;
    }

    CWorker *pWorker = getFreeWorker(usIndex);
    pWorker->setBusy();
    pWorker->addWorker(pWorkerTask);
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
            addTask(itTask->second->getName());
        }
    }
}

void CWorkerDisp::runSurplusTask(std::string *pszTask)
{
    CWorkerTask *pWorkerTask = getTask(pszTask);
    if (NULL != pWorkerTask)
    {
        pWorkerTask->runTask();
    }
}

void CWorkerDisp::destroyRun(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }
}

H_ENAMSP
