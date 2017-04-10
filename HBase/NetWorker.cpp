
#include "NetWorker.h"
#include "Funcs.h"
#include "Sender.h"
#include "NETAddr.h"
#include "Linker.h"

H_BNAMSP

SINGLETON_INIT(CNetWorker)
CNetWorker objNetWorker;

struct H_TcpLink
{
    unsigned short usSockType;
    unsigned short usPort;
    unsigned int uiID;
    H_SOCK sock;
    struct event *pMonitor;
    char acHost[H_IPLENS];
};

struct H_Listener
{
    unsigned short usType;
    struct evconnlistener *pEvListener;
    CNetWorker *pNetWorker;
};

CNetWorker::CNetWorker(void) : m_uiID(H_INIT_NUMBER), m_pIntf(NULL)
{
    m_stBinary.iLens = 4 * H_ONEK;
    m_stBinary.pBufer = (char*)malloc(m_stBinary.iLens);
    H_ASSERT(NULL != m_stBinary.pBufer, "malloc memory error.");
}

CNetWorker::~CNetWorker(void)
{
    for (listenerit itListener = m_mapListener.begin(); m_mapListener.end() != itListener; ++itListener)
    {
        evconnlistener_free(itListener->second->pEvListener);
        H_SafeDelete(itListener->second);
    }
    m_mapListener.clear();

    for (tcplinkit itTcpLink = m_mapTcpLink.begin(); m_mapTcpLink.end() != itTcpLink; ++itTcpLink)
    {
        event_free(itTcpLink->second->pMonitor);
        H_SafeDelete(itTcpLink->second);
    }
    m_mapTcpLink.clear();

    for (udpit itUdp = m_mapUdp.begin(); m_mapUdp.end() != itUdp; ++itUdp)
    {
        event_free(itUdp->second);
        evutil_closesocket(itUdp->first);
    }
    m_mapUdp.clear();

    H_SafeFree(m_stBinary.pBufer);
}

void CNetWorker::setIntf(CSVIntf *pIntf)
{
    m_pIntf = pIntf;
}

CSVIntf *CNetWorker::getIntf(void)
{
    return m_pIntf;
}

void CNetWorker::monitorCB(H_SOCK, short, void *arg)
{
    H_TcpLink *pTcpLink = (H_TcpLink *)arg;
    if (H_INVALID_SOCK == pTcpLink->sock)
    {
        CLinker::getSingletonPtr()->addLink(pTcpLink->uiID, pTcpLink->acHost, pTcpLink->usPort);
    }
}

struct event *CNetWorker::monitorLink(struct H_TcpLink *pTcpLink)
{
    timeval tVal;
    size_t uiMS = 5000;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = (long)uiMS / 1000;
        tVal.tv_usec = ((long)uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = ((long)uiMS * 1000);
    }

