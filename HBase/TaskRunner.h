
#ifndef H_TASKRUNNER_H_
#define H_TASKRUNNER_H_

#include "TaskLazy.h"
#include "TaskWorker.h"

H_BNAMSP

class CTaskRunner : public CTaskLazy<CTaskWorker>
{
public:
    CTaskRunner(void) : CTaskLazy<CTaskWorker>(H_QULENS_WORKER),
        m_usIndex(H_INIT_NUMBER), m_uiStatus(H_INIT_NUMBER)
    {
        setDel(false);
    };
    ~CTaskRunner(void) 
    {

    };

    H_INLINE void runTask(CTaskWorker *pMsg)
    {
        pMsg->Run();
        H_AtomicSet(&m_uiStatus, H_INIT_NUMBER);
    };
    H_INLINE void addWorker(CTaskWorker *pWorker)
    {
        pWorker->setStatus(1);
        addTask(pWorker);
    };

    H_INLINE unsigned int getStatus(void)
    {
        return H_AtomicGet(&m_uiStatus);
    };
    H_INLINE void setBusy(void)
    {
        H_AtomicSet(&m_uiStatus, 1);
    };
    void setIndex(const unsigned short &usIndex)
    {
        m_usIndex = usIndex;
    };

private:
    H_DISALLOWCOPY(CTaskRunner);

private:
    unsigned short m_usIndex;
    unsigned int m_uiStatus;
};

H_ENAMSP

#endif//H_TASKRUNNER_H_
