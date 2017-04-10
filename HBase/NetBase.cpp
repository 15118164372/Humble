
#include "NetBase.h"
#include "Funcs.h"
#include "EvBuffer.h"

H_BNAMSP

CNetBase::CNetBase(void) : m_uiCount(H_INIT_NUMBER), m_uiReadyStop(RSTOP_NONE),
    m_uiSession(H_INIT_NUMBER)
{
#ifdef H_OS_WIN
    WORD wVersionReq;
    WSAData wsData;
    wVersionReq = MAKEWORD(2, 2);
    (void)WSAStartup(wVersionReq, &wsData);
#endif

    H_ASSERT(H_RTN_OK == H_SockPair(m_sockOrder), "create sock pair error.");
    m_pBase = event_base_new();
    H_ASSERT(NULL != m_pBase, "event_base_new error.");
    m_pOrderBev = bufferevent_socket_new(getBase(), m_sockOrder[0], BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(m_pOrderBev, orderReadCB, NULL, NULL, this);
    (void)bufferevent_enable(m_pOrderBev, EV_READ);
}

CNetBase::~CNetBase(void)
{
    sessionit itSession;
    for (itSession = m_mapSession.begin(); m_mapSession.end() != itSession; ++itSession)
    {
        freeSession(itSession->second);
    }
    m_mapSession.clear();

    if (NULL != m_pOrderBev)
    {
        bufferevent_free(m_pOrderBev);
        m_pOrderBev = NULL;
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

void CNetBase::freeSession(H_Session *pSession)
{
    bufferevent_free(pSession->pBev);
    H_SafeDelete(pSession);
}

void CNetBase::sendOrder(const void *pBuf, const size_t iLens)
{
    m_objOrderLock.Lock();
    (void)H_SockWrite(m_sockOrder[1], (const char*)pBuf, iLens);
    m_objOrderLock.unLock();
}

void CNetBase::setReadyStop(const unsigned int uiStatus)
{
    H_AtomicSet(&m_uiReadyStop, uiStatus);
}
unsigned int CNetBase::getReadyStop(void)
{
    return H_AtomicGet(&m_uiReadyStop);
}

H_Session *CNetBase::getSession(const H_SOCK &sock)
{
    sessionit itSession;
    itSession = m_mapSession.find(sock);
    if (m_mapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

void CNetBase::delSession(H_SOCK &sock)
{
    sessionit itSession;
    itSession = m_mapSession.find(sock);
    if (m_mapSession.end() == itSession)
    {
        return;
    }

    onClose(itSession->second);

    freeSession(itSession->second);
    m_mapSession.erase(itSession);
}

void CNetBase::addSession(H_SOCK &sock, H_Session *pSesson, const bool &bAccept)
{
    m_mapSession[sock] = pSesson;
    onLinked(pSesson, bAccept);
}

void CNetBase::removByType(const unsigned short &usSockType)
{
    std::list<H_Session *> lstTmp;
    std::list<H_Session *>::iterator itTmp;
    for (sessionit itSesion = m_mapSession.begin(); m_mapSession.end() != itSesion; ++itSesion)
    {
        if (usSockType == itSesion->second->usSockType)
        {
            lstTmp.push_back(itSesion->second);
        }
    }
    for (itTmp = lstTmp.begin(); lstTmp.end() != itTmp; ++itTmp)
    {
        delSession((*itTmp)->sock);
    }
}

H_Session *CNetBase::addTcpEv(H_SOCK &sock, const unsigned short &usSockType, const bool bAccept)
{
    int iFlag = 1;
    (void)setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag));
    H_KeepAlive(sock, H_SOCKKEEPALIVE_IDLE, H_SOCKKEEPALIVE_INTERVAL);
    struct bufferevent *pBev = bufferevent_socket_new(getBase(), sock, BEV_OPT_CLOSE_ON_FREE);
    if (NULL == pBev)
    {
        H_Printf("%s", "bufferevent_socket_new error.");
        evutil_closesocket(sock);
        return NULL;
    }

    H_Session *pSession = new(std::nothrow) H_Session;
    H_ASSERT(NULL != pSession, "malloc memory error.");
    pSession->pNetBase = this;
    pSession->sock = sock;
    pSession->uiSession = ++m_uiSession;
    pSession->usSockType = usSockType;
    pSession->uiStatus = H_INIT_NUMBER;
    pSession->pBev = pBev;

    bufferevent_setcb(pBev, tcpReadCB, NULL, tcpEventCB, pSession);
    (void)bufferevent_enable(pBev, EV_READ);
    addSession(sock, pSession, bAccept);

    return pSession;
}

void CNetBase::tcpReadCB(struct bufferevent *bev, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetBase->onRead(pSession);
}

void CNetBase::tcpEventCB(struct bufferevent *bev, short, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetBase->delSession(pSession->sock);
}

void CNetBase::orderReadCB(struct bufferevent *bev, void *arg)
{
    CNetBase *pBase = (CNetBase *)arg;
    CEvBuffer objEvBuffer;
    objEvBuffer.setEvBuf(bev);

    size_t iMsgLens = sizeof(H_Order);
    size_t iCount = objEvBuffer.getTotalLens() / iMsgLens;
    if (0 == iCount)
    {
        return;
    }

    size_t iReadLens = iCount * iMsgLens;
    char *pBinary = objEvBuffer.readBuffer(iReadLens);
    if (NULL == pBinary)
    {
        return;
    }

    H_Order *pOrder = NULL;
    for (size_t i = 0; i < iCount; ++i)
    {
        pOrder = (H_Order*)(pBinary + iMsgLens * i);
        switch (pOrder->usCmd)
        {
            case ORDER_RSTOP:
            {
                if (RSTOP_NONE == pBase->getReadyStop())
                {
                    pBase->setReadyStop(RSTOP_RUN);
                    pBase->onReadyStop();
                    pBase->setReadyStop(RSTOP_RAN);
                }                
            }
            break;
            case ORDER_STOP:
            {
                (void)event_base_loopbreak(pBase->getBase());
            }
            break;
            case ORDER_CLOSESOCK:
            {
                H_Session *pSession = pBase->getSession(pOrder->sock);
                if (NULL == pSession)
                {
                    break;
                }
                if (pOrder->uiSession != pSession->uiSession)
                {
                    break;
                }

                pBase->delSession(pSession->sock);
            }
            break;
            case ORDER_CLOSEBYTYPE:
            {
                pBase->removByType(pOrder->usSockType);
            }
            break;
            default:
                pBase->onOrder(pOrder);
                break;
        }
    }

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

void CNetBase::readyStop(void)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_RSTOP;

    sendOrder(&stOrder, sizeof(stOrder));
}

void CNetBase::closeSock(const H_SOCK sock, const unsigned int uiSession)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_CLOSESOCK;
    stOrder.sock = sock;
    stOrder.uiSession = uiSession;

    sendOrder(&stOrder, sizeof(stOrder));
}

void CNetBase::closeByType(const unsigned short usSockType)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_CLOSEBYTYPE;
    stOrder.usSockType = usSockType;

    sendOrder(&stOrder, sizeof(stOrder));
}

void CNetBase::Join(void)
{
    if (RSTOP_NONE == getReadyStop())
    {
        readyStop();
        while (RSTOP_RAN != getReadyStop())
        {
            H_Sleep(10);
        }
    }    

    H_Order stOrder;
    stOrder.usCmd = ORDER_STOP;
    sendOrder(&stOrder, sizeof(stOrder));

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
