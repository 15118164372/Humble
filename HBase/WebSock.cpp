
#include "WebSock.h"
#include "Funcs.h"
#include "Binary.h"
#include "Base64.h"
#include "NetWorker.h"

H_BNAMSP

SINGLETON_INIT(CWebSock)
CWebSock objWebSock;

#define WebSock_ShakeHands_EndFlag "\r\n\r\n"
#define ShakeHands_SplitFlag "\r\n"

#define PAYLOADLENS_125 125
#define PAYLOADLENS_126 126
#define PAYLOADLENS_127 127

#define FRAME_HEAD_BASE_LEN 6
#define FRAME_HEAD_EXT16_LEN 8
#define FRAME_HEAD_EXT64_LEN 14

enum
{
    WSS_INIT = 0,
    WSS_SHAKEHANDS,
};

enum  WebSockOpCode
{
    WSOCK_CONTINUATION = 0x00,
    WSOCK_TEXTFRAME = 0x01,
    WSOCK_BINARYFRAME = 0x02,
    WSOCK_CLOSE = 0x08,
    WSOCK_PING = 0x09,
    WSOCK_PONG = 0x0A
};

//websock 解析后的头
struct WebSockFram
{
    char cFin;
    char cRsv1;
    char cRsv2;
    char cRsv3;
    WebSockOpCode emOpCode;
    char cMask;
    unsigned char ucPayloadLen;
    size_t uiDataLens;
    char acMaskKey[4];
    WebSockFram(void) : cFin(H_INIT_NUMBER), cRsv1(H_INIT_NUMBER),
        cRsv2(H_INIT_NUMBER), cRsv3(H_INIT_NUMBER), emOpCode(WSOCK_CONTINUATION),
        cMask(H_INIT_NUMBER), ucPayloadLen(H_INIT_NUMBER), uiDataLens(H_INIT_NUMBER)
    {
        H_Zero(acMaskKey, sizeof(acMaskKey));
    };
};

CWebSock::CWebSock(void)
{
    m_iEndFlagLens = strlen(WebSock_ShakeHands_EndFlag);
    m_strVersion = H_FormatStr("%d-%d-%d", H_MAJOR, H_MINOR, H_RELEASE);
    setName("websock");
}

CWebSock::~CWebSock(void)
{
}

std::string CWebSock::parseKey(class CBinary *pBinary) const
{
    int iLens = pBinary->Find("Sec-WebSocket-Key");
    if (-1 == iLens)
    {
        return "";
    }

    pBinary->skipRead(iLens);
    luabridge::H_LBinary stBin = pBinary->readLine();
    
    char *pPos = strstr(stBin.pBufer, ":");
    if (NULL == pPos)
    {
        return "";
    }

    return H_Trim(std::string(pPos + 1, stBin.iLens - (pPos - stBin.pBufer + 1)));
}

std::string CWebSock::createKey(std::string &strKey)
{    
    unsigned char m_acShaKey[20] = { 0 };
    strKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    m_objSha1.Update(strKey.c_str(), (unsigned int)strKey.size());
    m_objSha1.Final();
    if (!m_objSha1.GetHash(m_acShaKey))
    {
        return "";
    }

    return H_B64Encode(m_acShaKey, sizeof(m_acShaKey));
}

std::string CWebSock::createResponse(const std::string &strKey) const
{
    std::string strRtn =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Server: Humble version:" + m_strVersion + "\r\n"
        "Sec-WebSocket-Accept: " + strKey + "\r\n"
        "Sec-WebSocket-Origin: null\r\n"
        "\r\n";

    return strRtn;
}

int CWebSock::shakeHands(struct H_Session *pSession, char *pAllBuf, const size_t &iLens)
{
    char *pPos = strstr(pAllBuf, WebSock_ShakeHands_EndFlag);
    if (NULL == pPos)
    {
        return H_INIT_NUMBER;
    }

    size_t iParsed(pPos - pAllBuf + m_iEndFlagLens);

    CBinary objTmp;
    objTmp.setReadBuffer(pAllBuf, iParsed);
    std::string strVal = parseKey(&objTmp);
    if (strVal.empty())
    {
        CNetWorker::getSingletonPtr()->closeSock(pSession->sock, pSession->uiSession);
        return H_RTN_FAILE;
    }

    //创建返回的key
    strVal = createKey(strVal);
    if (strVal.empty())
    {
        CNetWorker::getSingletonPtr()->closeSock(pSession->sock, pSession->uiSession);
        return H_RTN_FAILE;
    }

    strVal = createResponse(strVal);
    bufferevent_write(pSession->pBev, strVal.c_str(), strVal.size());
    pSession->uiStatus = WSS_SHAKEHANDS;

    return (int)iParsed;
}

