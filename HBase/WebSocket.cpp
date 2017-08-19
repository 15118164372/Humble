
#include "WebSocket.h"
#include "Funcs.h"
#include "SHA1.h"
#include "Base64.h"
#include "Sender.h"

H_BNAMSP

SINGLETON_INIT(CWebSocket)
CWebSocket objWebSocket;

#define WebSock_ShakeHands_EndFlag "\r\n\r\n"
#define ShakeHands_SplitFlag "\r\n"
#define WebSocket_Version  13
#define WebSocket_Key "Sec-WebSocket-Key"
#define WebSocket_VersionKey "Sec-WebSocket-Version"

#define PAYLOADLENS_125 125
#define PAYLOADLENS_126 126
#define PAYLOADLENS_127 127

enum  WebSockOpCode
{
    WSOCK_CONTINUATION = 0x00,
    WSOCK_TEXTFRAME = 0x01,
    WSOCK_BINARYFRAME = 0x02,
    WSOCK_CLOSE = 0x08,
    WSOCK_PING = 0x09,
    WSOCK_PONG = 0x0A
};

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
    };
};

CWebSocket::CWebSocket(void)
{
    setName(H_PARSER_WS);
    m_iShakeHandsEndFlagLens = strlen(WebSock_ShakeHands_EndFlag);
    m_iKeyLens = strlen(WebSocket_Key);
    m_iVesionLens = strlen(WebSocket_VersionKey);
    m_strVersion = H_FormatStr("%d-%d-%d", H_MAJOR, H_MINOR, H_RELEASE);
}

CWebSocket::~CWebSocket(void)
{
}

std::string CWebSocket::parseKey(char *pMsg, const size_t &iLens)
{
    std::string strKey;

    char *pPos(strstr(pMsg, WebSocket_Key));
    if (NULL == pPos)
    {
        return "";
    }

    char *pTmp(pPos + m_iKeyLens);
    pPos = strstr(pTmp, ":");
    if (NULL == pPos)
    {
        return "";
    }

    pTmp = pPos + 1;
    pPos = strstr(pTmp, ShakeHands_SplitFlag);
    if (NULL == pPos)
    {
        return "";
    }

    strKey.append(pTmp, (size_t)(pPos - pTmp));
    strKey = H_Trim(strKey);

    pPos = strstr(pMsg, WebSocket_VersionKey);
    if (NULL == pPos)
    {
        return "";
    }

    pTmp = pPos + m_iVesionLens;
    pPos = strstr(pTmp, ":");
    if (NULL == pPos)
    {
        return "";
    }

    if (WebSocket_Version != atoi(pPos + 1))
    {
        return "";
    }

    return strKey;
}

std::string CWebSocket::createChallengeKey(std::string &strKey)
{
    unsigned char acShaKey[20];
    strKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    CSHA1 objSha1;
    objSha1.Update((const char *)strKey.c_str(), (unsigned int)strKey.size());
    objSha1.Final();
    if (!objSha1.GetHash(acShaKey))
    {
        return "";
    }

    return H_B64Encode((const char*)acShaKey, sizeof(acShaKey));
}

std::string CWebSocket::createHandshakeResponse(const bool &bWithMQTT, std::string &strKey)
{
    if (!bWithMQTT)
    {
        return
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Server: QServer version:" + m_strVersion + "\r\n"
            "Sec-WebSocket-Accept: " + strKey + "\r\n"
            "Sec-WebSocket-Origin: null\r\n"
            "\r\n";
    }
    
    return
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Server: QServer version:" + m_strVersion + "\r\n"
        "Sec-WebSocket-Accept: " + strKey + "\r\n"
        "Sec-WebSocket-Origin: null\r\n"
        "Sec-WebSocket-Protocol: mqtt\r\n"
        "\r\n";
}

