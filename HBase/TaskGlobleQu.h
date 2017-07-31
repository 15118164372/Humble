
#ifndef H_TASKGLOBLEQU_H_
#define H_TASKGLOBLEQU_H_

#include "CirQueue.h"
#include "Singleton.h"
#include "LockThis.h"
#include "Atomic.h"

H_BNAMSP

class CTaskGlobleQu : public CSingleton<CTaskGlobleQu>
{
public:
    CTaskGlobleQu(const int iCapacity);
    ~CTaskGlobleQu(void);

    friend class CChan;
    friend class CTaskRunner;

protected:
    unsigned int m_uiWait;
    CCirQueue m_objQu;
    CAtomic m_objQuLock;
    pthread_mutex_t m_objMutex;
    pthread_cond_t m_objCond;

private:
    CTaskGlobleQu(void);
    H_DISALLOWCOPY(CTaskGlobleQu);
};

H_ENAMSP

#endif//H_TASKGLOBLEQU_H_
