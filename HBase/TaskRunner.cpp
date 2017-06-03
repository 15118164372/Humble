
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

void CTaskRunner::Run(void)
{
    bool bDel(false);
    H_MSG *pMsg(NULL);
    CTaskWorker *pTask(NULL);
    int iCount(H_INIT_NUMBER);
    CTaskGlobleQu *pGlobleQu(CTaskGlobleQu::getSingletonPtr());

    H_AtomicAdd(&m_lCount, 1);

    while (H_INIT_NUMBER == H_AtomicGet(&m_lExit))
    {
        if (NULL != pTask)
        {
            bDel = false;
            for (iCount = H_INIT_NUMBER; iCount < H_MAXRUNNUM; ++iCount)
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
                    bDel = true;
                    break;
                }
                H_SafeDelete(pMsg);
            }

            if (!bDel)
            {
                pTask->getChan()->getLock()->Lock();
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
                pTask->getChan()->getLock()->unLock();
            }
        }        
        
        pGlobleQu->m_objQuLock.Lock();
        pTask = (CTaskWorker *)pGlobleQu->m_objQu.Pop();
        pGlobleQu->m_objQuLock.unLock();
        if (NULL == pTask)
        {
            CLckThis objLock(&pGlobleQu->m_objMutex);
            ++pGlobleQu->m_uiWait;
            pthread_cond_wait(&(pGlobleQu->m_objCond), objLock.getMutex());
            --pGlobleQu->m_uiWait;
        }
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
        if (NULL != pTask)
        {
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

        pGlobleQu->m_objQuLock.Lock();
        pTask = (CTaskWorker *)pGlobleQu->m_objQu.Pop();
        pGlobleQu->m_objQuLock.unLock();
        if (NULL == pTask)
        {
            break;
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
