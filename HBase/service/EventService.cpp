
#include "EventService.h"
#include "NETAddr.h"
#include "Utils.h"
#include "Log.h"
#include "event2/thread.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"

H_BNAMSP

#define H_READ_MAXALLOCS 4 //每次读取内存最多重分配几次
#define H_EXPANDBUFMUL   2 //buffer扩张倍数

static void evbufferDrain(struct bufferevent *pBev)
{
    struct evbuffer *pReadEv(bufferevent_get_input(pBev));
    size_t iBufLens(evbuffer_get_length(pReadEv));
    if (iBufLens >= H_ONEK)
    {
        (void)evbuffer_drain(pReadEv, iBufLens);
    }
}
static void stopEventLoop(struct bufferevent *pBev, void *pArg)
{
    evbufferDrain(pBev);
    (void)event_base_loopbreak((struct event_base *)pArg);
}
static void onAdjureEvent(struct bufferevent *pBev, void *pArg)
{
    evbufferDrain(pBev);

    CEventService *pService((CEventService *)pArg);
    CAdjure *pAdjure(NULL);
    while (NULL != (pAdjure = (CAdjure *)pService->getAdjure()))
    {
        pService->onAdjure(pAdjure);
        pService->afterAdjure(pAdjure);
    }
}

CEventService::CEventService(const size_t &uiCapacity) : m_bStop(false), m_bFreeQuAdjure(true),
    m_iRunFlage(H_INIT_NUMBER), m_objAdjureQu(uiCapacity)
{
#ifdef H_OS_WIN
    WSAData wsData;
    WORD wVersionReq(MAKEWORD(2, 2));
    (void)WSAStartup(wVersionReq, &wsData);

    (void)evthread_use_windows_threads();
    struct event_config *pEvCfg(event_config_new());
    H_ASSERT(NULL != pEvCfg, "event_config_new error.");
    H_ASSERT(H_RTN_OK == event_config_set_flag(pEvCfg, EVENT_BASE_FLAG_STARTUP_IOCP),
        "event_config_set_flag set EVENT_BASE_FLAG_STARTUP_IOCP error.");
    
    m_pBase = event_base_new_with_config(pEvCfg);
    event_config_free(pEvCfg);
#else
    m_pBase = event_base_new();
#endif
    H_ASSERT(NULL != m_pBase, "event_base_new error.");

    memset(m_sockStop, H_INVALID_SOCK, sizeof(m_sockStop));
    memset(m_sockAdjure, H_INVALID_SOCK, sizeof(m_sockAdjure));
    H_ASSERT(H_RTN_OK == sockPair(m_sockStop), "create sock pair error.");
    H_ASSERT(H_RTN_OK == sockPair(m_sockAdjure), "create sock pair error.");

    m_pStopEvent = bufferevent_socket_new((struct event_base *)m_pBase, m_sockStop[0], BEV_OPT_CLOSE_ON_FREE);
    H_ASSERT(NULL != m_pStopEvent, "bufferevent_socket_new error.");
    bufferevent_setcb((struct bufferevent *)m_pStopEvent, stopEventLoop, NULL, NULL, m_pBase);
    (void)bufferevent_enable((struct bufferevent *)m_pStopEvent, EV_READ);

    m_pAdjureEvent = bufferevent_socket_new((struct event_base *)m_pBase, m_sockAdjure[0], BEV_OPT_CLOSE_ON_FREE);
    H_ASSERT(NULL != m_pAdjureEvent, "bufferevent_socket_new error.");
    bufferevent_setcb((struct bufferevent *)m_pAdjureEvent, onAdjureEvent, NULL, NULL, this);
    (void)bufferevent_enable((struct bufferevent *)m_pAdjureEvent, EV_READ);
}
CEventService::~CEventService(void)
{
    if (NULL != m_pStopEvent)
    {
        bufferevent_free((struct bufferevent *)m_pStopEvent);
        m_pStopEvent = NULL;
    }
    if (NULL != m_pAdjureEvent)
    {
        bufferevent_free((struct bufferevent *)m_pAdjureEvent);
        m_pAdjureEvent = NULL;
    }

    if (H_INVALID_SOCK != m_sockStop[1])
    {
        evutil_closesocket(m_sockStop[1]);
        m_sockStop[1] = H_INVALID_SOCK;
    }
    if (H_INVALID_SOCK != m_sockAdjure[1])
    {
        evutil_closesocket(m_sockAdjure[1]);
        m_sockAdjure[1] = H_INVALID_SOCK;
    }

    if (NULL != m_pBase)
    {
        event_base_free((struct event_base *)m_pBase);
        m_pBase = NULL;
    }

    if (m_bFreeQuAdjure)
    {
        CAdjure *pAdjure(NULL);
        while (NULL != (pAdjure = (CAdjure *)getAdjure()))
        {
            H_SafeDelete(pAdjure);
        }
    }

#ifdef H_OS_WIN 
    (void)WSACleanup();
#endif
}

