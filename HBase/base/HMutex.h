
#ifndef H_MUTEX_H_
#define H_MUTEX_H_

#include "HObject.h"

H_BNAMSP

//mutex
class CMutex : public CObject
{
public:
    CMutex(void)
    {
#ifndef H_OS_WIN
        pthread_mutexattr_t attr;

        H_ASSERT(H_RTN_OK == pthread_mutexattr_init(&attr), "pthread_mutexattr_init error.");
        H_ASSERT(H_RTN_OK == pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK), "pthread_mutexattr_settype error.");
        H_ASSERT(H_RTN_OK == pthread_mutex_init(&m_objMutex, &attr), "pthread_mutex_init error.");

        (void)pthread_mutexattr_destroy(&attr);
#endif
    };
    ~CMutex(void)
    {
#ifndef H_OS_WIN
        (void)pthread_mutex_destroy(&m_objMutex);
#endif
    };

    void Lock(void)
    {
#ifdef H_OS_WIN
        m_objMutex.lock();
#else
        H_ASSERT(H_RTN_OK == pthread_mutex_lock(&m_objMutex), "pthread_mutex_lock error.");
#endif
    };
    void unLock(void)
    {
#ifdef H_OS_WIN
        m_objMutex.unlock();
#else
        H_ASSERT(H_RTN_OK == pthread_mutex_unlock(&m_objMutex), "pthread_mutex_unlock error.");
#endif
    };

    friend class CLckThis;

protected:
    pthread_mutex_t *getMutex(void)
    {
        return &m_objMutex;
    };

private:
    pthread_mutex_t m_objMutex;
};

H_ENAMSP

#endif//H_MUTEX_H_
