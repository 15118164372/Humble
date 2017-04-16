
#ifndef H_SERVICETASK_H_
#define H_SERVICETASK_H_

#include "Task.h"
#include "LockThis.h"

H_BNAMSP

//接收型服务基类
template <typename T>
class CRecvTask : public CTask
{
public:
    CRecvTask(void);
    ~CRecvTask(void);

    void Run(void);

    //Run动作分解
    virtual void initRun(void) {};
    virtual void runTask(T *pMsg) = 0;
    virtual void stopRun(void) {};
    virtual void runSurplusTask(T *pMsg);
    virtual void destroyRun(void) {};
    
    void Join(void);
    void waitStart(void);

protected:
    void addTask(T *pMsg);
    T *newT(void);
    T *newT(const size_t &iCount);
    void setDel(const bool bDel);
    void setArray(const bool bArray);

private:
    void Free(T *pMsg);

private:
    H_DISALLOWCOPY(CRecvTask);
    enum
    {
        RS_RUN = 0,
        RS_STOP,
    };

private:
    bool m_bDel;
    bool m_bArray;
    unsigned int m_uiWait;
    long m_lExit;
    long m_lCount;
    std::queue<T*> m_vcTask;
    pthread_mutex_t m_quLock;
    pthread_cond_t m_objCond;
};

template <typename T>
CRecvTask<T>::CRecvTask(void) : m_bDel(true), m_bArray(false), m_uiWait(H_INIT_NUMBER),
    m_lExit(RS_RUN), m_lCount(H_INIT_NUMBER)
{
    pthread_mutex_init(&m_quLock, NULL);
    pthread_cond_init(&m_objCond, NULL);
}

template <typename T>
CRecvTask<T>::~CRecvTask(void)
{
    Join();
    pthread_cond_destroy(&m_objCond);
    pthread_mutex_destroy(&m_quLock);
}

template <typename T>
void CRecvTask<T>::Free(T *pMsg)
{
    if (m_bDel)
    {
        if (m_bArray)
        {
            H_SafeDelArray(pMsg);
        }
        else
        {
            H_SafeDelete(pMsg);
        }
    }
}

template <typename T>
void CRecvTask<T>::Run(void)
{
    T *pMsg = NULL;
    initRun();

    H_AtomicAdd(&m_lCount, 1);

    while(RS_RUN == H_AtomicGet(&m_lExit))
    {
        if (NULL != pMsg)
        {
            runTask(pMsg);
            Free(pMsg);
            pMsg = NULL;
        }

        CLckThis objLckThis(&m_quLock);
        if (!m_vcTask.empty())
        {
            pMsg = m_vcTask.front();
            m_vcTask.pop();
        }
        else
        {
            ++m_uiWait;
            pthread_cond_wait(&m_objCond, objLckThis.getMutex());
            --m_uiWait;
        }
    }

    stopRun();

    pMsg = NULL;
    while (true)
    {
        if (NULL != pMsg)
        {
            runSurplusTask(pMsg);
            Free(pMsg);
        }

        CLckThis objLckThis(&m_quLock);
        if (!m_vcTask.empty())
        {
            pMsg = m_vcTask.front();
            m_vcTask.pop();
        }
        else
        {
            break;
        }
    }

    destroyRun();

    H_AtomicAdd(&m_lCount, -1);
}

template <typename T>
void CRecvTask<T>::runSurplusTask(T *pMsg)
{
    runTask(pMsg);
}

template <typename T>
void CRecvTask<T>::waitStart(void)
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
void CRecvTask<T>::addTask(T *pMsg)
{
    CLckThis objLckThis(&m_quLock);
    m_vcTask.push(pMsg);
    if (m_uiWait > H_INIT_NUMBER)
    {
        pthread_cond_signal(&m_objCond);
    }    
}

template <typename T>
T *CRecvTask<T>::newT(void)
{
    return new T;
}

template <typename T>
T *CRecvTask<T>::newT(const size_t &iCount)
{
    return new T[iCount];
}

template <typename T>
void CRecvTask<T>::setDel(const bool bDel)
{
    m_bDel = bDel;
}

template <typename T>
void CRecvTask<T>::setArray(const bool bArray)
{
    m_bArray = bArray;
}

template <typename T>
void CRecvTask<T>::Join(void)
{
    if (RS_RUN != H_AtomicGet(&m_lExit))
    {
        return;
    }

    //等待任务队列完成
    H_AtomicSet(&m_lExit, RS_STOP);
    for(;;)
    {
        pthread_cond_broadcast(&m_objCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

H_ENAMSP

#endif//H_SERVICETASK_H_
