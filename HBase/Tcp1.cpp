
#include "Tcp1.h"
#include "Sender.h"

H_BNAMSP

SINGLETON_INIT(CTcp1)
CTcp1 objTcp1;

#define TCPBUFLENS_125 125
#define TCPBUFLENS_126 126
#define TCPBUFLENS_127 127

#define TCP_HRAD_MAXLENS  5

CTcp1::CTcp1(void)
{
    setName("tcp1");
}

CTcp1::~CTcp1(void)
{
}

bool CTcp1::readHead(char *pBuffer, const size_t &iLens, size_t &iBufLens, size_t &iHeadLens)
{
    char cFlag = pBuffer[0];
    if (cFlag <= TCPBUFLENS_125)
    {
        iHeadLens = sizeof(cFlag);
        iBufLens = cFlag;
    }
    else if (TCPBUFLENS_126 == cFlag)
    {
        size_t iNeedReadLens = sizeof(cFlag) + sizeof(unsigned short);
        if (iLens < iNeedReadLens)
        {
            return false;
        }

        iBufLens = ntohs(*((unsigned short *)(pBuffer + sizeof(cFlag))));
        iHeadLens = sizeof(cFlag) + sizeof(unsigned short);
    }
    else if (TCPBUFLENS_127 == cFlag)
    {
        size_t iNeedReadLens = sizeof(cFlag) + sizeof(unsigned int);
        if (iLens < iNeedReadLens)
        {
            return false;
        }

        iBufLens = (size_t)ntohl(*((unsigned int *)(pBuffer + sizeof(cFlag))));
        iHeadLens = sizeof(cFlag) + sizeof(unsigned int);
    }
    else
    {
        return false;
    }

    return true;
}

H_Binary CTcp1::parsePack(char *pAllBuf, const size_t &iLens, size_t &iParsed)
{
    H_Binary stBinary;
    size_t iBufLens(H_INIT_NUMBER);
    size_t iHeadLens(H_INIT_NUMBER);
    if (!readHead(pAllBuf, iLens, iBufLens, iHeadLens))
    {
        return stBinary;
    }    
    if (iBufLens + iHeadLens > iLens)
    {
        return stBinary;
    }

    iParsed = iBufLens + iHeadLens;
    stBinary.iLens = iBufLens;
    stBinary.pBufer = pAllBuf + iHeadLens;

    return stBinary;
}

size_t CTcp1::getHeadLens(const size_t &iLens)
{
    if (iLens <= TCPBUFLENS_125)
    {
        return sizeof(char);
    }
    else if ((iLens > TCPBUFLENS_125) && (iLens <= 0xFFFF))
    {
        return sizeof(char) + sizeof(unsigned short);
    }
    else
    {
        return sizeof(char) + sizeof(unsigned int);
    }
}

void CTcp1::Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens)
{    
    size_t iBodyLens(iLens + sizeof(iProto));
    size_t iHeadLens(getHeadLens(iBodyLens));    

    char *pBuf = new(std::nothrow) char[iHeadLens + iBodyLens];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    if (iBodyLens <= TCPBUFLENS_125)
    {
        pBuf[0] = (char)iBodyLens;
    }
    else if ((iBodyLens > TCPBUFLENS_125) && (iBodyLens <= 0xFFFF))
    {
        pBuf[0] = TCPBUFLENS_126;
        unsigned short usLen = ntohs((unsigned short)iBodyLens);
        memcpy(pBuf + sizeof(char), &usLen, sizeof(usLen));
    }
    else
    {
        pBuf[0] = TCPBUFLENS_127;
        unsigned int uiLen = ntohl((unsigned int)iBodyLens);
        memcpy(pBuf + sizeof(char), &uiLen, sizeof(uiLen));
    }

    iProto = H_NTOH(iProto);
    memcpy(pBuf + iHeadLens, &iProto, sizeof(iProto));
    if (NULL != pszMsg)
    {
        memcpy(pBuf + iHeadLens + sizeof(iProto), pszMsg, iLens);
    }

    CSender::getSingletonPtr()->Send(sock, pBuf, iHeadLens + iBodyLens, false);
}

H_ENAMSP
