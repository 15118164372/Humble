
#include "NetWorkerMgr.h"
#include "Thread.h"
#include "Linker.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CNetWorkerMgr)
CNetWorkerMgr objNetWorkerMgr;

CNetWorkerMgr::CNetWorkerMgr(void) : m_usWorkerNum(H_INIT_NUMBER), m_pNetWorker(NULL)
{
    
}

CNetWorkerMgr::~CNetWorkerMgr(void)
{
    H_SafeDelArray(m_pNetWorker);
}

void CNetWorkerMgr::startWorker(const unsigned short usNum)
{
    m_usWorkerNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() : usNum);
    m_pNetWorker = new(std::nothrow) CNetWorker[m_usWorkerNum];
    H_ASSERT(NULL != m_pNetWorker, "malloc memory error.");

    for (unsigned short usIndex = H_INIT_NUMBER; usIndex < m_usWorkerNum; usIndex++)
    {
        CThread::Creat(&m_pNetWorker[usIndex]);
        m_pNetWorker[usIndex].waitStart();
    }
}

void CNetWorkerMgr::stopWorker(void)
{
    for (unsigned short usIndex = H_INIT_NUMBER; usIndex < m_usWorkerNum; usIndex++)
    {
        m_pNetWorker[usIndex].Join();
    }
}

bool CNetWorkerMgr::removeLink(H_SOCK &sock)
{
    if (H_INVALID_SOCK == sock)
    {
        return false;
    }

    CLinker::getSingletonPtr()->removeLink(sock);
    return m_pNetWorker[getIndex(sock)].closeLink(sock);
}

H_ENAMSP
