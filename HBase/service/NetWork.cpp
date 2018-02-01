
#include "NetWork.h"
#include "Log.h"
#include "NetMgr.H"
#include "RPCLink.h"
#include "Parser_RPC.h"
#include "Adjure_NetWorker.h"
#include "event2/util.h"
#include "event2/event_compat.h"
#include "event2/buffer.h"
#include "event2/event.h"

H_BNAMSP

CNetWorker::CNetWorker(void) : CEventService(H_QULENS_ADDSOCK), m_iNoDelay(1), m_pMsgTrigger(NULL), m_pWorkerMgr(NULL), m_pNetMgr(NULL)
{
    setSVName(H_SERVICE_NETWORKER);
}
CNetWorker::~CNetWorker(void)
{
}

void CNetWorker::setMsgTrigger(CMsgTrigger *pMsgTrigger)
{
    m_pMsgTrigger = pMsgTrigger;
}
CMsgTrigger *CNetWorker::getMsgTrigger(void)
{
    return m_pMsgTrigger;
}
void CNetWorker::setWorkerMgr(CWorkerMgr *pWorkerMgr)
{
    m_pWorkerMgr = pWorkerMgr;
}
CWorkerMgr *CNetWorker::getWorkerMgr(void)
{
    return m_pWorkerMgr;
}
void CNetWorker::setNetMgr(class CNetMgr *pNetMgr)
{
    m_pNetMgr = pNetMgr;
}
class CNetMgr *CNetWorker::getNetMgr(void)
{
    return m_pNetMgr;
}
void CNetWorker::setLinkStatus(class CLinkInfo *pLinkInfo, const LinkState emState)
{
    if (NULL != pLinkInfo)
    {
        pLinkInfo->setLinkState(emState);
    }
}

void CNetWorker::addSock(class CLinkInfo *pLinkInfo, class CParser *pParser,
    const H_SOCK &sock, const unsigned short &usType)
{
    CAddSockInAdjure *pAddInAdjure = new(std::nothrow) CAddSockInAdjure(pLinkInfo, pParser, sock, usType);
    if (NULL == pAddInAdjure)
    {
        CUtils::closeSock(sock);
        setLinkStatus(pLinkInfo, LS_CLOSED);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }
    if (!Adjure(pAddInAdjure))
    {
        CUtils::closeSock(sock);
        setLinkStatus(pLinkInfo, LS_CLOSED);
        H_SafeDelete(pAddInAdjure);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}
void CNetWorker::bindWorker(const H_SOCK &uiSock, class CWorker *pWorker)
{
    CBindToTaskAdjure *pAdjure = new(std::nothrow) CBindToTaskAdjure(pWorker, uiSock);
    if (NULL == pAdjure)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pAdjure))
    {
        H_SafeDelete(pAdjure);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}
void CNetWorker::unBindWorker(const H_SOCK &uiSock)
{
    CToSockAdjure *pAdjure = new(std::nothrow) CToSockAdjure(NETWORKERADJ_UNBINDWORKER, uiSock);
    if (NULL == pAdjure)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pAdjure))
    {
        H_SafeDelete(pAdjure);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}

void CNetWorker::onAdjure(CAdjure *pAdjure)
{
    switch (pAdjure->getAdjure())
    {
        case NETWORKERADJ_ADDSOCK:
        {
            addSock((CAddSockInAdjure *)pAdjure);
        }
        break;
        case NETWORKERADJ_BINDWORKER:
        {
            bindWorker((CBindToTaskAdjure *)pAdjure);
        }
        break;
        case NETWORKERADJ_UNBINDWORKER:
        {
            unBindWorker((CToSockAdjure *)pAdjure);
        }
        break;
        default:
        break;
    }
}
void CNetWorker::afterAdjure(CAdjure *pAdjure)
{
    H_SafeDelete(pAdjure);
}

