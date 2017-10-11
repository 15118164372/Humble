
#ifndef H_TASKRUNNER_H_
#define H_TASKRUNNER_H_

#include "Task.h"
#include "TaskWorker.h"

H_BNAMSP

class CTaskRunner : public CTask
{
public:
    CTaskRunner(void);
    ~CTaskRunner(void);

    void Run(void);
    
    void Join(void);
    void waitStart(void);
    
    void setIndex(const unsigned short &usIndex)
    {
        m_usIndex = usIndex;
    };

    void setAlarmTime(const double &dMs)
    {
        m_dAlarmTime = dMs;
    };

    void setAdjustLoad(unsigned short &usToIndex);

private:
    void runSurplus(void);
    bool runTask(class CTaskWorker *pTask, class CClock *pClock, double *pRunTime);
    void adjustInGloble(struct TaskQueue *pTaskQueue, class CTaskWorker *pTask);
    void adjustLoad(class CTaskWorker *pTask, const unsigned short &usIndex);

private:
    H_DISALLOWCOPY(CTaskRunner);

private:
    unsigned short m_usIndex;
    unsigned short m_usToIndex;
    unsigned int m_uiAdjustLoad;
    double m_dAlarmTime;
    long m_lExit;
    long m_lCount;
};

H_ENAMSP

#endif//H_TASKRUNNER_H_
