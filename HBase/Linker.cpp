
#include "Linker.h"
#include "NETAddr.h"
#include "NetWorker.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CLinker)
CLinker objLinker;

CLinker::CLinker(void) : CRecvTask<H_Link>(H_QULENS_LINKER)
{
}

CLinker::~CLinker(void)
{
}

void CLinker::runTask(H_Link *pMsg)
{    
    H_SOCK sock = H_ClientSock(pMsg->pHost, pMsg->usPort);
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    CNetWorker::getSingletonPtr()->addLinkEv(sock, pMsg->uiID);
}

void CLinker::addLink(const unsigned int &uiID, const char *pszHost, const unsigned short &usPort)
{
    H_Link *pLinker = newT();
    pLinker->usPort = usPort;
    pLinker->uiID = uiID;
    pLinker->pHost = (char*)pszHost;

    addTask(pLinker);
}

H_ENAMSP
