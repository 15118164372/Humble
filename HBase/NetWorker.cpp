
#include "NetWorker.h"
#include "NetParser.h"
#include "Linker.h"
#include "MSGDispatch.h"
#include "Chan.h"
#include "Funcs.h"
#include "TaskMgr.h"
#include "Sender.h"
#include "Log.h"

H_BNAMSP

CNetWorker::CNetWorker(void) : m_iSockFlag(1)
{

}

CNetWorker::~CNetWorker(void)
{
    
}

void CNetWorker::tcpReadCB(struct bufferevent *bev, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetWorker->onRead(pSession);
}

void CNetWorker::tcpEventCB(struct bufferevent *bev, short, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetWorker->onClose(pSession);
}

size_t CNetWorker::onOrder(CEvBuffer *pEvBuffer)
{    
    size_t iCount = pEvBuffer->getTotalLens() / sizeof(H_WorkerCMD);
    if (H_INIT_NUMBER == iCount)
    {
        return H_INIT_NUMBER;
    }

    size_t iReadLens = iCount * sizeof(H_WorkerCMD);
    char *pBinary = pEvBuffer->readBuffer(iReadLens);
    if (NULL == pBinary)
    {
        return H_INIT_NUMBER;
    }

    H_WorkerCMD *pCMD;
    for (size_t i = 0; i < iCount; ++i)
    {
        pCMD = (H_WorkerCMD*)(pBinary + sizeof(H_WorkerCMD) * i);
        switch (pCMD->cCmd)
        {
            case NET_CMD_ADDLINK:
            {
                addEvent(pCMD);
            }
            break;
            case NET_CMD_DELLINK:
            {
            }
            break;
            default:
                break;
        }
    }

    return iReadLens;
}

void CNetWorker::onAccept(H_Session *pSession)
{   
    if (SOCKTYPE_HTTP == pSession->stLink.usType)
    {
        return;
    }

    CMSGDispatch::getSingletonPtr()->sendEvent(MSG_NET_ACCEPT, (void*)&pSession->stLink, sizeof(pSession->stLink));
}

void CNetWorker::onLinked(H_Session *pSession)
{
    if (SOCKTYPE_HTTP == pSession->stLink.usType)
    {
        return;
    }

    CMSGDispatch::getSingletonPtr()->sendEvent(MSG_NET_LINKED, (void*)&pSession->stLink, sizeof(pSession->stLink));
}

void CNetWorker::onClose(H_Session *pSession)
{
    if (SOCKTYPE_HTTP == pSession->stLink.usType)
    {
        bufferevent_free(pSession->pEv);
        H_SafeDelete(pSession);
        return;
    }

    CMSGDispatch::getSingletonPtr()->sendEvent(MSG_NET_CLOSE, (void*)&pSession->stLink, sizeof(pSession->stLink));

    if (pSession->bReLink)
    {
        CLinker::getSingletonPtr()->linkClosed(pSession->stLink.sock);
    }

    bufferevent_free(pSession->pEv);
    H_SafeDelete(pSession);
}

void CNetWorker::onRead(H_Session *pSession)
{
    CEvBuffer objEvBuf;
    objEvBuf.setEvBuf(pSession->pEv);
    size_t iBufLens(objEvBuf.getTotalLens());
    char *pBuf(objEvBuf.readBuffer(iBufLens));
    if (NULL == pBuf)
    {
        return;
    }

    H_TCPBUF stTcpBuf;
    H_Binary stBinary;
    size_t iParsed(H_INIT_NUMBER);
    size_t iCurParsed(H_INIT_NUMBER);
    size_t iSurplus(H_INIT_NUMBER);
    
    stTcpBuf.stLink.sock = pSession->stLink.sock;
    stTcpBuf.stLink.usType = pSession->stLink.usType;

    while (true)
    {
        if (iParsed >= iBufLens)
        {
            break;
        }

        iCurParsed = H_INIT_NUMBER;
        iSurplus = iBufLens - iParsed;
        stBinary = pSession->pParser->parsePack(pBuf + iParsed, iSurplus, iCurParsed);
        if (H_INIT_NUMBER == iCurParsed)
        {
            if (iSurplus > H_MAXPACK_LENS)
            {
                H_LOG(LOGLV_ERROR, "%s", "pack too large.");
                onClose(pSession);
                return;
            }
            break;
        }        
        if (stBinary.iLens < sizeof(H_PROTOTYPE)
            || stBinary.iLens > H_MAXPACK_LENS)
        {
            H_LOG(LOGLV_ERROR, "%s", "pack length error.");
            onClose(pSession);
            return;
        }
        iParsed += iCurParsed;

        dispProto(pSession, stTcpBuf, stBinary);
    }

    objEvBuf.delBuffer(iParsed);
}

