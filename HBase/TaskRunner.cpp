
#include "TaskRunner.h"
#include "TaskGlobleQu.h"
#include "Log.h"

H_BNAMSP

CTaskRunner::CTaskRunner(void) : m_usIndex(H_INIT_NUMBER), m_usToIndex(H_INIT_NUMBER), m_uiAdjustLoad(H_INIT_NUMBER),
    m_dAlarmTime(H_INIT_NUMBER), m_lExit(H_INIT_NUMBER), m_lCount(H_INIT_NUMBER)
{
}

CTaskRunner::~CTaskRunner(void)
{
}

bool CTaskRunner::runTask(class CTaskWorker *pTask, class CClock *pClock, double *pRunTime)
{
    H_MSG *pMsg;
    double dTime(H_INIT_NUMBER);
    H_PROTOTYPE iProto(H_INIT_NUMBER);
    *pRunTime = H_INIT_NUMBER;

    for (int iCount = H_INIT_NUMBER; iCount < H_MAXRUNNUM; ++iCount)
    {
        pMsg = (H_MSG*)pTask->getChan()->Recv();
        if (NULL == pMsg)
        {
            break;
        }

        pClock->reStart();
        iProto = pTask->Run(pMsg);
        dTime = pClock->Elapsed(); 
        *pRunTime += dTime;

        if (MSG_TASK_DEL == pMsg->usEnevt)
        {
            H_SafeDelete(pMsg);
            return true;
        }
        
        if (m_dAlarmTime > H_INIT_NUMBER
            && dTime > m_dAlarmTime
            && MSG_TASK_INIT != pMsg->usEnevt)
        {
            if (MSG_NET_READ == pMsg->usEnevt)
            {
                H_LOG(LOGLV_SYS, "task %s, event %d, protocol %d run time %f ms.",
                    pTask->getName()->c_str(), pMsg->usEnevt, iProto, dTime);
            }
            else
            {
                H_LOG(LOGLV_SYS, "task %s, event %d run time %f ms.",
                    pTask->getName()->c_str(), pMsg->usEnevt, dTime);
            }
        }
        H_SafeDelete(pMsg);
    }

    return false;
}

void CTaskRunner::setAdjustLoad(unsigned short &usToIndex)
{
    m_usToIndex = usToIndex;
    H_AtomicSet(&m_uiAdjustLoad, 1);
}

//该函数需要加锁
void CTaskRunner::adjustInGloble(struct TaskQueue *pTaskQueue, class CTaskWorker *pTask)
{
    if (H_INIT_NUMBER != pTask->getChan()->getSize())
    {
        CLckThis objLock(&pTaskQueue->objMutex);
        if (!pTaskQueue->objQueue.Push(pTask))
        {
            pTask->setInGloble(false);
        }

        return;
    }
    
    pTask->setInGloble(false);
}

void CTaskRunner::adjustLoad(class CTaskWorker *pTask, const unsigned short &usIndex)
{
    pTask->getLock()->Lock();

    //原工作线程任务数减一
    TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(pTask->getIndex()));
    H_AtomicAdd(&(pTaskQueue->uiTaskNum), -1);
    
    //设置新工作线程
    pTask->setIndex(usIndex);
    pTaskQueue = CTaskGlobleQu::getSingletonPtr()->getQueue(usIndex);

    //新工作线程任务数加一
    H_AtomicAdd(&(pTaskQueue->uiTaskNum), 1);
    adjustInGloble(pTaskQueue, pTask);
    bool bNotify(pTask->getInGloble());

    pTask->getLock()->unLock();

    if (pTaskQueue->uiWait > H_INIT_NUMBER && bNotify)
    {
        pthread_cond_signal(&pTaskQueue->objCond);
    }

    H_LOG(LOGLV_SYS, "adjust task %s work thread:source thread %d, target thread %d.",
        pTask->getName()->c_str(), m_usIndex, usIndex);
}

void CTaskRunner::Run(void)
{
    bool bDel(false);
    double dRunTime(H_INIT_NUMBER);
    CTaskWorker *pTask(NULL);
    TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(m_usIndex));

    H_AtomicAdd(&m_lCount, 1);

    while (H_INIT_NUMBER == H_AtomicGet(&m_lExit))
    {
        //获取一任务
        {
            CLckThis objLock(&pTaskQueue->objMutex);
            pTask = (CTaskWorker *)pTaskQueue->objQueue.Pop();
            if (NULL == pTask)
            {
                ++pTaskQueue->uiWait;
                pthread_cond_wait(&(pTaskQueue->objCond), objLock.getMutex());
                --pTaskQueue->uiWait;

                continue;
            }
        }

        //执行
        bDel = runTask(pTask, &pTaskQueue->objClock, &dRunTime);
        H_AtomicAdd(&(pTaskQueue->uiTime), (unsigned int)(dRunTime * 1000));
        if (bDel)
        {
            continue;
        }

        //是否需要调整负载
        if (H_INIT_NUMBER != H_AtomicGet(&m_uiAdjustLoad))
        {
            //至少要有1个任务
            if (H_AtomicGet(&(pTaskQueue->uiTaskNum)) > 1)
            {
                adjustLoad(pTask, m_usToIndex);
                H_AtomicSet(&m_uiAdjustLoad, H_INIT_NUMBER);
                m_usToIndex = H_INIT_NUMBER;

                continue;
            }

            H_AtomicSet(&m_uiAdjustLoad, H_INIT_NUMBER);
            m_usToIndex = H_INIT_NUMBER;
        }

        //调整全局队列
        pTask->getLock()->Lock();
        adjustInGloble(pTaskQueue, pTask);
        pTask->getLock()->unLock();
    }

    runSurplus();

    H_AtomicAdd(&m_lCount, -1);
}

void CTaskRunner::runSurplus(void)
{
    H_MSG *pMsg(NULL);
    CTaskWorker *pTask(NULL);
    TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(m_usIndex));

    while (true)
    {
        {
            CLckThis objLock(&pTaskQueue->objMutex);
            pTask = (CTaskWorker *)pTaskQueue->objQueue.Pop();
            if (NULL == pTask)
            {
                break;
            }
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
    TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(m_usIndex));
    H_AtomicSet(&m_lExit, 1);
    for (;;)
    {
        pthread_cond_signal(&pTaskQueue->objCond);
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
