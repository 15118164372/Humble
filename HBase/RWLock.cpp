
#include "RWLock.h"

H_BNAMSP

CRWLock::CRWLock(void)
{
#ifdef H_OS_WIN
    InitializeSRWLock(&m_RWLock);
#else
    H_ASSERT((H_RTN_OK == pthread_rwlock_init(&m_RWLock, NULL)), "pthread_rwlock_init error.");
#endif
}

CRWLock::~CRWLock(void)
{
#ifdef H_OS_WIN
    
#else
    (void)pthread_rwlock_destroy(&m_RWLock);
#endif
}

void CRWLock::rLock(void)
{
#ifdef H_OS_WIN
    AcquireSRWLockShared(&m_RWLock);
#else
    (void)pthread_rwlock_rdlock(&m_RWLock);
#endif
}

void CRWLock::runLock(void)
{
#ifdef H_OS_WIN
    ReleaseSRWLockShared(&m_RWLock);
#else
    (void)pthread_rwlock_unlock(&m_RWLock);
#endif
}


void CRWLock::wLock(void)
{
#ifdef H_OS_WIN
    AcquireSRWLockExclusive(&m_RWLock);
#else
    (void)pthread_rwlock_wrlock(&m_RWLock);
#endif
}

void CRWLock::wunLock(void)
{
#ifdef H_OS_WIN
    ReleaseSRWLockExclusive(&m_RWLock);
#else
    (void)pthread_rwlock_unlock(&m_RWLock);
#endif
}

H_ENAMSP
