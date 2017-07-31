
#include "TaskRunner.h"
#include "TaskGlobleQu.h"
#include "Log.h"

H_BNAMSP

CTaskRunner::CTaskRunner(void) : m_usIndex(H_INIT_NUMBER), m_lExit(H_INIT_NUMBER), m_lCount(H_INIT_NUMBER)
{

}

CTaskRunner::~CTaskRunner(void)
{

}

bool CTaskRunner::runTask(class CTaskWorker *pTask)
{
    H_MSG *pMsg;
    for (int iCount = H_INIT_NUMBER; iCount < H_MAXRUNNUM; ++iCount)
    {
        pMsg = (H_MSG*)pTask->getChan()->Recv();
        if (NULL == pMsg)
        {
            break;
        }

        pTask->Run(pMsg);
        if (MSG_TASK_DEL == pMsg->usEnevt)
        {
            H_SafeDelete(pMsg);
            return true;
        }
        H_SafeDelete(pMsg);
    }

    return false;
}

void CTaskRunner::Run(void)
{
    bool bDel(false);    
    CTaskWorker *pTask(NULL);
    CTaskGlobleQu *pGlobleQu(CTaskGlobleQu::getSingletonPtr());

    H_AtomicAdd(&m_lCount, 1);

    while (H_INIT_NUMBER == H_AtomicGet(&m_lExit))
    {
        pGlobleQu->m_objQuLock.Lock();
        pTask = (CTaskWorker *)pGlobleQu->m_objQu.Pop();
        pGlobleQu->m_objQuLock.unLock();
        if (NULL == pTask)
        {
            CLckThis objLock(&pGlobleQu->m_objMutex);
            ++pGlobleQu->m_uiWait;
            pthread_cond_wait(&(pGlobleQu->m_objCond), objLock.getMutex());
            --pGlobleQu->m_uiWait;

            continue;
        }

        bDel = runTask(pTask);        
        if (bDel)
        {
            continue;
        }

        pTask->getInGlobleLock()->Lock();
        if (H_INIT_NUMBER != pTask->getChan()->getSize())
        {
            pGlobleQu->m_objQuLock.Lock();
            if (!pGlobleQu->m_objQu.Push(pTask))
            {
                pTask->setInGloble(false);
                H_LOG(LOGLV_ERROR, "add task %s in globle queue error.", pTask->getName()->c_str());
            }
            pGlobleQu->m_objQuLock.unLock();
        }
        else
        {
            pTask->setInGloble(false);
        }
        pTask->getInGlobleLock()->unLock();
    }

    runSurplus();

    H_AtomicAdd(&m_lCount, -1);
}

void CTaskRunner::runSurplus(void)
{
    H_MSG *pMsg(NULL);
    CTaskWorker *pTask(NULL);
    CTaskGlobleQu *pGlobleQu(CTaskGlobleQu::getSingletonPtr());

    while (true)
    {
        pGlobleQu->m_objQuLock.Lock();
        pTask = (CTaskWorker *)pGlobleQu->m_objQu.Pop();
        pGlobleQu->m_objQuLock.unLock();
        if (NULL == pTask)
        {
            break;
        }

        while (true)
        {
            pMsg = (H_MSG*)pTask->getChan()->Recv();
            if (NULL == pMsg)
            {
                break;
            }

            pTask->Run(pMsg);
            if (MSG_TASK_DEL == pMsg->usEnevt)
            {
                H_SafeDelete(pMsg);
                break;
            }
            H_SafeDelete(pMsg);
        }
    }
}

void CTaskRunner::Join(void)
{
    if (H_INIT_NUMBER != H_AtomicGet(&m_lExit))
    {
        return;
    }

    //等待任务队列完成
    H_AtomicSet(&m_lExit, 1);
    for (;;)
    {
        pthread_cond_signal(&CTaskGlobleQu::getSingletonPtr()->m_objCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

void CTaskRunner::waitStart(void)
{
    for (;;)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_lCount))
        {
            return;
        }

        H_Sleep(10);
    }
}

H_ENAMSP
