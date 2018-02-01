#ifndef H_SEM_H_
#define H_SEM_H_

#include "HObject.h"

H_BNAMSP

//sem
class CSem : public CObject
{
public:
    CSem(void)
    {
        initSem(H_INIT_NUMBER);
    };
    //uiValue 信号量的初始计数
    explicit CSem(const unsigned int &uiValue)
    {
        initSem(uiValue);
    };
    ~CSem(void) 
    {
#ifdef H_OS_WIN
        (void)CloseHandle(m_stSem);
#else
        (void)sem_destroy(&m_stSem);
#endif
    };

    void Wait(void)
    {
#ifdef H_OS_WIN
        H_ASSERT(WAIT_OBJECT_0 == WaitForSingleObject(m_stSem, INFINITE), "WaitForSingleObject error.");
#else
        int iRtn;
        do 
        {
            iRtn = sem_wait(&m_stSem);
        } while (-1 == iRtn && EINTR == errno);

        H_ASSERT(H_RTN_OK == iRtn, "sem_wait error.");
#endif
    };
    int tryWait(void)
    {
#ifdef H_OS_WIN
        DWORD dwRnt = WaitForSingleObject(m_stSem, 0);
        if (WAIT_OBJECT_0 == dwRnt)
        {
            return H_RTN_OK;
        }
        if (WAIT_TIMEOUT == dwRnt)
        {
            return H_RTN_TIMEDOUT;
        }

        H_ASSERT(false, "WaitForSingleObject error.");
#else
        int iRtn;
        do
        {
            iRtn = sem_trywait(&m_stSem);
        }
        while (-1 == iRtn && EINTR == errno);

        if (H_RTN_OK == iRtn)
        {
            return H_RTN_OK;
        }
        if (EAGAIN == errno)
        {
            return H_RTN_TIMEDOUT;
        }

        H_ASSERT(false, "sem_trywait error.");
#endif

        return H_RTN_OK;
    };
    H_INLINE void Signal(void)
    {
#ifdef H_OS_WIN
        H_ASSERT(ReleaseSemaphore(m_stSem, 1, NULL), "ReleaseSemaphore error.");
#else
        H_ASSERT(H_RTN_OK == sem_post(&m_stSem), "sem_post error.");
#endif
    };

private:
    void initSem(const unsigned int &uiVal)
    {
#ifdef H_OS_WIN
        m_stSem = CreateSemaphore(NULL, uiVal, INT_MAX, NULL);
        H_ASSERT(NULL != m_stSem, "CreateSemaphore error.");
#else
        H_ASSERT(H_RTN_OK == sem_init(&m_stSem, 0, uiVal), "sem_init error.");
#endif
    };

private:
    sem_t m_stSem;
};

H_ENAMSP

#endif//H_SEM_H_