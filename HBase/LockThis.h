
#ifndef H_LCKTHIS_H_
#define H_LCKTHIS_H_

#include "Macros.h"

H_BNAMSP

class CLckThis
{
public:
    ~CLckThis(void)
    {
#ifndef H_OS_WIN
        (void)pthread_mutex_unlock(m_pMutex);
#endif
    };

#ifdef H_OS_WIN
    CLckThis(pthread_mutex_t *pMutex) : m_objLck(*pMutex)
    {

    };

    std::unique_lock <std::mutex> &getMutex(void)
    {
        return m_objLck;
    };
#else
    CLckThis(pthread_mutex_t *pMutex) : m_pMutex(pMutex)
    {
        (void)pthread_mutex_lock(m_pMutex);
    };

    pthread_mutex_t *getMutex(void)
    {
        return m_pMutex;
    };
#endif // H_OS_WIN

private:
    H_DISALLOWCOPY(CLckThis);
    CLckThis(void);

private:
#ifdef H_OS_WIN
    std::unique_lock <std::mutex> m_objLck;
#else
    pthread_mutex_t *m_pMutex;
#endif
};

H_ENAMSP

#endif // H_LCKTHIS_H_