void CNetWorker::dispProto(H_Session *pSession, H_TCPBUF &stTcpBuf, H_Binary &stBinary)
{
    switch (pSession->stLink.usType)
    {
        case SOCKTYPE_HTTP:
        {
            dispHttp(stTcpBuf, stBinary);
        }
        break;

        case SOCKTYPE_CMD:
        {
            H_PROTOTYPE iProto(H_NTOH(*((H_PROTOTYPE*)stBinary.pBufer)));
            if (PROTO_CMD == iProto)
            {
                dispCMD(stTcpBuf, stBinary);
            }
        }
        break;

        case SOCKTYPE_RPC:
        {
            H_PROTOTYPE iProto(H_NTOH(*((H_PROTOTYPE*)stBinary.pBufer)));
            switch (iProto)
            {
                case PROTO_RPCCAL:
                {
                    dispRPCCall(stTcpBuf, stBinary);
                }
                break;
                case PROTO_RPCRTN:
                {
                    dispRPCRtn(stTcpBuf, stBinary);
                }
                break;
                default:
                    break;
            }
        }
        break;

        default:
        {
            H_PROTOTYPE iProto(H_NTOH(*((H_PROTOTYPE*)stBinary.pBufer)));
            dispNomal(iProto, stTcpBuf, stBinary);
        }
        break;
    }
}

void CNetWorker::dispHttp(H_TCPBUF &stTcpBuf, H_Binary &stBinary)
{
    //GET / HTTP/1.1
    char *pPos = strstr(stBinary.pBufer, " HTTP/");
    if (NULL == pPos)
    {
        return;
    }
    std::string strReqHeader(stBinary.pBufer, pPos - stBinary.pBufer);
    std::string strUrl(H_GetLastOfFlag(strReqHeader, " "));

    CChan *pChan(CMSGDispatch::getSingletonPtr()->getStrProto(strUrl.c_str()));
    if (NULL == pChan)
    {
        return;
    }

    stTcpBuf.iProto = PROTO_HTTP;
    stTcpBuf.stBinary.iLens = stBinary.iLens;
    stTcpBuf.stBinary.pBufer = new(std::nothrow) char[stTcpBuf.stBinary.iLens];
    H_ASSERT(NULL != stTcpBuf.stBinary.pBufer, "malloc memory error.");
    memcpy(stTcpBuf.stBinary.pBufer, stBinary.pBufer, stTcpBuf.stBinary.iLens);

    H_MSG *pMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pMsg, "malloc memory error.");
    pMsg->usEnevt = MSG_NET_READ;
    pMsg->pEvent = new(std::nothrow) char[sizeof(H_TCPBUF)];
    H_ASSERT(NULL != pMsg->pEvent, "malloc memory error.");
    memcpy(pMsg->pEvent, &stTcpBuf, sizeof(stTcpBuf));
    if (!pChan->Send(pMsg))
    {
        H_SafeDelArray(stTcpBuf.stBinary.pBufer);
        H_SafeDelete(pMsg->pEvent);
        H_SafeDelete(pMsg);
    }
}

void CNetWorker::dispNomal(H_PROTOTYPE &iProto, H_TCPBUF &stTcpBuf, H_Binary &stBinary)
{
    CChan *pChan(CMSGDispatch::getSingletonPtr()->getNetProto(iProto));
    if (NULL == pChan)
    {
        return;
    }
    
    stTcpBuf.iProto = iProto;
    stTcpBuf.stBinary.pBufer = NULL;
    stTcpBuf.stBinary.iLens = stBinary.iLens - sizeof(H_PROTOTYPE);
    if (H_INIT_NUMBER != stTcpBuf.stBinary.iLens)
    {
        stTcpBuf.stBinary.pBufer = new(std::nothrow) char[stTcpBuf.stBinary.iLens];
        H_ASSERT(NULL != stTcpBuf.stBinary.pBufer, "malloc memory error.");
        memcpy(stTcpBuf.stBinary.pBufer, stBinary.pBufer + sizeof(H_PROTOTYPE), stTcpBuf.stBinary.iLens);
    }

    H_MSG *pMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pMsg, "malloc memory error.");
    pMsg->usEnevt = MSG_NET_READ;
    pMsg->pEvent = new(std::nothrow) char[sizeof(H_TCPBUF)];
    H_ASSERT(NULL != pMsg->pEvent, "malloc memory error.");
    memcpy(pMsg->pEvent, &stTcpBuf, sizeof(stTcpBuf));
    if (!pChan->Send(pMsg))
    {
        H_SafeDelArray(stTcpBuf.stBinary.pBufer);
        H_SafeDelete(pMsg->pEvent);
        H_SafeDelete(pMsg);
    }
}