bool CWebSocket::handShake(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &bCLose)
{
    char *pPos = strstr(pAllBuf, WebSock_ShakeHands_EndFlag);
    if (NULL == pPos)
    {
        return false;
    }

    iParsed = (size_t)(pPos - pAllBuf) + m_iShakeHandsEndFlagLens;
    std::string strVal(parseKey(pAllBuf, iParsed));
    if (strVal.empty())
    {
        bCLose = true;
        return false;
    }

    strVal = createChallengeKey(strVal);
    if (strVal.empty())
    {
        bCLose = true;
        return false;
    }

    pSession->bWSWithMQTT = (NULL == strstr(pAllBuf, "mqtt") ? false : true);
    strVal = createHandshakeResponse(pSession->bWSWithMQTT, strVal);
    CSender::getSingletonPtr()->Send(pSession->stLink.sock, strVal.c_str(), strVal.size());

    return true;
}

bool CWebSocket::parseHead(char *pAllBuf, const size_t &iLens, struct WebSockFram *pFram, size_t &iParsed, bool &bCLose)
{
    if (FRAME_HEAD_BASE_LEN > iLens)
    {
        return false;
    }

    pFram->cFin = (pAllBuf[0] & 0x80) >> 7;
    pFram->cRsv1 = (pAllBuf[0] & 0x40) >> 6;
    pFram->cRsv2 = (pAllBuf[0] & 0x20) >> 5;
    pFram->cRsv3 = (pAllBuf[0] & 0x10) >> 4;
    pFram->emOpCode = (WebSockOpCode)(pAllBuf[0] & 0xF);
    pFram->cMask = (pAllBuf[1] & 0x80) >> 7;
    pFram->ucPayloadLen = pAllBuf[1] & 0x7F;
    if ((1 != pFram->cMask)
        || (0 != pFram->cRsv1)
        || (0 != pFram->cRsv2)
        || (0 != pFram->cRsv3))
    {
        bCLose = true;
        return false;
    }

    if (pFram->ucPayloadLen <= PAYLOADLENS_125)
    {
        iParsed = FRAME_HEAD_BASE_LEN;
        pFram->acMaskKey[0] = pAllBuf[2];
        pFram->acMaskKey[1] = pAllBuf[3];
        pFram->acMaskKey[2] = pAllBuf[4];
        pFram->acMaskKey[3] = pAllBuf[5];
        pFram->uiDataLens = pFram->ucPayloadLen;
    }
    else if (PAYLOADLENS_126 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT16_LEN)
        {
            return false;
        }

        iParsed = FRAME_HEAD_EXT16_LEN;
        pFram->acMaskKey[0] = pAllBuf[4];
        pFram->acMaskKey[1] = pAllBuf[5];
        pFram->acMaskKey[2] = pAllBuf[6];
        pFram->acMaskKey[3] = pAllBuf[7];
        pFram->uiDataLens = htons(*(u_short *)(pAllBuf + 2));
    }
    else if (PAYLOADLENS_127 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT64_LEN)
        {
            return false;
        }

        iParsed = FRAME_HEAD_EXT64_LEN;
        pFram->acMaskKey[0] = pAllBuf[10];
        pFram->acMaskKey[1] = pAllBuf[11];
        pFram->acMaskKey[2] = pAllBuf[12];
        pFram->acMaskKey[3] = pAllBuf[13];
        pFram->uiDataLens = (size_t)ntohl64(*(uint64_t *)(pAllBuf + 2));
    }
    else
    {
        bCLose = true;
        return false;
    }

    return true;
}

H_Binary CWebSocket::parsePack(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &bCLose)
{
    H_Binary stBinary;
    size_t iHeadLens(H_INIT_NUMBER);
    WebSockFram stFram;
    bool bOk(parseHead(pAllBuf, iLens, &stFram, iHeadLens, bCLose));
    if (bCLose || !bOk)
    {
        return stBinary;
    }

    if (stFram.uiDataLens + iHeadLens > iLens)
    {
        return stBinary;
    }

    if (stFram.uiDataLens > 0)
    {
        stBinary.pBufer = pAllBuf + iHeadLens;
        for (size_t i = 0; i < stFram.uiDataLens; ++i)
        {
            stBinary.pBufer[i] = stBinary.pBufer[i] ^ stFram.acMaskKey[i % 4];
        }
    }

    iParsed = stFram.uiDataLens + iHeadLens;
    stBinary.iLens = stFram.uiDataLens;
    //必须为完整的帧
    if ((WSOCK_CONTINUATION == stFram.emOpCode)
        || (0 == stFram.cFin))
    {
        bCLose = true;
        return stBinary;
    }

    //mqtt必须使用WSOCK_BINARYFRAME
    if (pSession->bWSWithMQTT && WSOCK_BINARYFRAME != stFram.emOpCode)
    {
        bCLose = true;
        return stBinary;
    }

    //控制帧处理
    switch (stFram.emOpCode)
    {
        case WSOCK_CLOSE:
        {
            bCLose = true;
        }
        break;
        case WSOCK_PING:
        {
            stBinary.pBufer = NULL;
            size_t iHeadLens(H_INIT_NUMBER);
            char acHead[FRAME_HEAD_EXT64_LEN];
            createHead(WSOCK_PONG, 0, acHead, iHeadLens);

            CSender::getSingletonPtr()->Send(pSession->stLink.sock, acHead, iHeadLens);
        }
        break;
        default:
            break;
    }

    return stBinary;
}

