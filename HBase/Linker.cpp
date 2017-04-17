
#include "Linker.h"
#include "NETAddr.h"
#include "NetWorker.h"

H_BNAMSP

SINGLETON_INIT(CLinker)
CLinker objLinker;

CLinker::CLinker(void) : CRecvTask<H_Link>(H_QULENS_LINKER)
{
}

CLinker::~CLinker(void)
{
}

H_SOCK CLinker::initSock(const char *pszHost, const unsigned short &usPort)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        H_Printf("%s", "setAddr error.");
        return H_INVALID_SOCK;
    }

    //´´½¨socket
    H_SOCK sock = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == sock)
    {
        H_Printf("%s", "creat socket error.");
        return H_INVALID_SOCK;
    }

    if (0 != connect(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        H_Printf("connect %s on port %d error.", pszHost, usPort);
        evutil_closesocket(sock);

        return H_INVALID_SOCK;
    }

    return sock;
}

void CLinker::runTask(H_Link *pMsg)
{
    H_Printf("try link %s on port %d", pMsg->pHost, pMsg->usPort);
    H_SOCK sock = initSock(pMsg->pHost, pMsg->usPort);
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
