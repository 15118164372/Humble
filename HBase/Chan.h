
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "Atomic.h"
#include "CirQueue.h"

H_BNAMSP

//消息通道
class CChan
{
public:
    CChan(class CWorkerTask *pTask, const int iCapacity);
    ~CChan(void);

    bool Send(void *pszVal);
    void *Recv(void);

    class CWorkerTask *getTask(void);

    size_t getSize(void);
    size_t getCapacity(void);

private:
    CChan(void);
    H_DISALLOWCOPY(CChan);

private:
    class CWorkerTask *m_pTask;
    CAtomic m_objQuLck;
    CCirQueue m_quData;
};

H_ENAMSP

#endif//H_CHAN_H_
