
#include "Chan.h"
#include "TaskWorker.h"
#include "TaskGlobleQu.h"
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
    m_objQuLck.Lock();
    bool bOk(m_quData.Push(pszVal));
    if (bOk)
    {
        if (!m_pTask->getInGloble())
        {
            CTaskGlobleQu::getSingletonPtr()->m_objQuLock.Lock();
            if (CTaskGlobleQu::getSingletonPtr()->m_objQu.Push(m_pTask))
            {
                m_pTask->setInGloble(true);
            }
            else
            {
                H_LOG(LOGLV_ERROR, "add task %s in globle queue error.", m_pTask->getName()->c_str());
            }
            CTaskGlobleQu::getSingletonPtr()->m_objQuLock.unLock();
        }
    }
    m_objQuLck.unLock();

    if (bOk)
    {
        if (CTaskGlobleQu::getSingletonPtr()->m_uiWait > H_INIT_NUMBER)
        {
            pthread_cond_signal(&CTaskGlobleQu::getSingletonPtr()->m_objCond);
        }
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
    return m_quData.Size();
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
