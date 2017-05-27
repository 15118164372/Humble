
#include "Sender.h"
#include "Log.h"
#include "Funcs.h"
#include "Thread.h"
#include "HEnum.h"

H_BNAMSP

SINGLETON_INIT(CSender)
CSender objSender;

CSender::CSender(void) : m_usWorkerNum(H_INIT_NUMBER), m_pWorker(NULL)
{

}

CSender::~CSender(void)
{   
    H_SafeDelArray(m_pWorker);
}

void CSender::startSender(const unsigned short usNum)
{
    m_usWorkerNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() : usNum);
    m_pWorker = new(std::nothrow) CSendWorker[m_usWorkerNum];
    H_ASSERT(NULL != m_pWorker, "malloc memory error.");

    for (unsigned short usIndex = H_INIT_NUMBER; usIndex < m_usWorkerNum; usIndex++)
    {
        CThread::Creat(&m_pWorker[usIndex]);
        m_pWorker[usIndex].waitStart();
    }
}

void CSender::stopSender(void)
{
    for (unsigned short usIndex = H_INIT_NUMBER; usIndex < m_usWorkerNum; usIndex++)
    {
        m_pWorker[usIndex].Join();
    }
}

void CSender::sendCMDRtn(H_SOCK &sock, const char *pszBuf, const size_t &iLens)
{
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    unsigned int uiLens((unsigned int)ntohl((u_long)iLens));
    char *pBuf = new(std::nothrow) char[iLens + sizeof(unsigned int)];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    memcpy(pBuf, &uiLens, sizeof(uiLens));
    if (NULL != pszBuf)
    {
        memcpy(pBuf + sizeof(uiLens), pszBuf, iLens);
    }

    Send(sock, pBuf, iLens + sizeof(unsigned int), false);
}

void CSender::sendRPCCall(H_SOCK &sock, unsigned int &uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask,
    const char *pMsg, const size_t &iLens)
{
    H_ASSERT(NULL != pszRPCName 
        && NULL != pszToTask 
        && NULL != pszSrcTask, "got null pointer.");
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    H_PROTOTYPE iProto(H_NTOH(PROTO_RPCCAL));
    unsigned int uiLens((unsigned int)ntohl((u_long)(sizeof(H_RPC) + iLens + sizeof(H_PROTOTYPE))));
    char *pBuf = new(std::nothrow) char[sizeof(H_RPC) + iLens + sizeof(H_PROTOTYPE) + sizeof(uiLens)];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    H_RPC stRPC;
    H_Zero(&stRPC, sizeof(stRPC));
    stRPC.uiId = (unsigned int)ntohl((u_long)uiId);
    stRPC.uiMsgLens = (unsigned int)ntohl((u_long)iLens);
    memcpy(stRPC.acRPC, pszRPCName, strlen(pszRPCName));
    memcpy(stRPC.acSrcTask, pszSrcTask, strlen(pszSrcTask));
    memcpy(stRPC.acToTask, pszToTask, strlen(pszToTask));

    memcpy(pBuf, &uiLens, sizeof(uiLens));
    memcpy(pBuf + sizeof(uiLens), &iProto, sizeof(iProto));
    memcpy(pBuf + sizeof(uiLens) + sizeof(iProto), &stRPC, sizeof(stRPC));
    if (NULL != pMsg)
    {
        memcpy(pBuf + sizeof(uiLens) + sizeof(iProto) + sizeof(stRPC), pMsg, iLens);
    }    

    Send(sock, pBuf, sizeof(H_RPC) + iLens + sizeof(H_PROTOTYPE) + sizeof(uiLens), false);
}

void CSender::sendRPCRtn(H_SOCK &sock, H_RPC *pRPC, const char *pszMsg, const size_t &iLens)
{
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    H_PROTOTYPE iProto(H_NTOH(PROTO_RPCRTN));
    unsigned int uiLens((unsigned int)ntohl((u_long)(sizeof(H_RPCRTN) + iLens + sizeof(H_PROTOTYPE))));
    char *pBuf = new(std::nothrow) char[sizeof(H_RPCRTN) + iLens + sizeof(H_PROTOTYPE) + sizeof(uiLens)];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    H_RPCRTN stRPCRtn;
    stRPCRtn.uiId = pRPC->uiId;
    stRPCRtn.uiMsgLens = (unsigned int)ntohl((u_long)iLens);
    H_Zero(stRPCRtn.acToTask, sizeof(stRPCRtn.acToTask));
    memcpy(stRPCRtn.acToTask, pRPC->acSrcTask, strlen(pRPC->acSrcTask));

    memcpy(pBuf, &uiLens, sizeof(uiLens));
    memcpy(pBuf + sizeof(uiLens), &iProto, sizeof(iProto));
    memcpy(pBuf + sizeof(uiLens) + sizeof(iProto), &stRPCRtn, sizeof(stRPCRtn));
    if (NULL != pszMsg)
    {
        memcpy(pBuf + sizeof(uiLens) + sizeof(iProto) + sizeof(stRPCRtn), pszMsg, iLens);
    }

    Send(sock, pBuf, sizeof(H_RPCRTN) + iLens + sizeof(H_PROTOTYPE) + sizeof(uiLens), false);
}

H_ENAMSP