    struct event *pEvent = event_new(getBase(),
        -1, EV_PERSIST, monitorCB, pTcpLink);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CNetWorker::acceptCB(struct evconnlistener *, H_SOCK sock, struct sockaddr *,
    int, void *arg)
{
    H_Listener *pListener = (H_Listener *)arg;
    if (RSTOP_NONE != pListener->pNetWorker->getReadyStop())
    {
        evutil_closesocket(sock);
        return;
    }
    
    (void)pListener->pNetWorker->addTcpEv(sock, pListener->usType, true);
}

void CNetWorker::udpCB(H_SOCK sock, short sEv, void *arg)
{    
    int iLens = H_GetSockDataLens(sock);
    if (H_INIT_NUMBER >= iLens)
    {
        return;
    }

    CNetWorker *pWorker = (CNetWorker *)arg;
    luabridge::H_LBinary *pLBinary = pWorker->getLBinary();
    if (iLens > (int)pLBinary->iLens)
    {
        char *pTmp = (char*)malloc(iLens);
        if (NULL == pTmp)
        {
            H_Printf("%s", "malloc memory error.");
            return;
        }

        H_SafeFree(pLBinary->pBufer);
        pLBinary->pBufer = pTmp;
        pLBinary->iLens = iLens;
    }

    sockaddr stAddr;
    socklen_t iAddrLens = sizeof(stAddr);
    int iRecvLens = recvfrom(sock, pLBinary->pBufer, (int)pLBinary->iLens, 0, &stAddr, &iAddrLens);
    if (iRecvLens <= 0)
    {
        return;
    }

    CNETAddr objAddr;
    objAddr.setAddr(&stAddr);
    pWorker->getIntf()->onUdpRead(sock, objAddr.getIp().c_str(), objAddr.getPort(), 
        pLBinary->pBufer, iRecvLens);
}

void CNetWorker::onOrder(H_Order *pOrder)
{
    switch (pOrder->usCmd)
    {
        case ORDER_ADDLISTENER:
        {
            CNETAddr objAddr;
            if (H_RTN_OK != objAddr.setAddr(pOrder->acHost, pOrder->usPort))
            {
                H_Printf("%s", "addListener setAddr error.");
                break;
            }

            H_Listener *pListener = new(std::nothrow) H_Listener;
            H_ASSERT(NULL != pListener, "malloc memory error.");

            pListener->pNetWorker = this;
            pListener->usType = pOrder->usSockType;
            pListener->pEvListener = evconnlistener_new_bind(getBase(), acceptCB, pListener,
                LEV_OPT_CLOSE_ON_FREE, -1, objAddr.getAddr(), (int)objAddr.getAddrSize());
            if (NULL == pListener->pEvListener)
            {
                H_Printf("listen at host %s port %d error.", pOrder->acHost, pOrder->usPort);
                H_SafeDelete(pListener);
                break;
            }

            H_Printf("listen at host %s port %d.", pOrder->acHost, pOrder->usPort);            
            m_mapListener[pOrder->uiSession] = pListener;
        }
        break;

        case ORDER_DELLISTENER:
        {
            listenerit itListener = m_mapListener.find(pOrder->uiSession);
            if (m_mapListener.end() != itListener)
            {
                evconnlistener_free(itListener->second->pEvListener);
                H_SafeDelete(itListener->second);

                m_mapListener.erase(itListener);
            }
        }
        break;

        case ORDER_ADDLINK:
        {
            size_t iLens(strlen(pOrder->acHost));
            H_TcpLink *pTcpLink = new(std::nothrow) H_TcpLink;
            H_ASSERT(NULL != pTcpLink, "malloc memory error.");
            memcpy(pTcpLink->acHost, pOrder->acHost, iLens);
            pTcpLink->acHost[iLens] = '\0';
            pTcpLink->sock = H_INVALID_SOCK;
            pTcpLink->uiID = pOrder->uiSession;
            pTcpLink->usPort = pOrder->usPort;
            pTcpLink->usSockType = pOrder->usSockType;

            pTcpLink->pMonitor = monitorLink(pTcpLink);
            m_mapTcpLink[pTcpLink->uiID] = pTcpLink;
            CLinker::getSingletonPtr()->addLink(pTcpLink->uiID, pTcpLink->acHost, pTcpLink->usPort);
        }
        break;

        case ORDER_ADDLINKEV:
        {
            tcplinkit itTcpLink = m_mapTcpLink.find(pOrder->uiSession);
            if (m_mapTcpLink.end() != itTcpLink)
            {
                itTcpLink->second->sock = pOrder->sock;
                H_Session *pSession = addTcpEv(itTcpLink->second->sock, itTcpLink->second->usSockType, false);
                pSession->bLinker = true;
            }
        }
        break;

        case ORDER_DELLINK:
        {
            tcplinkit itTcpLink = m_mapTcpLink.find(pOrder->uiSession);
            if (m_mapTcpLink.end() != itTcpLink)
            {
                if (H_INVALID_SOCK != itTcpLink->second->sock)
                {
                    delSession(itTcpLink->second->sock);
                }

                event_free(itTcpLink->second->pMonitor);
                H_SafeDelete(itTcpLink->second);

                m_mapTcpLink.erase(itTcpLink);
            }
        }
        break;

        case ORDER_ADDUDP:
        {
            struct event *pBev = event_new(getBase(),
                pOrder->sock, EV_READ | EV_PERSIST, udpCB, this);
            if (NULL == pBev)
            {
                break;
            }

            (void)event_add(pBev, NULL);
            m_mapUdp[pOrder->sock] = pBev;
        }
        break;

        case ORDER_DELUDP:
        {
            udpit itUdp = m_mapUdp.find(pOrder->sock);
            if (m_mapUdp.end() != itUdp)
            {
                event_free(itUdp->second);
                evutil_closesocket(itUdp->first);
                m_mapUdp.erase(itUdp);
            }
        }
        break;

        default:
            break;
    }
}

void CNetWorker::onStart(void)
{
    m_pIntf->onStart();
}

void CNetWorker::onReadyStop(void)
{
    m_pIntf->onStop();
}

void CNetWorker::onClose(H_Session *pSession)
{
    CSender::getSingletonPtr()->delSock(pSession->sock);
    if (pSession->bLinker)
    {
        for (tcplinkit itLink = m_mapTcpLink.begin(); m_mapTcpLink.end() != itLink; ++itLink)
        {
            if (itLink->second->sock == pSession->sock)
            {
                itLink->second->sock = H_INVALID_SOCK;
                break;
            }            
        }
    }
    m_pIntf->onTcpClose(pSession);
}

void CNetWorker::onLinked(H_Session *pSession, const bool &bAccept)
{
    CSender::getSingletonPtr()->addSock(pSession->sock, pSession->uiSession);
    if (bAccept) 
    {
        m_pIntf->onTcpAccept(pSession);
    }
    else
    {
        m_pIntf->onTcpLinked(pSession);
    }    
}

void CNetWorker::onRead(H_Session *pSession)
{
    m_pIntf->onTcpRead(pSession);
}

unsigned int CNetWorker::addListener(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDLISTENER;
    stOrder.usSockType = usSockType;
    stOrder.usPort = usPort;
    size_t iLens = strlen(pszHost);
    memcpy(stOrder.acHost, pszHost, iLens);
    stOrder.acHost[iLens] = '\0';
    stOrder.uiSession = H_AtomicAdd(&m_uiID, 1);

    sendOrder(&stOrder, sizeof(stOrder));    

    return stOrder.uiSession;
}

void CNetWorker::delListener(const unsigned int uiID)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_DELLISTENER;
    stOrder.uiSession = uiID;