static void onSocketClose(CSession *pSession, evutil_socket_t &sock)
{
    CLinkInfo *pLinkInfo(pSession->getLinkInfo());
    CNetWorker *pNetWorker(pSession->getNetWorker());
    CWorker *pBindWorker(pNetWorker->getBindWorker(sock));
    uint64_t uiBindId(NULL != pLinkInfo ? pLinkInfo->getBindId() : H_INIT_NUMBER);

    pNetWorker->setLinkStatus(pLinkInfo, LS_CLOSED);
    pNetWorker->removeBind(sock);
    pNetWorker->getMsgTrigger()->triggerClosed(pBindWorker, sock, pSession->getType(), uiBindId);

    CUtils::closeSock(sock);
    struct event *pEv((struct event *)pSession->getEvent());
    event_free(pEv);
    pEv = (struct event *)pSession->getTimeEvent();
    if (NULL != pEv)
    {
        event_free(pEv);
    }

    if (pSession->getParser()->getRPC()
        && H_OK_STATUS == *pSession->getHSStatus())
    {
        SVIdType *pExtendData((SVIdType *)pSession->getExtendData());
        pSession->getNetWorker()->getNetMgr()->
            getRPCLink()->Unregister(pExtendData->iId, pExtendData->iType, sock);
        H_LOG(LOGLV_SYS, "unregister rpc link, id %d, type %d, sock %d", 
            pExtendData->iId, pExtendData->iType, sock);
    }

    H_SafeDelete(pSession);
}
static void parseProtocol(CSession *pSession, H_SOCK &sock)
{
    bool bClose(false);
    size_t iParsed(H_INIT_NUMBER);
    short *psHSStatus(pSession->getHSStatus());
    CParser *pParser(pSession->getParser());
    CDynaBuffer *pSockBuf(pSession->getDynaBuffer());
    CNetWorker *pNetWorker(pSession->getNetWorker());

    //需要握手
    if (pParser->needHandShake()
        && H_OK_STATUS != *(psHSStatus))
    {
        CBuffer *pBuf(pParser->handShake(pSession, pSockBuf->getBuffer(), pSockBuf->getLens(), iParsed, bClose));
        if (bClose)
        {
            onSocketClose(pSession, sock);
            H_LOG(LOGLV_WARN, "socket %d handshake error.", sock);
            return;
        }

        if (H_OK_STATUS == *(psHSStatus))
        {
            //握手完成，移除计时器
            event_free((struct event *)pSession->getTimeEvent());
            pSession->setTimeEvent(NULL);
            if (NULL == pBuf)
            {
                pSockBuf->Remove(iParsed);
                return;
            }
        }

        if (NULL == pBuf)
        {
            return;
        }

        pNetWorker->getNetMgr()->sendMsg(sock, pBuf);
        pSockBuf->Remove(iParsed);
        return;
    }

    CAdjure *pAdjure(NULL);
    size_t iTotalParsed(H_INIT_NUMBER);
    CWorker *pBindWorker(pNetWorker->getBindWorker(sock));
    
    while (iTotalParsed < pSockBuf->getLens())
    {
        iParsed = H_INIT_NUMBER;
        pAdjure = pParser->Parse(pSession, pSockBuf->getBuffer() + iTotalParsed, pSockBuf->getLens() - iTotalParsed, iParsed, bClose);
        if (bClose)
        {
            onSocketClose(pSession, sock);
            H_SafeDelete(pAdjure);
            return;
        }
        if (NULL == pAdjure)
        {
            break;
        }
        
        iTotalParsed += iParsed; 

        pNetWorker->handleAdjure(pBindWorker, pAdjure, sock);
    }

    pSockBuf->Remove(iTotalParsed);
}
static void socketEventCB(evutil_socket_t sock, short sEvent, void *pArg)
{
    CSession *pSession((CSession *)pArg);
    READ_RETURN emRet(CEventService::sockRead(sock, pSession->getDynaBuffer()));
    switch (emRet)
    {
        case READ_DATA_RECEIVED://处理数据
        {
            parseProtocol(pSession, sock);
        }
        break;
        case READ_NO_DATA_RECEIVED://无数据
            break;
        case READ_ERROR:
        case READ_MEMORY_ERROR:
        {
            onSocketClose(pSession, sock);
        }
        break;
        default:
            break;
    }
}
static void handShakeTimeOut(evutil_socket_t, short, void *pArg)
{
    CSession *pSession((CSession *)pArg);
    if (H_OK_STATUS != *(pSession->getHSStatus()))
    {
        H_SOCK sock(pSession->getSock());
        onSocketClose(pSession, sock);

        H_LOG(LOGLV_WARN, "socket %d handshake timeout.", sock);
    }
}
void *CNetWorker::handShakeMonitor(class CSession *pSession, const unsigned int &uiMs)
{
    struct event *pTimeEvent = evtimer_new((struct event_base *)getLoop(), handShakeTimeOut, pSession);
    if (NULL == pTimeEvent)
    {
        H_LOG(LOGLV_ERROR, "%s", "evtimer_new error.");
        return NULL;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    if (uiMs >= 1000)
    {
        tv.tv_sec = uiMs / 1000;
        tv.tv_usec = (uiMs % 1000) * 1000;
    }
    else
    {
        tv.tv_usec = uiMs * 1000;
    }
    if (H_RTN_OK != evtimer_add(pTimeEvent, &tv))
    {
        event_free(pTimeEvent);
        H_LOG(LOGLV_ERROR, "%s", "evtimer_add error.");
        return NULL;
    }

    pSession->setTimeEvent(pTimeEvent);
    return pTimeEvent;
}
CSession *CNetWorker::newSession(class CParser *pParser, class CLinkInfo *pLinkInfo, 
    H_SOCK &sock, const unsigned short &usType)
{
    CSession *pSession = new(std::nothrow) CSession(this, pParser, pLinkInfo, sock, usType);
    if (NULL == pSession)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    if (!pSession->getDynaBuffer()->New(H_SOCKBUFSIZE))
    {
        H_SafeDelete(pSession);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    return pSession;
}
void *CNetWorker::newSockEvent(class CSession *pSession, H_SOCK &sock)
{
    (void)evutil_make_socket_nonblocking(sock);
    (void)setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&m_iNoDelay, sizeof(m_iNoDelay));
    struct event *pEvent(event_new((struct event_base *)getLoop(), sock, EV_READ | EV_PERSIST, socketEventCB, pSession));
    if (NULL == pEvent)
    {
        H_LOG(LOGLV_ERROR, "%s", "event_new error.");
        return NULL;
    }

    if (H_RTN_OK != event_add(pEvent, NULL))
    {
        event_free(pEvent);
        H_LOG(LOGLV_ERROR, "%s", "event_add error.");
        return NULL;
    }

    pSession->setEvent(pEvent);
    return pEvent;
}
void CNetWorker::addSock(CAddSockInAdjure *pAdjure)
{
    H_SOCK sock(pAdjure->getSock());
    CLinkInfo *pLinkInfo(pAdjure->getLinkInfo());
    CParser *pParser(pAdjure->getParser());
    CWorker *pBindWorker(NULL);
    uint64_t ulBindId(H_INIT_NUMBER);
    if (NULL != pLinkInfo)
    {
        pBindWorker = pLinkInfo->getBindWorker();
        ulBindId = pLinkInfo->getBindId();
    }

    CSession *pSession(newSession(pParser, pLinkInfo, sock, pAdjure->getType()));
    if (NULL == pSession)
    {
        CUtils::closeSock(sock);
        setLinkStatus(pLinkInfo, LS_CLOSED);
        return;
    }
    
    struct event *pEvent((struct event *)newSockEvent(pSession, sock));
    if (NULL == pEvent)
    {
        CUtils::closeSock(sock);
        setLinkStatus(pLinkInfo, LS_CLOSED);
        H_SafeDelete(pSession);
        return;
    }

    struct event *pTimeEvent(NULL);
    if (pParser->needHandShake())
    {
        pTimeEvent = (struct event *)handShakeMonitor(pSession, pParser->getHSTimeOut());
        if (NULL == pTimeEvent)
        {
            CUtils::closeSock(sock);
            setLinkStatus(pLinkInfo, LS_CLOSED);
            H_SafeDelete(pSession);
            event_free(pEvent);
            return;
        }
    }
    
    if (NULL != pBindWorker)
    {
        m_mapBind[sock] = pBindWorker;
    }

    if (pSession->Accept())
    {
        m_pMsgTrigger->triggerAccept(pBindWorker, sock, pSession->getType(), H_INIT_NUMBER);
        return;
    }

    if (pParser->getRPC())
    {
        //rpc发起握手
        bool bClose(false);
        CRPCParser *pRPCParser((CRPCParser *)pParser);
        CBuffer *pBuffer(pRPCParser->Sign(bClose));
        if (bClose)
        {
            onSocketClose(pSession, sock);
            return;
        }

        m_pNetMgr->sendMsg(sock, pBuffer);
    }
    m_pMsgTrigger->triggerConnect(pBindWorker, sock, pSession->getType(), ulBindId);
}
void CNetWorker::bindWorker(CBindToTaskAdjure *pAdjure)
{
    m_mapBind[pAdjure->getSock()] = pAdjure->getWorker();
}
void CNetWorker::unBindWorker(CToSockAdjure *pAdjure)
{
    removeBind(pAdjure->getSock());
}
void CNetWorker::removeBind(const H_SOCK &uiSock)
{
    bindit it(m_mapBind.find(uiSock));
    if (m_mapBind.end() != it)
    {
        m_mapBind.erase(it);
    }
}
class CWorker *CNetWorker::getBindWorker(const H_SOCK &uiSock)
{
    bindit it(m_mapBind.find(uiSock));
    if (m_mapBind.end() != it)
    {
        return it->second;
    }

    return NULL;
}

void CNetWorker::handleAdjure(class CWorker *pBindWorker, CAdjure *pAdjure, const H_SOCK &uiSock)
{
    switch (pAdjure->getAdjure())
    {
        case MSG_NET_READ_CONTINUE://websocket分帧
        {}
        break;
        case MSG_NET_READ_RTNBUF://websocket ping返回
        {
            CRtnBufAdjure *pRtnBuf((CRtnBufAdjure *)pAdjure);
            m_pNetMgr->sendMsg(uiSock, pRtnBuf->getPack());
            H_SafeDelete(pAdjure);
        }
        break;
        default:
        {
            if (NULL != pBindWorker)
            {
                pAdjure->setNorProc(false);
                m_pWorkerMgr->addAdjureToTask(pBindWorker, pAdjure);
                break;
            }

            dispAdjure(pAdjure);
        }
        break;
    }
}
void CNetWorker::dispAdjure(CAdjure *pAdjure)
{
    switch (pAdjure->getAdjure())
    {
        case MSG_NET_READ_I:
        {
            CINetReadAdjure *pINetRead((CINetReadAdjure *)pAdjure);
            m_pMsgTrigger->triggerIProto(pINetRead);
        }
        break;
        case MSG_NET_READ_HTTPD:
        {
            CTaskHttpdAdjure *pHttpdRead((CTaskHttpdAdjure *)pAdjure);
            m_pMsgTrigger->triggerHttpdProto(pHttpdRead);
        }
        break;
        case MSG_TASK_DEBUG:
        {
            CDebugAdjure *pDebug((CDebugAdjure *)pAdjure);
            m_pMsgTrigger->triggerDebug(pDebug);
            H_SafeDelete(pDebug);
        }
        break;
        case MSG_NET_RPC:
        {
            CNetRPCAdjure *pNetRPC((CNetRPCAdjure *)pAdjure);
            m_pMsgTrigger->triggerNetRPC(pNetRPC);
        }
        break;
        default:
            break;
    }
}

H_ENAMSP
