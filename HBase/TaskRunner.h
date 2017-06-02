
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

private:
    void runSurplus(void);

private:
    H_DISALLOWCOPY(CTaskRunner);

private:
    unsigned short m_usIndex;
    long m_lExit;
    long m_lCount;
};

H_ENAMSP

#endif//H_TASKRUNNER_H_
