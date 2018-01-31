
#ifndef H_ADJURE_RUNNER_H_
#define H_ADJURE_RUNNER_H_

#include "Adjure.h"

H_BNAMSP

//执行任务请求
class CRunWorkerAdjure : public CAdjure
{
public:
    CRunWorkerAdjure(class CWorker *pWorker) : CAdjure(H_INIT_NUMBER), m_pWorker(pWorker)
    {};
    ~CRunWorkerAdjure(void)
    {};

    class CWorker *getWorker(void)
    {
        return m_pWorker;
    };
private:
    CRunWorkerAdjure(void);

private:
    class CWorker *m_pWorker;
};

H_ENAMSP

#endif//H_ADJURE_RUNNER_H_
