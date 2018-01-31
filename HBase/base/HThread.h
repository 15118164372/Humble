
#ifndef H_THREAD_H_
#define H_THREAD_H_

#include "Task.h"

H_BNAMSP

//Ïß³Ì
class CThread : public CObject
{
public:
    CThread(void) {};
    ~CThread(void) {};
    
    static pthread_t Creat(CTask *pTask) 
    {
        pthread_t pThread;

#ifdef H_OS_WIN
        pThread = (HANDLE)_beginthreadex(NULL, 0, threadCB, pTask, 0, NULL);
        H_ASSERT(pThread, "_beginthreadex error.");
#else
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        H_ASSERT((H_RTN_OK == pthread_create(&pThread, &attr, threadCB, (void*)pTask)), "pthread_create error.");
        pthread_attr_destroy(&attr);
#endif

        return pThread;
    };
    static void Join(pthread_t &pThread)
    {
#ifdef H_OS_WIN
        (void)WaitForSingleObject(pThread, INFINITE);
#else
        (void)pthread_join(pThread, NULL);
#endif
    };

private:
#ifdef H_OS_WIN
    static unsigned int __stdcall CThread::threadCB(void *pArg)
#else
    static void *threadCB(void *pArg)
#endif
    {
        CTask *pTask = (CTask *)pArg;
        if (NULL != pTask)
        {
            pTask->Run();
            pTask->afterRun();
        }

        return NULL;
    }
};

H_ENAMSP

#endif//H_THREAD_H_
