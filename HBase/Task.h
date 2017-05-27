
#ifndef H_TASK_H_
#define H_TASK_H_

#include "Macros.h"

H_BNAMSP

class CTask
{
public:
    CTask(void){};
    virtual ~CTask(void){};

    virtual void Run(void) = 0;
};

H_ENAMSP

#endif//H_TASK_H_
