
#include "NetListener.h"
#include "NETAddr.h"
#include "NetParser.h"
#include "NetWorkerMgr.h"
#include "Funcs.h"
#include "HEnum.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CNetListener)
CNetListener objListener;

CNetListener::CNetListener(void)
{
}

CNetListener::~CNetListener(void)
{
    std::vector<H_Listener *>::iterator itListener;
    for (itListener = m_vcListener.begin(); m_vcListener.end() != itListener; itListener++)
    {
        evconnlistener_free((*itListener)->pEvListener);
        H_SafeDelete(*itListener);
    }
    m_vcListener.clear();
}

size_t CNetListener::onOrder(CEvBuffer *pEvBuffer)
{
    size_t iCount(pEvBuffer->getTotalLens() / sizeof(H_ListenAt));
    if (H_INIT_NUMBER == iCount)
    {
        return H_INIT_NUMBER;
    }

    size_t iReadLens(iCount * sizeof(H_ListenAt));
    char *pBinary(pEvBuffer->readBuffer(iReadLens));
    if (NULL == pBinary)
    {
        return H_INIT_NUMBER;
    }
    
    H_ListenAt *pListen;
    CParser *pParser;
    H_Listener *pListener;
    CParserMgr *pParserMgr(CParserMgr::getSingletonPtr());
    CNETAddr objAddr;
    for (size_t i = 0; i < iCount; ++i)
    {
        pListen = (H_ListenAt*)(pBinary + sizeof(H_ListenAt) * i);
        pParser = pParserMgr->getParser(pListen->acParser);
        if (NULL == pParser)
        {
            H_LOG(LOGLV_ERROR, "get net parser %s error.", pListen->acParser);
            continue;
        }        

        if (H_RTN_OK != objAddr.setAddr(pListen->acHost, pListen->usPort))
        {
            H_LOG(LOGLV_ERROR, "setAddr error. host %s, port %d", pListen->acHost, pListen->usPort);
            continue;
        }

        pListener = new(std::nothrow) H_Listener;
        H_ASSERT(NULL != pListener, "malloc memory error.");
        pListener->usType = pListen->usType;
        pListener->pParser = pParser;

        pListener->pEvListener = evconnlistener_new_bind(getBase(), acceptCB, pListener,
            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC, -1, objAddr.getAddr(), (int)objAddr.getAddrSize());
        if (NULL == pListener->pEvListener)
        {
            H_LOG(LOGLV_ERROR, "listen at host %s, port %d error.", pListen->acHost, pListen->usPort);
            H_SafeDelete(pListener);
            continue;
        }

        H_LOG(LOGLV_SYS, "listen at host %s, port %d.", pListen->acHost, pListen->usPort);

        m_objLock.Lock();
        m_vcListener.push_back(pListener);
        m_objLock.unLock();
    }

    return iReadLens;
}

void CNetListener::acceptCB(struct evconnlistener *, H_SOCK sock, struct sockaddr *, int, void *arg)
{
    H_Listener *pListener((H_Listener *)arg);
    if (!CNetWorkerMgr::getSingletonPtr()->addLink(sock, pListener->pParser, pListener->usType, true))
    {
        evutil_closesocket(sock);
    }
}

void CNetListener::addListener(const char *pszParser, const unsigned short &usType,
    const char *pszHost, const unsigned short &usPort)
{
    H_ASSERT(NULL != pszParser && NULL != pszHost, "got null pointer.");

    H_ListenAt stListen;
    stListen.usType = usType;
    stListen.usPort = usPort;
    H_Zero(stListen.acHost, sizeof(stListen.acHost));
    memcpy(stListen.acHost, pszHost, strlen(pszHost));
    H_Zero(stListen.acParser, sizeof(stListen.acParser));
    memcpy(stListen.acParser, pszParser, strlen(pszParser));

    (void)sendOrder((const void*)&stListen, sizeof(stListen));
}


H_ENAMSP
