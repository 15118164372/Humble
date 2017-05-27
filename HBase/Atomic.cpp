
#include "Atomic.h"

H_BNAMSP

CAtomic::CAtomic(void) : m_lLock(unLocked)
{
    
}

CAtomic::~CAtomic(void)
{

}

void CAtomic::Lock(void)
{
    while(Locked == H_AtomicSet(&m_lLock, Locked))
    { }
}

bool CAtomic::tryLock(void)
{
    return unLocked == H_AtomicSet(&m_lLock, Locked);
}

void CAtomic::unLock(void)
{
    H_AtomicSet(&m_lLock, unLocked);
}

H_ENAMSP