int CWebSock::parseHead(struct WebSockFram *pFram, const char *pBuffer, const size_t &iLens)
{
    int iHeadLens(H_INIT_NUMBER);

    if (FRAME_HEAD_BASE_LEN > iLens)
    {
        return H_INIT_NUMBER;
    }

    pFram->cFin = (pBuffer[0] & 0x80) >> 7;
    pFram->cRsv1 = (pBuffer[0] & 0x40) >> 6;
    pFram->cRsv2 = (pBuffer[0] & 0x20) >> 5;
    pFram->cRsv3 = (pBuffer[0] & 0x10) >> 4;
    pFram->emOpCode = (WebSockOpCode)(pBuffer[0] & 0xF);
    pFram->cMask = (pBuffer[1] & 0x80) >> 7;
    pFram->ucPayloadLen = pBuffer[1] & 0x7F;
    if ((1 != pFram->cMask)
        || (0 != pFram->cRsv1)
        || (0 != pFram->cRsv2)
        || (0 != pFram->cRsv3))
    {
        return H_RTN_FAILE;
    }

    if (pFram->ucPayloadLen <= PAYLOADLENS_125)
    {
        iHeadLens = FRAME_HEAD_BASE_LEN;

        pFram->acMaskKey[0] = pBuffer[2];
        pFram->acMaskKey[1] = pBuffer[3];
        pFram->acMaskKey[2] = pBuffer[4];
        pFram->acMaskKey[3] = pBuffer[5];
        pFram->uiDataLens = pFram->ucPayloadLen;
    }
    else if (PAYLOADLENS_126 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT16_LEN)
        {
            return H_INIT_NUMBER;
        }

        iHeadLens = FRAME_HEAD_EXT16_LEN;

        pFram->acMaskKey[0] = pBuffer[4];
        pFram->acMaskKey[1] = pBuffer[5];
        pFram->acMaskKey[2] = pBuffer[6];
        pFram->acMaskKey[3] = pBuffer[7];
        pFram->uiDataLens = htons(*(u_short *)(pBuffer + 2));
    }
    else if (PAYLOADLENS_127 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT64_LEN)
        {
            return H_INIT_NUMBER;
        }

        iHeadLens = FRAME_HEAD_EXT64_LEN;

        pFram->acMaskKey[0] = pBuffer[10];
        pFram->acMaskKey[1] = pBuffer[11];
        pFram->acMaskKey[2] = pBuffer[12];
        pFram->acMaskKey[3] = pBuffer[13];
        pFram->uiDataLens = (size_t)ntohl64(*(uint64_t *)(pBuffer + 2));
    }
    else
    {        
        H_Printf("%s", "invalid payload len.");
        return H_RTN_FAILE;
    }

    return iHeadLens;
}

char *CWebSock::parseData(struct WebSockFram *pFram, char *pBuffer, const size_t &iLens)
{
    for (size_t i = 0; i < iLens; ++i)
    {
        pBuffer[i] = pBuffer[i] ^ pFram->acMaskKey[i % 4];
    }

    return pBuffer;
}

int CWebSock::parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    std::string strval(pAllBuf, iLens);
    //握手
    if (WSS_SHAKEHANDS != pSession->uiStatus)
    {
        if (pSession->bLinker)
        {
            char *pPos = strstr(pAllBuf, WebSock_ShakeHands_EndFlag);
            if (NULL == pPos)
            {
                return H_INIT_NUMBER;
            }
            pSession->uiStatus = WSS_SHAKEHANDS;

            return (int)(pPos - pAllBuf + m_iEndFlagLens);
        }
        else
        {
            return shakeHands(pSession, pAllBuf, iLens);
        }        
    }

    //正常数据处理
    int iHeadLens(H_INIT_NUMBER);
    size_t iParsed(H_INIT_NUMBER);
    WebSockFram stFram;

    //解析头
    iHeadLens = parseHead(&stFram, pAllBuf, iLens);
    if (H_INIT_NUMBER == iHeadLens)
    {
        return H_INIT_NUMBER;
    }
    if (H_RTN_FAILE == iHeadLens)
    {
        CNetWorker::getSingletonPtr()->closeSock(pSession->sock, pSession->uiSession);
        return H_RTN_FAILE;
    }

    iParsed = stFram.uiDataLens + iHeadLens;
    if (iParsed > iLens)
    {
        return H_INIT_NUMBER;
    }
    if (H_INIT_NUMBER != stFram.uiDataLens)
    {
        char *pParsed = parseData(&stFram, pAllBuf + iHeadLens, stFram.uiDataLens);
        pBinary->setReadBuffer(pParsed, stFram.uiDataLens);
    }
    
    pBinary->setUint8(stFram.emOpCode);
    pBinary->setUint8(stFram.cFin);
    
    return (int)iParsed;
}

void CWebSock::createHead(std::string *pBuf, const char &cFin, const char &cCode,
    const size_t &iDataLens)
{
    char acWebSockHead[FRAME_HEAD_EXT64_LEN] = { 0 };

    acWebSockHead[0] = (char)(cCode | 0x80);
    if (0 == cFin)
    {
        acWebSockHead[0] = (acWebSockHead[0] & 0x7f);
    }

    if (iDataLens <= PAYLOADLENS_125)
    {
        acWebSockHead[1] = (char)iDataLens;
        pBuf->append(acWebSockHead, FRAME_HEAD_BASE_LEN - 4);
    }
    else if ((iDataLens > PAYLOADLENS_125) && (iDataLens <= 0xFFFF))
    {
        acWebSockHead[1] = PAYLOADLENS_126;
        uint16_t uiLen16 = htons((u_short)iDataLens);
        memcpy(acWebSockHead + 2, &uiLen16, sizeof(uiLen16));

        pBuf->append(acWebSockHead, FRAME_HEAD_EXT16_LEN - 4);
    }
    else
    {
        acWebSockHead[1] = PAYLOADLENS_127;
        uint64_t uiLens64 = ntohl64((uint64_t)iDataLens);
        memcpy(acWebSockHead + 2, &uiLens64, sizeof(uiLens64));
        pBuf->append(acWebSockHead, FRAME_HEAD_EXT64_LEN - 4);
    }
}

H_ENAMSP
