
#ifndef H_COND_H_
#define H_COND_H_

#include "LockThis.h"

H_BNAMSP

//cond
class CCond : public CObject
{
public:
    CCond(void)
    {
#ifndef H_OS_WIN
        pthread_condattr_t attr;

        H_ASSERT(H_RTN_OK == pthread_condattr_init(&attr), "pthread_condattr_init error.");
        H_ASSERT(H_RTN_OK == pthread_cond_init(&m_objCond, &attr), "pthread_cond_init error.");

        (void)pthread_condattr_destroy(&attr);
#endif
    };
    ~CCond(void)
    {
#ifndef H_OS_WIN
        (void)pthread_cond_destroy(&m_objCond);
#endif
    };

    void Wait(CLckThis *pLckThis)
    {
#ifdef H_OS_WIN
        m_objCond.wait(*(pLckThis->getCondLck()));
#else
        H_ASSERT(H_RTN_OK == pthread_cond_wait(&m_objCond, pLckThis->getCondLck()), "pthread_cond_wait error.");
#endif
    };
    //H_RTN_TIMEDOUT  ³¬Ê±·µ»Ø
    int timeWait(CLckThis *pLckThis, unsigned int uiTime)
    {
#ifdef H_OS_WIN 
        auto time(std::chrono::system_clock::now() + std::chrono::milliseconds(uiTime));
        if (std::cv_status::timeout == m_objCond.wait_until(*(pLckThis->getCondLck()), time))
        {
            return H_RTN_TIMEDOUT;
        }
#else
        struct timespec abstime;
        struct timeval now;
        (void)gettimeofday(&now, NULL);
        long nsec = now.tv_usec * 1000 + ((long)uiTime % 1000) * 1000000;
        abstime.tv_sec = now.tv_sec + nsec / 1000000000 + (long)uiTime / 1000;
        abstime.tv_nsec = nsec % 1000000000;
        int iRnt = pthread_cond_timedwait(&m_objCond, pLckThis->getCondLck(), &abstime);
        if (H_RTN_OK == iRnt)
        {
            return H_RTN_OK;
        }
        if (ETIMEDOUT == iRnt)
        {
            return H_RTN_TIMEDOUT;
        }

        H_ASSERT(false, "pthread_cond_timedwait error.");
#endif

        return H_RTN_OK;
    };

    void Signal(void)
    {
#ifdef H_OS_WIN
        m_objCond.notify_one();
#else
        H_ASSERT(H_RTN_OK == pthread_cond_signal(&m_objCond), "pthread_cond_signal error.");
#endif
    };
    void Broadcast(void)
    {
#ifdef H_OS_WIN
        m_objCond.notify_all();
#else
        H_ASSERT(H_RTN_OK == pthread_cond_broadcast(&m_objCond), "pthread_cond_broadcast error.");
#endif
    };

private:
    pthread_cond_t m_objCond;
};

H_ENAMSP

#endif//H_COND_H_
