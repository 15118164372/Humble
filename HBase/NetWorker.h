
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "HStruct.h"
#include "HEnum.h"
#include "NetBase.h"
#include "RWLock.h"

H_BNAMSP

class CNetWorker : public CNetBase
{
private:
    struct H_WorkerCMD
    {
        bool bAccept;       
        class CParser *pParser;
        H_LINK stLink;
    };

public:
    CNetWorker(void);
    ~CNetWorker(void);

    size_t onOrder(CEvBuffer *pEvBuffer);
    H_INLINE bool addLink(H_SOCK &sock, class CParser *pParser, const unsigned short &usType, const bool &bAccept)
    {
        H_WorkerCMD stCMD;
        stCMD.bAccept = bAccept;
        stCMD.stLink.usType = usType;
        stCMD.pParser = pParser;
        stCMD.stLink.sock = sock;

        return sendOrder((const void*)&stCMD, sizeof(stCMD));
    };

    void onLinked(H_Session *pSession);
    void onAccept(H_Session *pSession);
    void onRead(H_Session *pSession);
    void onClose(H_Session *pSession);

    static void tcpReadCB(struct bufferevent *bev, void *arg);
    static void tcpEventCB(struct bufferevent *bev, short, void *arg);

private:
    H_DISALLOWCOPY(CNetWorker);
    H_INLINE void addEvent(H_WorkerCMD *pCMD) 
    {
        H_Session *pSession = new(std::nothrow) H_Session;
        H_ASSERT(NULL != pSession, "malloc memory error.");

        //(void)setsockopt(pCMD->stLink.sock, IPPROTO_TCP, TCP_NODELAY, (char *)&m_iSockFlag, sizeof(m_iSockFlag));
        //(void)evutil_make_socket_nonblocking(pCMD->stLink.sock);

        pSession->pEv = bufferevent_socket_new(getBase(), pCMD->stLink.sock, BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pSession->pEv)
        {
            evutil_closesocket(pCMD->stLink.sock);
            H_SafeDelete(pSession);

            return;
        }

        pSession->pNetWorker = this;
        pSession->pParser = pCMD->pParser;
        pSession->stLink.usType = pCMD->stLink.usType;
        pSession->stLink.sock = pCMD->stLink.sock;
        pSession->bReLink = !(pCMD->bAccept);

        bufferevent_setcb(pSession->pEv, tcpReadCB, NULL, tcpEventCB, pSession);
        (void)bufferevent_enable(pSession->pEv, EV_READ);

        if (pCMD->bAccept)
        {
            onAccept(pSession);
        }
        else
        {
            onLinked(pSession);
        }
    };
    void closeSock(H_Session *pSession);

    void dispProto(H_Session *pSession, H_TCPBUF &stTcpBuf, H_Binary &stBinary);

    void dispHttp(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void dispNomal(H_PROTOTYPE &iProto, H_TCPBUF &stTcpBuf, H_Binary &stBinary);

    void dispRPCCall(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void dispRPCRtn(H_TCPBUF &stTcpBuf, H_Binary &stInBinary);

    void dispCMD(H_TCPBUF &stTcpBuf, H_Binary &stBinary);
    void sendCMD(const char *pszTaskName, H_LINK *pLink, H_CMD *pCmd, H_TCPBUF &stTcpBuf);

//private:
    //int m_iSockFlag;
};

H_ENAMSP

#endif//H_NETWORKER_H_
