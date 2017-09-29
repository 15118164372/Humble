
#ifndef H_NETBASE_H_
#define H_NETBASE_H_

#include "Task.h"
#include "Atomic.h"
#include "EvBuffer.h"
#include "Funcs.h"
#include "Log.h"

H_BNAMSP

//网络基类
class CNetBase : public CTask
{
public:
    CNetBase(void);
    ~CNetBase(void);

    //返回读取的字节数
    virtual size_t onOrder(CEvBuffer *pEvBuffer) 
    {
        return H_INIT_NUMBER;
    };
    virtual void onStart(void) {};

    void Run(void);
    void Join(void);
    void waitStart(void);

    static void stopReadCB(struct bufferevent *bev, void *arg);
    static void orderReadCB(struct bufferevent *bev, void *arg);

protected:
    H_INLINE bool sendOrder(const void *pBuf, const size_t &iLens)
    {
        m_objOrderLock.Lock();
        int iRtn(H_SockWrite(m_sockOrder[1], (const char*)pBuf, iLens));
        m_objOrderLock.unLock();
        if (H_RTN_OK != iRtn)
        {
            H_LOG(LOGLV_ERROR, "%s", H_SockError2Str(iRtn));
            return false;
        }

        return true;
    };
    H_INLINE struct event_base *getBase(void)
    {
        return m_pBase;
    };

private:
    H_DISALLOWCOPY(CNetBase);

private:
    unsigned int m_uiCount;
    struct bufferevent *m_pStopOrderBev;
    struct bufferevent *m_pOrderBev;
    struct event_base *m_pBase;
    H_SOCK m_sockStopOrder[2];
    H_SOCK m_sockOrder[2];
    CAtomic m_objStopLock;
    CAtomic m_objOrderLock;
};

H_ENAMSP

#endif//H_NETBASE_H_