    sendOrder(&stOrder, sizeof(stOrder));
}

unsigned int CNetWorker::addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDLINK;
    stOrder.usSockType = usSockType;
    stOrder.usPort = usPort;
    size_t iLens = strlen(pszHost);
    memcpy(stOrder.acHost, pszHost, iLens);
    stOrder.acHost[iLens] = '\0';
    stOrder.uiSession = H_AtomicAdd(&m_uiID, 1);

    sendOrder(&stOrder, sizeof(stOrder));

    return stOrder.uiSession;
}

void CNetWorker::delTcpLink(const unsigned int uiID)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_DELLINK;
    stOrder.uiSession = uiID;

    sendOrder(&stOrder, sizeof(stOrder));
}

H_SOCK  CNetWorker::addUdp(const char *pszHost, const unsigned short usPort)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        return H_INVALID_SOCK;
    }

    H_SOCK sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (H_INVALID_SOCK == sock)
    {
        return H_INVALID_SOCK;
    }

    int iOpt = 1;
    (void)setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&iOpt, sizeof(iOpt));
    (void)evutil_make_socket_nonblocking(sock);
    if (-1 == bind(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        evutil_closesocket(sock);
        return H_INVALID_SOCK;
    }

    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDUDP;
    stOrder.sock = sock;

    sendOrder(&stOrder, sizeof(stOrder));

    return sock;
}

void CNetWorker::delUdp(H_SOCK sock)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_DELUDP;
    stOrder.sock = sock;

    sendOrder(&stOrder, sizeof(stOrder));
}

void CNetWorker::addLinkEv(const H_SOCK &sock, const unsigned int &uiID)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDLINKEV;
    stOrder.sock = sock;
    stOrder.uiSession = uiID;

    sendOrder(&stOrder, sizeof(stOrder));
}

H_ENAMSP
