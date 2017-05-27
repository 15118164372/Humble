
#include "Chan.h"
#include "TaskDispatch.h"
#include "Log.h"

H_BNAMSP

CChan::CChan(class CTaskWorker *pTask, const int iCapacity) : m_pTask(pTask), m_quData(iCapacity)
{

}

CChan::~CChan(void)
{

}

bool CChan::Send(void *pszVal)
{
    if (m_pTask->getDestroy())
    {
        H_LOG(LOGLV_ERROR, "task %s already closed.", m_pTask->getName()->c_str());
        return false;
    }

    m_objQuLck.Lock();
    bool bOk(m_quData.Push(pszVal));
    m_objQuLck.unLock();

	if (bOk)
	{
		CTaskDispatch::getSingletonPtr()->notifyRun(m_pTask);
	}
    else
    {
        H_LOG(LOGLV_ERROR, "push message to task %s error.", m_pTask->getName()->c_str());
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

class CTaskWorker *CChan::getTask(void)
{
    return m_pTask;
}

H_ENAMSP
