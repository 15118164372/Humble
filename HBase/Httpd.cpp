
#include "Httpd.h"
#include "Funcs.h"
#include "Sender.h"

H_BNAMSP

SINGLETON_INIT(CHttp)
CHttp objHttp;

#define Http_HeadEndFlag "\r\n\r\n"
#define Http_ChunkEndFlag "\r\n0\r\n\r\n"
#define Http_ContentLensStr "Content-Length:"
#define Http_ChunkedStr "Transfer-Encoding: chunked"

CHttp::CHttp(void)
{
    m_iHeadEndFlagLens = strlen(Http_HeadEndFlag);
    m_iChunkEndFlagLens = strlen(Http_ChunkEndFlag);
    m_iContentLens = strlen(Http_ContentLensStr);
    setName("http");
}

CHttp::~CHttp(void)
{
}

size_t CHttp::getHeadLens(const char *pBuffer)
{
    const char *pFind = strstr(pBuffer, Http_HeadEndFlag);
    if (NULL == pFind)
    {
        return H_INIT_NUMBER;
    }

    return pFind - pBuffer + m_iHeadEndFlagLens;
}

bool CHttp::getContentLens(const char *pszHead, const size_t &iHeadLens, size_t &iContentLens)
{
    const char *pBegin = strstr(pszHead, Http_ContentLensStr);
    if (NULL == pBegin)
    {
        return false;
    }

    iContentLens = atoi(pBegin + m_iContentLens);

    return true;
}

bool CHttp::checkChunk(const char *pszHead, const size_t &iHeadLens)
{
    if (NULL == strstr(pszHead, Http_ChunkedStr))
    {
        return false;
    }

    return true;
}

bool CHttp::getChunkLens(const char *pBuffer, size_t &iChunkLens)
{
    const char *pChunck = strstr(pBuffer, Http_ChunkEndFlag);
    if (NULL == pChunck)
    {
        return false;
    }

    iChunkLens = pChunck - pBuffer + m_iChunkEndFlagLens;

    return true;
}

H_Binary CHttp::parsePack(char *pAllBuf, const size_t &iLens, size_t &iParsed)
{
    H_Binary stBinary;
    size_t iHeadLens = getHeadLens(pAllBuf);
    if (H_INIT_NUMBER == iHeadLens)
    {
        return stBinary;
    }

    size_t iContentLens = H_INIT_NUMBER;
    if (!getContentLens(pAllBuf, iHeadLens, iContentLens))
    {
        if (checkChunk(pAllBuf, iHeadLens))
        {
            if (!getChunkLens(pAllBuf + iHeadLens, iContentLens))
            {
                return stBinary;
            }
        }
    }    

    if (H_INIT_NUMBER == iContentLens)
    {
        iParsed = iHeadLens;

        stBinary.iLens = iHeadLens;
        stBinary.pBufer = pAllBuf;

        return stBinary;
    }

    size_t iTotalLens(iHeadLens + iContentLens);
    if (iLens < iTotalLens)
    {
        return stBinary;
    }

    iParsed = iTotalLens;

    stBinary.iLens = iTotalLens;
    stBinary.pBufer = pAllBuf;

    return stBinary;
}

void CHttp::Response(H_SOCK &sock, H_PROTOTYPE &, const char *pszMsg, const size_t &iLens)
{
    CSender::getSingletonPtr()->Send(sock, pszMsg, iLens);
}

H_ENAMSP
