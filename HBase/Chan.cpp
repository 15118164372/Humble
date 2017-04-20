
#include "Chan.h"
#include "WorkerDisp.h"

H_BNAMSP

CChan::CChan(const int iCapacity) : m_pstrName(NULL), m_quData(iCapacity)
{
}

CChan::~CChan(void)
{
}

bool CChan::Send(void *pszVal)
{
    m_objQuLck.Lock();
    bool bOk(m_quData.Push(pszVal));
    m_objQuLck.unLock();

	if (NULL != m_pstrName 
        && bOk)
	{
		CWorkerDisp::getSingletonPtr()->Notify(m_pstrName);
	} 

    return bOk;
}

void *CChan::Recv(void)
{
    m_objQuLck.Lock();
    void *pVal(m_quData.Pop());
    m_objQuLck.unLock();    

    return pVal;
}

size_t CChan::getSize(void)
{
    m_objQuLck.Lock();
    size_t iSize(m_quData.Size());
    m_objQuLck.unLock();

    return iSize;
}

size_t CChan::getCapacity(void)
{
    return m_quData.Capacity();
}

void CChan::setTaskNam(std::string *pszName)
{
    m_pstrName = pszName;
}

const char *CChan::getTaskName(void)
{
    return m_pstrName->c_str();
}

H_ENAMSP
