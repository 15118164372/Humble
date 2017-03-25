
#include "Tcp3.h"
#include "Binary.h"

H_BNAMSP

SINGLETON_INIT(CTcp3)
CTcp3 objTcp3;

CTcp3::CTcp3(void)
{
    setName("tcp3");
}

CTcp3::~CTcp3(void)
{
}

int CTcp3::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    unsigned int uiBufLens(H_INIT_NUMBER);
    size_t iHeadLens(sizeof(uiBufLens));
    if (iHeadLens > iLens)
    {
        return H_INIT_NUMBER;
    }

    uiBufLens = ntohl(*(unsigned int*)(pAllBuf));
    if (H_INIT_NUMBER == uiBufLens)
    {
        return (int)iHeadLens;
    }
    if (uiBufLens + iHeadLens > iLens)
    {
        return H_INIT_NUMBER;
    }

    pBinary->setReadBuffer(pAllBuf + iHeadLens, uiBufLens);

    return (int)(uiBufLens + iHeadLens);
}

H_ENAMSP
