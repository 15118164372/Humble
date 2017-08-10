
#ifndef H_TASKLAZY_H_
#define H_TASKLAZY_H_

#include "Task.h"
#include "CirQueue.h"
#include "LockThis.h"

H_BNAMSP

template <typename T>
class CTaskLazy : public CTask
{
public:
    CTaskLazy(const int iCapacity);
    ~CTaskLazy(void);

    void Run(void);

    virtual void runTask(T *pMsg) = 0;

    void Join(void);
    void waitStart(void);

protected:
    bool addTask(T *pMsg);
    T *newT(void);
    void setDel(const bool bDel);

private:
    void Free(T *pMsg);

private:
    CTaskLazy(void);
    H_DISALLOWCOPY(CTaskLazy);

private:
    bool m_bDel;
    unsigned int m_uiWait;
    long m_lExit;
    long m_lCount;
    CCirQueue m_quTask;
    pthread_mutex_t m_quLock;
    pthread_cond_t m_objCond;
};

template <typename T>
CTaskLazy<T>::CTaskLazy(const int iCapacity) : m_bDel(true), m_uiWait(H_INIT_NUMBER),
    m_lExit(H_INIT_NUMBER), m_lCount(H_INIT_NUMBER), m_quTask(iCapacity)
{
    pthread_mutex_init(&m_quLock, NULL);
    pthread_cond_init(&m_objCond, NULL);
}

template <typename T>
CTaskLazy<T>::~CTaskLazy(void)
{
    Join();
    pthread_cond_destroy(&m_objCond);
    pthread_mutex_destroy(&m_quLock);
}

template <typename T>
void CTaskLazy<T>::Free(T *pMsg)
{
    if (m_bDel)
    {
        H_SafeDelete(pMsg);
    }
}

template <typename T>
void CTaskLazy<T>::Run(void)
{
    T *pMsg(NULL);
    H_AtomicAdd(&m_lCount, 1);

    while (H_INIT_NUMBER == H_AtomicGet(&m_lExit))
    {
        if (NULL != pMsg)
        {
            runTask(pMsg);
            Free(pMsg);
        }

        CLckThis objLckThis(&m_quLock);
        pMsg = (T *)m_quTask.Pop();
        if (NULL == pMsg)
        {
            ++m_uiWait;
            pthread_cond_wait(&m_objCond, objLckThis.getMutex());
            --m_uiWait;
        }
    }

    pMsg = NULL;
    while (true)
    {
        if (NULL != pMsg)
        {
            runTask(pMsg);
            Free(pMsg);
        }

        CLckThis objLckThis(&m_quLock);
        pMsg = (T *)m_quTask.Pop();
        if (NULL == pMsg)
        {
            break;
        }
    }

    H_AtomicAdd(&m_lCount, -1);
}

template <typename T>
void CTaskLazy<T>::waitStart(void)
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

template <typename T>
bool CTaskLazy<T>::addTask(T *pMsg)
{
    bool bOk(false);
    {
        CLckThis objLckThis(&m_quLock);
        bOk = m_quTask.Push((void*)pMsg);
    }
    
    if (bOk && m_uiWait > H_INIT_NUMBER)
    {
        pthread_cond_signal(&m_objCond);
    }

    return bOk;
}

template <typename T>
T *CTaskLazy<T>::newT(void)
{
    return new(std::nothrow) T;
}

template <typename T>
void CTaskLazy<T>::setDel(const bool bDel)
{
    m_bDel = bDel;
}

template <typename T>
void CTaskLazy<T>::Join(void)
{
    if (H_INIT_NUMBER != H_AtomicGet(&m_lExit))
    {
        return;
    }

    //等待任务队列完成
    H_AtomicSet(&m_lExit, 1);
    for (;;)
    {
        pthread_cond_signal(&m_objCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

H_ENAMSP

#endif//H_TASKLAZY_H_
