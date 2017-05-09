
#ifndef H_THREAD_H_
#define H_THREAD_H_

#include "Task.h"

H_BNAMSP

//Ïß³Ì
class CThread
{
public:
    CThread(void);
    ~CThread(void);

    static pthread_t Creat(CTask *pTask);
    static void Join(pthread_t pThread);

private:
#ifdef H_OS_WIN
    static unsigned int __stdcall threadCB(void *arg);
#else
    static void *threadCB(void *arg);    
#endif
};

H_ENAMSP

#endif//H_THREAD_H_
