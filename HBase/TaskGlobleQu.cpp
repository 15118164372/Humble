
#include "TaskGlobleQu.h"

H_BNAMSP

SINGLETON_INIT(CTaskGlobleQu)
CTaskGlobleQu objTaskGlobleQu(H_MAXTASKNUM);

CTaskGlobleQu::CTaskGlobleQu(const int iCapacity) : m_uiWait(H_INIT_NUMBER), m_objQu(iCapacity)
{
    pthread_mutex_init(&m_objMutex, NULL);
    pthread_cond_init(&m_objCond, NULL);
}

CTaskGlobleQu::~CTaskGlobleQu()
{
    pthread_cond_destroy(&m_objCond);
    pthread_mutex_destroy(&m_objMutex);
}

H_ENAMSP
