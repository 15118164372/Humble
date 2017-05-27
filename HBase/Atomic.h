
#ifndef H_ATOMICLOCK_H_
#define H_ATOMICLOCK_H_

#include "Macros.h"

H_BNAMSP

//Ô­×ÓËø
class CAtomic
{
public:
    CAtomic(void);
    ~CAtomic(void);

    void Lock(void);
    bool tryLock(void);
    void unLock(void);

private:
    H_DISALLOWCOPY(CAtomic);
    enum
    {
        unLocked = 0,
        Locked = 1,
    };

private:
#ifdef H_OS_WIN
    volatile long m_lLock;
#else
    int m_lLock;
#endif
};

H_ENAMSP

#endif//H_ATOMICLOCK_H_
