
#ifndef H_TASK_H_
#define H_TASK_H_

#include "TimeElapsed.h"

H_BNAMSP

//任务基类
class CTask : public CObject
{
public:
    CTask(void)
    {};
    ~CTask(void)
    {};

    virtual void Run(void) = 0;
    virtual void afterRun(void) {};
};

H_ENAMSP

#endif//H_TASK_H_
