
#include "SendWorker.h"
#include "Funcs.h"
#include "Log.h"

H_BNAMSP

CSendWorker::CSendWorker(void) : CTaskLazy<H_NetSendMsg>(H_QULENS_SENDER)
{
}

CSendWorker::~CSendWorker(void)
{
}

void CSendWorker::runTask(H_NetSendMsg *pMsg)
{
    int iRtn(H_SockWrite(pMsg->sock, pMsg->stBuf.pBufer, pMsg->stBuf.iLens));
    if (H_RTN_OK != iRtn)
    {
        H_LOG(LOGLV_ERROR, "%s", H_SockError2Str(iRtn));
    }
    if (NULL == pMsg->pRef)
    {
        H_SafeDelArray(pMsg->stBuf.pBufer);
        return;
    }
    
    if (1 == H_AtomicAdd(pMsg->pRef, -1))
    {
        H_SafeDelArray(pMsg->stBuf.pBufer);
        H_SafeDelete(pMsg->pRef);
    }
}

bool CSendWorker::broadCast(H_SOCK &sock, unsigned int *pRef, const char *pszBuf, const size_t &iLens)
{
    H_NetSendMsg *pMsg(newT());
    H_ASSERT(NULL != pMsg, "malloc memory error.");

    pMsg->sock = sock;
    pMsg->pRef = pRef;
    pMsg->stBuf.pBufer = (char*)pszBuf;
    pMsg->stBuf.iLens = iLens;
    if (!addTask(pMsg))
    {
        H_SafeDelete(pMsg);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");

        return false;
    }

    return true;
}

void CSendWorker::addSend(H_SOCK &sock, const char *pszBuf, const size_t &iLens, const bool &bCopy)
{
    H_NetSendMsg *pMsg(newT());
    H_ASSERT(NULL != pMsg, "malloc memory error.");

    pMsg->pRef = NULL;
    pMsg->sock = sock;
    pMsg->stBuf.iLens = iLens;
    if (bCopy)
    {
        pMsg->stBuf.pBufer = new(std::nothrow) char[iLens];
        H_ASSERT(NULL != pMsg->stBuf.pBufer, "malloc memory error.");
        memcpy(pMsg->stBuf.pBufer, pszBuf, iLens);
    }
    else
    {
        pMsg->stBuf.pBufer = (char*)pszBuf;
    }

    if (!addTask(pMsg))
    {
        H_SafeDelArray(pMsg->stBuf.pBufer);
        H_SafeDelete(pMsg);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

H_ENAMSP
