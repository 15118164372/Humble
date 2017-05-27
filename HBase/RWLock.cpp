
#include "RWLock.h"

H_BNAMSP

CRWLock::CRWLock(void)
{
#ifdef H_OS_WIN
    m_lReader_Count = 0; 
    m_bEnter_Writer = false;

    //初始化临界区
    InitializeCriticalSection(&m_Write_Lock);
    InitializeCriticalSection(&m_Read_Lock);
#else
    H_ASSERT((H_RTN_OK == pthread_rwlock_init(&m_RWLock, NULL)), "pthread_rwlock_init error.");
#endif
}

CRWLock::~CRWLock(void)
{
#ifdef H_OS_WIN
    //删除临界区
    DeleteCriticalSection(&m_Write_Lock);
    DeleteCriticalSection(&m_Read_Lock);
#else
    (void)pthread_rwlock_destroy(&m_RWLock);
#endif
}

void CRWLock::rLock(void)
{
#ifdef H_OS_WIN
    //进入临界区
    EnterCriticalSection(&m_Write_Lock);
    if(Locked == InterlockedIncrement(&m_lReader_Count))//原子自加 ,如果读锁未锁定，则锁定临界区
    {
        EnterCriticalSection(&m_Read_Lock);
    }

    //离开临界区
    LeaveCriticalSection(&m_Write_Lock);
#else
    (void)pthread_rwlock_rdlock(&m_RWLock);
#endif
}

void CRWLock::wLock(void)
{
#ifdef H_OS_WIN
    EnterCriticalSection(&m_Write_Lock);
    EnterCriticalSection(&m_Read_Lock);

    m_bEnter_Writer = true;
#else
    (void)pthread_rwlock_wrlock(&m_RWLock);
#endif
}

void CRWLock::unLock(void)
{
#ifdef H_OS_WIN
    if(m_bEnter_Writer)
    {
        m_bEnter_Writer = false;

        LeaveCriticalSection(&m_Read_Lock);
        LeaveCriticalSection(&m_Write_Lock);
    }
    else
    {
        if(unLocked == InterlockedDecrement(&m_lReader_Count))//原子自减,所有读锁离开后解锁临界区
        {
            LeaveCriticalSection(&m_Read_Lock);
        }
    }
#else
    (void)pthread_rwlock_unlock(&m_RWLock);
#endif
}

H_ENAMSP
