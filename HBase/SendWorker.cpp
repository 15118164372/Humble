
#include "SendWorker.h"
#include "Funcs.h"
#include "Log.h"

H_BNAMSP

CSendWorker::CSendWorker(void) : CTaskLazy<H_NetMsg>(H_QULENS_SENDER)
{

}

CSendWorker::~CSendWorker(void)
{

}

void CSendWorker::runTask(H_NetMsg *pMsg)
{
    if (!H_SockWrite(pMsg->sock, pMsg->stBuf.pBufer, pMsg->stBuf.iLens))
    {
        H_LOG(LOGLV_WARN, "%s", "sock write error.");
    }
    H_SafeDelArray(pMsg->stBuf.pBufer);
}

void CSendWorker::addSend(H_SOCK &sock, const char *pszBuf, const size_t &iLens, const bool &bCopy)
{
    if (H_INVALID_SOCK == sock
        || NULL == pszBuf)
    {
        return;
    }

    H_NetMsg *pMsg = newT();
    H_ASSERT(NULL != pMsg, "malloc memory error.");

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
