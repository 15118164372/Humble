
#ifndef H_RWLOCK_H_
#define H_RWLOCK_H_

#include "HObject.h"

H_BNAMSP

//��д��
class CRWLock : public CObject
{
public:
    CRWLock(void) 
    {
#ifdef H_OS_WIN
        InitializeSRWLock(&m_RWLock);
#else
        H_ASSERT((H_RTN_OK == pthread_rwlock_init(&m_RWLock, NULL)), "pthread_rwlock_init error.");
#endif
    };
    ~CRWLock(void) 
    {
#ifndef H_OS_WIN
        (void)pthread_rwlock_destroy(&m_RWLock);
#endif
    };

    //��ȡ���������Դ
    void rLock(void) 
    {
#ifdef H_OS_WIN
        AcquireSRWLockShared(&m_RWLock);
#else
        (void)pthread_rwlock_rdlock(&m_RWLock);
#endif
    };
    //��ȡ�߽�����ȡ��Դ���ͷŶ���Դ��ռ��
    void runLock(void) 
    {
#ifdef H_OS_WIN
        ReleaseSRWLockShared(&m_RWLock);
#else
        (void)pthread_rwlock_unlock(&m_RWLock);
#endif
    };
    //д��������д��Դ
    void wLock(void) 
    {
#ifdef H_OS_WIN
        AcquireSRWLockExclusive(&m_RWLock);
#else
        (void)pthread_rwlock_wrlock(&m_RWLock);
#endif
    };
    //д����д��Դ��ϣ��ͷŶ���Դ��ռ��
    void wunLock(void) 
    {
#ifdef H_OS_WIN
        ReleaseSRWLockExclusive(&m_RWLock);
#else
        (void)pthread_rwlock_unlock(&m_RWLock);
#endif
    };

private:
#ifdef H_OS_WIN
    SRWLOCK m_RWLock;
#else
    pthread_rwlock_t m_RWLock;
#endif
};

H_ENAMSP

#endif//