void CWebSocket::createHead(const unsigned short usCode, const size_t &iDataLens,
    char acHead[FRAME_HEAD_EXT64_LEN], size_t &iOutLens)
{
    acHead[0] = (char)(usCode | 0x80);

    if (iDataLens <= PAYLOADLENS_125)
    {
        acHead[1] = (char)iDataLens;
        iOutLens = FRAME_HEAD_BASE_LEN - 4;
    }
    else if ((iDataLens > PAYLOADLENS_125) && (iDataLens <= 0xFFFF))
    {
        acHead[1] = PAYLOADLENS_126;
        uint16_t uiLen16 = htons((u_short)iDataLens);
        memcpy(acHead + 2, &uiLen16, sizeof(uiLen16));

        iOutLens = FRAME_HEAD_EXT16_LEN - 4;
    }
    else
    {
        acHead[1] = PAYLOADLENS_127;
        uint64_t uiLens64 = ntohl64((uint64_t)iDataLens);
        memcpy(acHead + 2, &uiLens64, sizeof(uiLens64));

        iOutLens = FRAME_HEAD_EXT64_LEN - 4;
    }
}

H_Binary CWebSocket::createPack(H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens)
{
    iProto = H_NTOH(iProto);

    size_t iHeadLens(H_INIT_NUMBER);
    size_t iBodyLens(sizeof(iProto) + iLens);
    char acHead[FRAME_HEAD_EXT64_LEN];

    createHead(WSOCK_BINARYFRAME, iBodyLens, acHead, iHeadLens);

    char *pBuf = new(std::nothrow) char[iHeadLens + iBodyLens];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    memcpy(pBuf, acHead, iHeadLens);
    memcpy(pBuf + iHeadLens, &iProto, sizeof(iProto));
    if (NULL != pszMsg)
    {
        memcpy(pBuf + iHeadLens + sizeof(iProto), pszMsg, iLens);
    }

    H_Binary stBinary;
    stBinary.iLens = iHeadLens + iBodyLens;
    stBinary.pBufer = pBuf;

    return stBinary;
}

void CWebSocket::Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens)
{
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    H_Binary stBinary(createPack(iProto, pszMsg, iLens));
    CSender::getSingletonPtr()->Send(sock, stBinary.pBufer, stBinary.iLens, false);
}

void CWebSocket::resWithOutProto(H_SOCK &sock, const char *pszMsg, const size_t &iLens)
{
    if (H_INVALID_SOCK == sock)
    {
        return;
    }

    size_t iHeadLens(H_INIT_NUMBER);
    char acHead[FRAME_HEAD_EXT64_LEN];

    createHead(WSOCK_BINARYFRAME, iLens, acHead, iHeadLens);

    size_t iTotalLens(iHeadLens + iLens);
    char *pBuf = new(std::nothrow) char[iTotalLens];
    H_ASSERT(NULL != pBuf, "malloc memory error.");

    memcpy(pBuf, acHead, iHeadLens);
    if (NULL != pszMsg)
    {
        memcpy(pBuf + iHeadLens, pszMsg, iLens);
    }

    CSender::getSingletonPtr()->Send(sock, pBuf, iTotalLens, false);
}

H_ENAMSP
