
#include "NetBase.h"
#include "EvBuffer.h"

H_BNAMSP

CNetBase::CNetBase(void) : m_uiCount(H_INIT_NUMBER), m_pStopOrderBev(NULL), m_pOrderBev(NULL), m_pBase(NULL)
{
#ifdef H_OS_WIN
    WORD wVersionReq(MAKEWORD(2, 2));
    WSAData wsData;
    (void)WSAStartup(wVersionReq, &wsData);

    (void)evthread_use_windows_threads();
    struct event_config *pEvCfg(event_config_new());
    H_ASSERT(NULL != pEvCfg, "event_config_new error.");
    if (H_RTN_OK != event_config_set_flag(pEvCfg, EVENT_BASE_FLAG_STARTUP_IOCP))
    {
        H_Printf("%s", "event_config_set_flag set EVENT_BASE_FLAG_STARTUP_IOCP error.");
    }

    m_pBase = event_base_new_with_config(pEvCfg);
    event_config_free(pEvCfg);
#else
    m_pBase = event_base_new();
#endif
    H_ASSERT(NULL != m_pBase, "event_base_new error.");
    H_Printf("kernel event notification mechanism %s", event_base_get_method(m_pBase));

    memset(m_sockStopOrder, H_INVALID_SOCK, sizeof(m_sockStopOrder));
    memset(m_sockOrder, H_INVALID_SOCK, sizeof(m_sockOrder));
    H_ASSERT(H_RTN_OK == H_SockPair(m_sockStopOrder), "create sock pair error.");
    H_ASSERT(H_RTN_OK == H_SockPair(m_sockOrder), "create sock pair error.");

    m_pStopOrderBev = bufferevent_socket_new(m_pBase, m_sockStopOrder[0], BEV_OPT_CLOSE_ON_FREE);
    H_ASSERT(NULL != m_pStopOrderBev, "bufferevent_socket_new error.");
    bufferevent_setcb(m_pStopOrderBev, stopReadCB, NULL, NULL, m_pBase);
    (void)bufferevent_enable(m_pStopOrderBev, EV_READ);

    m_pOrderBev = bufferevent_socket_new(m_pBase, m_sockOrder[0], BEV_OPT_CLOSE_ON_FREE);
    H_ASSERT(NULL != m_pOrderBev, "bufferevent_socket_new error.");
    bufferevent_setcb(m_pOrderBev, orderReadCB, NULL, NULL, this);
    (void)bufferevent_enable(m_pOrderBev, EV_READ);
}

CNetBase::~CNetBase(void)
{
    if (NULL != m_pStopOrderBev)
    {
        bufferevent_free(m_pStopOrderBev);
        m_pStopOrderBev = NULL;
    }
    if (NULL != m_pOrderBev)
    {
        bufferevent_free(m_pOrderBev);
        m_pOrderBev = NULL;
    }

    if (H_INVALID_SOCK != m_sockStopOrder[1])
    {
        evutil_closesocket(m_sockStopOrder[1]);
        m_sockStopOrder[1] = H_INVALID_SOCK;
    }
    if (H_INVALID_SOCK != m_sockOrder[1])
    {
        evutil_closesocket(m_sockOrder[1]);
        m_sockOrder[1] = H_INVALID_SOCK;
    }

    if (NULL != m_pBase)
    {
        event_base_free(m_pBase);
        m_pBase = NULL;
    }

#ifdef H_OS_WIN 
    (void)WSACleanup();
#endif
}

void CNetBase::stopReadCB(struct bufferevent *bev, void *arg)
{
    CEvBuffer objEvBuffer;
    objEvBuffer.setEvBuf(bev);
    if (0 == objEvBuffer.getTotalLens())
    {
        return;
    }

    (void)event_base_loopbreak((struct event_base *)arg);
}

void CNetBase::orderReadCB(struct bufferevent *bev, void *arg)
{
    CEvBuffer objEvBuffer;
    objEvBuffer.setEvBuf(bev);

    size_t iReadLens(((CNetBase *)arg)->onOrder(&objEvBuffer));

    objEvBuffer.delBuffer(iReadLens);
}

void CNetBase::Run(void)
{
    onStart();

    H_AtomicAdd(&m_uiCount, 1);
    (void)event_base_dispatch(m_pBase);
    H_AtomicAdd(&m_uiCount, -1);
}

void CNetBase::waitStart(void)
{
    for (;;)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_uiCount))
        {
            return;
        }

        H_Sleep(10);
    }
}

void CNetBase::Join(void)
{
    if (H_INIT_NUMBER == H_AtomicGet(&m_uiCount))
    {
        return;
    }

    int iStop = 1;
    m_objStopLock.Lock();
    (void)H_SockWrite(m_sockStopOrder[1], (const char*)&iStop, sizeof(iStop));
    m_objStopLock.unLock();

    while (true)
    {
        if (H_INIT_NUMBER == H_AtomicGet(&m_uiCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

H_ENAMSP
