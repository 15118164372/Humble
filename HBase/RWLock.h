
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
    void runLock(void);

    void wLock(void);    
    void wunLock(void);

private:
    H_DISALLOWCOPY(CRWLock);
#ifdef H_OS_WIN
    SRWLOCK m_RWLock;
#else
    pthread_rwlock_t m_RWLock;
#endif
};

H_ENAMSP

#endif//