void CEventService::setSVName(const char *pszName)
{
    m_strServiceName = pszName;
}
const char *CEventService::getSVName(void)
{
    return m_strServiceName.c_str();
}
void CEventService::setFreeQuAdjure(const bool &bFree)
{
    m_bFreeQuAdjure = bFree;
}

void CEventService::Run(void)
{
    onStart();
    ++m_iRunFlage;
    H_Printf("start service: %s", m_strServiceName.c_str());

    (void)event_base_dispatch((struct event_base *)m_pBase);

    onStop();
    H_Printf("stop service: %s", m_strServiceName.c_str());
    --m_iRunFlage;
}
void CEventService::Adjure(CAdjure *pAdjure)
{
    m_objAdjureQu.Push(pAdjure);

    const char *pszAdjure = "";
    (void)send(m_sockAdjure[1], pszAdjure, 1, 0);
}
void CEventService::onStop(void)
{
    CAdjure *pAdjure(NULL);
    while (NULL != (pAdjure = getAdjure()))
    {
        onAdjure(pAdjure);
        afterAdjure(pAdjure);
    }
}
void CEventService::Stop(void)
{
    m_bStop = true;
    const char *pszStop = "";
    (void)send(m_sockStop[1], pszStop, 1, 0);

    unsigned int uiCount(H_INIT_NUMBER);
    while (H_INIT_NUMBER != m_iRunFlage)
    {
        H_Sleep(10);
        uiCount += 10;
        if (uiCount >= 5000)
        {
            H_Printf("stop service %s time out.", m_strServiceName.c_str());
            uiCount = H_INIT_NUMBER;
        }
    }
}
void CEventService::waitStart(void)
{
    unsigned int uiCount(H_INIT_NUMBER);
    while (H_INIT_NUMBER == m_iRunFlage)
    {
        H_Sleep(10);
        uiCount += 10;
        if (uiCount >= 5000)
        {
            H_Printf("start service %s time out.", m_strServiceName.c_str());
            uiCount = H_INIT_NUMBER;
        }
    }
}
CAdjure *CEventService::getAdjure(void)
{
    return (CAdjure *)m_objAdjureQu.Pop();
};

