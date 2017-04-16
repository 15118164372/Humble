
#include "Chan.h"
#include "WorkerDisp.h"

H_BNAMSP

CChan::CChan(void) : m_pstrName(NULL)
{
}

CChan::~CChan(void)
{
}

void CChan::Send(void *pszVal)
{
    m_objQuLck.Lock();
    m_quData.push(pszVal);
    m_objQuLck.unLock();

	if (NULL != m_pstrName)
	{
		CWorkerDisp::getSingletonPtr()->Notify(m_pstrName);
	}    
}

void *CChan::Recv(void)
{
    void *pVal = NULL;

    m_objQuLck.Lock();
    if (!m_quData.empty())
    {
        pVal = m_quData.front();
        m_quData.pop();
    }
    m_objQuLck.unLock();    

    return pVal;
}

size_t CChan::getSize(void)
{
    size_t iSize(H_INIT_NUMBER);

    m_objQuLck.Lock();
    iSize = m_quData.size();
    m_objQuLck.unLock();

    return iSize;
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
