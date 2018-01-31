
#include "NetListener.h"
#include "Parser.h"
#include "Log.h"
#include "NETAddr.h"
#include "NetMgr.H"
#include "Adjure_NetWorker.h"
#include "event2/listener.h"

H_BNAMSP

CNetListener::CNetListener(class CParserMgr *pMgr, class CNetMgr *pNetMgr) : CEventService(H_QULENS_LISTENER),
    m_pParserMgr(pMgr), m_pNetMgr(pNetMgr)
{
    setSVName(H_SERVICE_LISTENER);
}

CNetListener::~CNetListener(void)
{
    struct evconnlistener *pListener;
    for (std::vector<void *>::iterator it = m_vcListener.begin(); m_vcListener.end() != it; ++it)
    {
        pListener = (struct evconnlistener *)(*it);
        evconnlistener_free(pListener);
    }

    for (std::vector<class CNetInfo *>::iterator it = m_vcNetInfo.begin(); m_vcNetInfo.end() != it; ++it)
    {
        H_SafeDelete((*it));
    }
}

void CNetListener::addListener(const char *pszParser, const unsigned short &usType,
    const char *pszHost, const unsigned short &usPort)
{
    CParser *pParser(m_pParserMgr->getParser(pszParser));
    H_ASSERT(NULL != pParser, "get parser error.");

    CNetInfo *pNetInfo = new(std::nothrow) CNetInfo(this, pParser, usType, pszHost, usPort);
    if (NULL == pNetInfo)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    CAddListenAdjure *pAdjure = new(std::nothrow) CAddListenAdjure(pNetInfo);
    if (NULL == pAdjure)
    {
        H_SafeDelete(pNetInfo);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pAdjure))
    {
        H_SafeDelete(pNetInfo);
        H_SafeDelete(pAdjure);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}

static void acceptCB(struct evconnlistener *, H_SOCK sock, struct sockaddr *, int, void *pArg)
{
    CNetInfo *pNetInfo((CNetInfo *)pArg);
    CNetListener *pNetListener((CNetListener *)pNetInfo->getEventService());
    CNetWorker *pNetWorker(pNetListener->getNetMgr()->getNetWorker(sock));

    pNetWorker->addSock(NULL, pNetInfo->getParser(), sock, pNetInfo->getType());
}
void CNetListener::onAdjure(CAdjure *pAdjure)
{
    CNETAddr objAddr;
    CNetInfo *pNetInfo(((CAddListenAdjure *)pAdjure)->getListenInfo());    
    if (H_RTN_OK != objAddr.setAddr(pNetInfo->getHost(), pNetInfo->getPort(), pNetInfo->isIpV6()))
    {
        H_SafeDelete(pNetInfo);
        H_LOG(LOGLV_ERROR, "parse host %s port %d error.", pNetInfo->getHost(), pNetInfo->getPort());
        return;
    }

    struct evconnlistener *pListener(evconnlistener_new_bind((struct event_base *)getLoop(), acceptCB, pNetInfo,
        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC, -1, objAddr.getAddr(), (int)objAddr.getAddrSize()));
    if (NULL == pListener)
    {
        H_SafeDelete(pNetInfo);
        H_LOG(LOGLV_ERROR, "%s", "evconnlistener_new_bind error.");
        return;
    }    

    H_LOG(LOGLV_SYS, "listen at host %s port %d", pNetInfo->getHost(), pNetInfo->getPort());
    m_vcListener.push_back(pListener);
    m_vcNetInfo.push_back(pNetInfo);
}

void CNetListener::afterAdjure(CAdjure *pAdjure)
{
    H_SafeDelete(pAdjure);
}

H_ENAMSP
