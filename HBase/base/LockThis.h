
#ifndef H_LCKTHIS_H_
#define H_LCKTHIS_H_

#include "HMutex.h"

H_BNAMSP

//·¶Î§Ëø
class CLckThis : public CObject
{
public:
#ifdef H_OS_WIN
    explicit CLckThis(CMutex *pMutex) : m_pMutex(pMutex), m_objLck(*(pMutex->getMutex()))
    {};
#else
    explicit CLckThis(CMutex *pMutex) : m_pMutex(pMutex)
    {
        m_pMutex->Lock();
    };
#endif
    ~CLckThis(void)
    {
#ifndef H_OS_WIN
        m_pMutex->unLock();
#endif
    };

    friend class CCond;

protected:
#ifdef H_OS_WIN
    std::unique_lock <std::mutex> *getCondLck(void)
    {
        return &m_objLck;
    };
#else
    pthread_mutex_t *getCondLck(void)
    {
        return m_pMutex->getMutex();
    };
#endif   

private:
    CLckThis(void);
    CMutex *m_pMutex;
#ifdef H_OS_WIN
    std::unique_lock <std::mutex> m_objLck;
#endif
};

H_ENAMSP

#endif // H_LCKTHIS_H_