int CEventService::creatListener(H_SOCK &fdListener)
{
    CNETAddr objListen_addr;
    if (H_RTN_OK != objListen_addr.setAddr("127.0.0.1", 0))
    {
        return H_RTN_FAILE;
    }

    fdListener = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == fdListener)
    {
        return H_RTN_FAILE;
    }
    if (H_RTN_FAILE == bind(fdListener, objListen_addr.getAddr(), (int)objListen_addr.getAddrSize()))
    {
        evutil_closesocket(fdListener);
        return H_RTN_FAILE;
    }
    if (H_RTN_FAILE == listen(fdListener, 1))
    {
        evutil_closesocket(fdListener);
        return H_RTN_FAILE;
    }

    return H_RTN_OK;

}
int CEventService::sockPair(H_SOCK acSock[2])
{
    H_SOCK fdListener(H_INVALID_SOCK);
    H_SOCK fdConnector(H_INVALID_SOCK);
    H_SOCK fdAcceptor(H_INVALID_SOCK);
    ev_socklen_t iSize(H_INIT_NUMBER);
    struct sockaddr_in connect_addr;
    struct sockaddr_in listen_addr;
    int iKeepAlive(1);
    if (H_RTN_OK != creatListener(fdListener))
    {
        return H_RTN_FAILE;
    }

    fdConnector = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == fdConnector)
    {
        evutil_closesocket(fdListener);
        return H_RTN_FAILE;
    }
    iSize = sizeof(connect_addr);
    if (H_RTN_FAILE == getsockname(fdListener, (struct sockaddr *) &connect_addr, &iSize))
    {
        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);
        return H_RTN_FAILE;
    }
    if (iSize != sizeof(connect_addr))
    {
        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);
        return H_RTN_FAILE;
    }
    if (H_RTN_FAILE == connect(fdConnector, (struct sockaddr *) &connect_addr, sizeof(connect_addr)))
    {
        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);
        return H_RTN_FAILE;
    }
    iSize = sizeof(listen_addr);
    fdAcceptor = accept(fdListener, (struct sockaddr *) &listen_addr, &iSize);
    if (H_INVALID_SOCK == fdAcceptor)
    {
        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);
        return H_RTN_FAILE;
    }
    evutil_closesocket(fdListener);
    if (H_RTN_FAILE == getsockname(fdConnector, (struct sockaddr *) &connect_addr, &iSize))
    {
        evutil_closesocket(fdAcceptor);
        evutil_closesocket(fdConnector);
        return H_RTN_FAILE;
    }

    if (iSize != sizeof(connect_addr)
        || listen_addr.sin_family != connect_addr.sin_family
        || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
        || listen_addr.sin_port != connect_addr.sin_port)
    {
        evutil_closesocket(fdAcceptor);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    int iFlag(1);
    (void)setsockopt(fdConnector, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag));
    (void)setsockopt(fdAcceptor, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag));
    (void)setsockopt(fdConnector, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive));
    (void)setsockopt(fdAcceptor, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive));
    (void)evutil_make_socket_nonblocking(fdAcceptor);
    (void)evutil_make_socket_nonblocking(fdConnector);
    acSock[0] = fdAcceptor;
    acSock[1] = fdConnector;

    return H_RTN_OK;
}

READ_RETURN CEventService::sockRead(H_SOCK &sock, CDynaBuffer *pSockBuf)
{
    int iRet(H_RTN_OK);
    int iAllocsNum(H_INIT_NUMBER);
    size_t uiSurplus(H_INIT_NUMBER);
    READ_RETURN emRet(READ_NO_DATA_RECEIVED);

    while (true)
    {
        //内存不足,重新分配
        if (pSockBuf->Full())
        {
            //超过限制
            if (H_READ_MAXALLOCS == iAllocsNum)
            {
                return emRet;
            }

            ++iAllocsNum;
            if (!pSockBuf->New(pSockBuf->getTotalLens() * H_EXPANDBUFMUL))
            {
                H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
                return READ_MEMORY_ERROR;
            }
        }

        uiSurplus = pSockBuf->getSurplusLens();
        iRet = recv(sock, pSockBuf->getBuffer() + pSockBuf->getLens(), (int)uiSurplus, 0);
        if (iRet > H_INIT_NUMBER)
        {
            emRet = READ_DATA_RECEIVED;
            pSockBuf->addLens(iRet);
            if ((size_t)iRet == uiSurplus)
            {
                continue;
            }

            break;
        }
        //另一端已关闭
        if (H_INIT_NUMBER == iRet)
        {
            return READ_ERROR;
        }

        iRet = H_SockError();
        if (IS_EAGAIN(iRet))
        {
            break;
        }

        return READ_ERROR;
    }

    return emRet;
}

H_ENAMSP
