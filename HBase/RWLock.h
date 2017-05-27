
#ifndef H_RWLOCK_H_
#define H_RWLOCK_H_

#include "Macros.h"

H_BNAMSP

//¶ÁÐ´Ëø
class CRWLock
{
public:
    CRWLock(void);
    ~CRWLock(void);

    void rLock(void);
    void wLock(void);
    void unLock(void);

private:
    H_DISALLOWCOPY(CRWLock);
#ifdef H_OS_WIN
    enum
    {
        unLocked = 0,
        Locked = 1,
    };
    CRITICAL_SECTION m_Write_Lock;
    CRITICAL_SECTION m_Read_Lock;
    volatile LONG m_lReader_Count;
    volatile bool m_bEnter_Writer;
#else
    pthread_rwlock_t m_RWLock;
#endif
};

H_ENAMSP

#endif//