void CNetWorker::dispRPCCall(H_TCPBUF &stTcpBuf, H_Binary &stBinary)
{
    H_RPC *pRPC((H_RPC *)(stBinary.pBufer + sizeof(H_PROTOTYPE)));
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pRPC->acToTask));
    if (NULL == pChan)
    {
        return;
    }

    unsigned int uiLens((unsigned int)ntohl((u_long)pRPC->uiMsgLens));
    char *pRPCMsg(stBinary.pBufer + sizeof(H_PROTOTYPE) + sizeof(H_RPC));
    H_MSG *pMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pMsg, "malloc memory error.");
    pMsg->pEvent = new char[sizeof(H_RPC) + uiLens + sizeof(H_LINK)];
    H_ASSERT(NULL != pMsg->pEvent, "malloc memory error.");
    pMsg->usEnevt = MSG_NET_RPCCALL;
    memcpy(pMsg->pEvent, &stTcpBuf.stLink, sizeof(stTcpBuf.stLink));
    memcpy(pMsg->pEvent + sizeof(stTcpBuf.stLink), pRPC, sizeof(H_RPC));
    memcpy(pMsg->pEvent + sizeof(stTcpBuf.stLink) +sizeof(H_RPC), pRPCMsg, uiLens);
    if (!pChan->Send(pMsg))
    {
        H_SafeDelArray(pMsg->pEvent);
        H_SafeDelete(pMsg);
    }
}

void CNetWorker::dispRPCRtn(H_TCPBUF &stTcpBuf, H_Binary &stInBinary)
{
    H_RPCRTN *pRPCRtn((H_RPCRTN *)(stInBinary.pBufer + sizeof(H_PROTOTYPE)));
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pRPCRtn->acToTask));
    if (NULL == pChan)
    {
        return;
    }

    char *pRPCMsg = stInBinary.pBufer + sizeof(H_PROTOTYPE) + sizeof(H_RPCRTN);
    unsigned int uiLens((unsigned int)ntohl((u_long)pRPCRtn->uiMsgLens));
    unsigned int uiId((unsigned int)ntohl((u_long)pRPCRtn->uiId));

    H_Binary stBinary;
    stBinary.iLens = uiLens;
    stBinary.pBufer = new(std::nothrow) char[uiLens];
    H_ASSERT(NULL != stBinary.pBufer, "malloc memory error.");
    memcpy(stBinary.pBufer, pRPCMsg, uiLens);

    H_MSG *pMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pMsg, "malloc memory error.");
    pMsg->pEvent = new(std::nothrow) char[sizeof(stBinary) + sizeof(uiId)];
    H_ASSERT(NULL != pMsg->pEvent, "malloc memory error.");
    memcpy(pMsg->pEvent, &uiId, sizeof(uiId));
    memcpy(pMsg->pEvent + sizeof(uiId), &stBinary, sizeof(stBinary));
    pMsg->usEnevt = MSG_NET_RPCRTN;
    if (!pChan->Send(pMsg))
    {
        H_SafeDelArray(stBinary.pBufer);
        H_SafeDelArray(pMsg->pEvent);
        H_SafeDelete(pMsg);
    }
}

void CNetWorker::sendCMD(const char *pszTaskName, H_LINK *pLink, H_CMD *pCmd, H_TCPBUF &stTcpBuf)
{
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pszTaskName));
    if (NULL == pChan)
    {
        std::string strRtn("[ \"fail\", \"not find task.\"]");
        CSender::getSingletonPtr()->sendCMDRtn(stTcpBuf.stLink.sock, strRtn.c_str(), strRtn.size());
        return;
    }
    
    H_MSG *pMsg = new(std::nothrow) H_MSG;
    H_ASSERT(NULL != pMsg, "malloc memory error.");
    pMsg->usEnevt = MSG_NET_CMD;
    pMsg->pEvent = new(std::nothrow) char[sizeof(H_LINK) + sizeof(H_CMD)];
    H_ASSERT(NULL != pMsg->pEvent, "malloc memory error.");
    memcpy(pMsg->pEvent, pLink, sizeof(H_LINK));
    memcpy(pMsg->pEvent + sizeof(H_LINK), pCmd, sizeof(H_CMD));

    if (!pChan->Send(pMsg))
    {
        H_SafeDelArray(pMsg->pEvent);
        H_SafeDelete(pMsg);
    }
}

void CNetWorker::dispCMD(H_TCPBUF &stTcpBuf, H_Binary &stBinary)
{
    H_LINK stLink;
    stLink.usType = stTcpBuf.stLink.usType;
    H_CMD *pCmd((H_CMD*)(stBinary.pBufer + sizeof(H_PROTOTYPE)));
    if (0 == strcmp("hotfix", pCmd->acCommand) 
        && 0 == strcmp("all", pCmd->acTask))
    {
        stLink.sock = H_INVALID_SOCK;
        std::vector<std::string>::iterator itName;
        std::vector<std::string> vcName = CTaskMgr::getSingletonPtr()->getAllName();
        for (itName = vcName.begin(); vcName.end() != itName; ++itName)
        {
            sendCMD((*itName).c_str(), &stLink, pCmd, stTcpBuf);
        }

        std::string strRtn("[ \"ok\", \"\"]");        
        CSender::getSingletonPtr()->sendCMDRtn(stTcpBuf.stLink.sock, strRtn.c_str(), strRtn.size());

        return;
    }

    stLink.sock = stTcpBuf.stLink.sock;
    sendCMD(pCmd->acTask, &stLink, pCmd, stTcpBuf);
}

H_ENAMSP
