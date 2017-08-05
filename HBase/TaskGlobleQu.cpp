
#include "TaskGlobleQu.h"

H_BNAMSP

SINGLETON_INIT(CTaskGlobleQu)
CTaskGlobleQu objTaskGlobleQu;

CTaskGlobleQu::CTaskGlobleQu(void) : m_usThreadNum(H_INIT_NUMBER)
{
}

CTaskGlobleQu::~CTaskGlobleQu()
{
    std::vector<TaskQueue *>::iterator itQu;
    for (itQu = m_vcQueue.begin(); m_vcQueue.end() != itQu; ++itQu)
    {
        H_SafeDelete((*itQu));
    }
    m_vcQueue.clear();
}

void CTaskGlobleQu::setThreadNum(const unsigned short usNum)
{
    TaskQueue *pQueue;
    m_usThreadNum = usNum;    
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        pQueue = new(std::nothrow) TaskQueue(H_MAXTASKNUM);
        H_ASSERT(NULL != pQueue, "malloc memory error.");
        m_vcQueue.push_back(pQueue);
    }
}

H_ENAMSP
