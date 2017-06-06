
#include "Tcp2.h"
#include "Sender.h"

H_BNAMSP

SINGLETON_INIT(CTcp2)
CTcp2 objTcp2;

CTcp2::CTcp2(void)
{
    setName("tcp2");
}

CTcp2::~CTcp2(void)
{
}

H_Binary CTcp2::parsePack(char *pAllBuf, const size_t &iLens, size_t &iParsed)
{
    H_Binary stBinary;
    unsigned int uiBufLens(H_INIT_NUMBER);
    size_t iHeadLens(sizeof(uiBufLens));
    if (iHeadLens > iLens)
    {
        return stBinary;
    }

    uiBufLens = ntohl(*(unsigned int*)(pAllBuf));    
    if (uiBufLens + iHeadLens > iLens)
    {
        return stBinary;
    }

    iParsed = uiBufLens + iHeadLens;
    stBinary.iLens = uiBufLens;
    stBinary.pBufer = pAllBuf + iHeadLens;

    return stBinary;
}

void CTcp2::Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens)
{
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    size_t iBodyLens(iLens + sizeof(iProto));
    unsigned int uiHead((unsigned int)ntohl((u_long)iBodyLens));
    iProto = H_NTOH(iProto);

    char *pBuf = new(std::nothrow) char[iBodyLens + sizeof(unsigned int)];
    H_ASSERT(NULL != pBuf, "malloc memory error.");    
    memcpy(pBuf, &uiHead, sizeof(uiHead));
    memcpy(pBuf + sizeof(uiHead), &iProto, sizeof(iProto));
    if (NULL != pszMsg)
    {
        memcpy(pBuf + sizeof(uiHead) + sizeof(iProto), pszMsg, iLens);
    }

    CSender::getSingletonPtr()->Send(sock, pBuf, iBodyLens + sizeof(unsigned int), false);
}

H_ENAMSP
