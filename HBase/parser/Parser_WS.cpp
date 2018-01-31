
#include "Parser_WS.h"
#include "Session.h"
#include "Log.h"
#include "SHA1.h"
#include "EncryptUtils.h"
#include "Utils.h"
#include "http-parser/http_parser.h"

H_BNAMSP

#define WebSocket_Version  13

#define PAYLOADLENS_125 125
#define PAYLOADLENS_126 126
#define PAYLOADLENS_127 127

struct WSHSExtendData : public CObject
{
    bool bOver;
    CSession *pSession;
    size_t uiOffset;
    std::string strHeaderField;
    http_parser stParser;
    std::map<std::string, std::string > mapHead;
    void Clean(void)
    {
        bOver = false;
        uiOffset = H_INIT_NUMBER;
        strHeaderField.clear();
        mapHead.clear();
    };
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

struct WebSockFrame
{
    char cFin;
    char cRsv1;
    char cRsv2;
    char cRsv3;    
    char cMask;
    unsigned char ucPayloadLen;
    WebSockOpCode emOpCode;
    size_t uiDataLens;
    char acMaskKey[4];
    WebSockFrame(void) : cFin(H_INIT_NUMBER), cRsv1(H_INIT_NUMBER),
        cRsv2(H_INIT_NUMBER), cRsv3(H_INIT_NUMBER), cMask(H_INIT_NUMBER),
        ucPayloadLen(H_INIT_NUMBER), emOpCode(WSOCK_CONTINUATION), uiDataLens(H_INIT_NUMBER)
    {};
};

static int onBeginCB(http_parser *pParser)
{
    WSHSExtendData *pExtendData((WSHSExtendData *)pParser->data);
    pExtendData->Clean();
    return H_RTN_OK;
}
static int onUrlCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    return H_RTN_OK;
}
static int onHeaderFieldCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    WSHSExtendData *pExtendData((WSHSExtendData *)pParser->data);
    pExtendData->strHeaderField.assign(pszAt, iLens);
    return H_RTN_OK;
}
static int onHeaderValueCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    WSHSExtendData *pExtendData((WSHSExtendData *)pParser->data);
    pExtendData->mapHead[pExtendData->strHeaderField] = std::string(pszAt, iLens);
    return H_RTN_OK;
}
static int onHeadersCompleteCB(http_parser *pParser)
{
    return H_RTN_OK;
}
static int onCompleteCB(http_parser *pParser)
{
    WSHSExtendData *pExtendData((WSHSExtendData *)pParser->data);
    pExtendData->bOver = true;
    return H_RTN_OK;
}

CWSParser::CWSParser(const char *pszName) : CParser(pszName, true)
{
    m_pHttpParserSetting = new(std::nothrow) http_parser_settings;
    H_ASSERT(NULL != m_pHttpParserSetting, H_ERR_MEMORY);

    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    pSetting->on_message_begin = onBeginCB;
    pSetting->on_url = onUrlCB;
    pSetting->on_header_field = onHeaderFieldCB;
    pSetting->on_header_value = onHeaderValueCB;
    pSetting->on_headers_complete = onHeadersCompleteCB;
    pSetting->on_message_complete = onCompleteCB;

    m_strVersion = CUtils::formatStr("%d-%d-%d", H_MAJOR, H_MINOR, H_RELEASE);
    setHSTimeOut(H_TIMEOUT_WSHS);
};
CWSParser::~CWSParser(void)
{
    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    H_SafeDelete(pSetting);
    m_pHttpParserSetting = NULL;
}

struct WSHSExtendData *CWSParser::initParser(class CSession *pSession)
{
    WSHSExtendData *pExtendData((WSHSExtendData *)pSession->getExtendData());
    if (NULL == pExtendData)
    {
        pExtendData = new(std::nothrow) WSHSExtendData;
        if (NULL == pExtendData)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return NULL;
        }

        http_parser_init(&pExtendData->stParser, HTTP_REQUEST);
        pExtendData->Clean();
        pExtendData->stParser.data = pExtendData;
        pExtendData->pSession = pSession;
        pSession->setExtendData(pExtendData);
    }

    return pExtendData;
}
bool CWSParser::checkOtherParam(std::map<std::string, std::string > *pHead)
{
    return true;
}
bool CWSParser::checkParam(struct WSHSExtendData *pExtendData, std::string &strKey)
{
    std::map<std::string, std::string >::iterator it;
    it = pExtendData->mapHead.find("Connection");
    if (pExtendData->mapHead.end() == it)
    {
        return false;
    }
    if (it->second != "Upgrade")
    {
        return false;
    }

    it = pExtendData->mapHead.find("Upgrade");
    if (pExtendData->mapHead.end() == it)
    {
        return false;
    }
    if (it->second != "websocket")
    {
        return false;
    }

    it = pExtendData->mapHead.find("Sec-WebSocket-Version");
    if (pExtendData->mapHead.end() == it)
    {
        return false;
    }
    if (WebSocket_Version != atoi(it->second.c_str()))
    {
        return false;
    }

    std::string *pKey(NULL);
    it = pExtendData->mapHead.find("Sec-WebSocket-Key");
    if (pExtendData->mapHead.end() == it)
    {
        return false;
    }
    pKey = &(it->second);
    if (pKey->empty())
    {
        return false;
    }

    strKey = *pKey;

    return checkOtherParam(&pExtendData->mapHead);
}
std::string CWSParser::createKey(std::string &strKey)
{
    unsigned char acShaKey[20];
    strKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    CSHA1 objSha1;
    objSha1.Update((const char *)strKey.c_str(), (unsigned int)strKey.size());
    objSha1.Final();
    (void)objSha1.GetHash(acShaKey);

    return CEnUtils::b64Encode((const char*)acShaKey, sizeof(acShaKey));
}
std::string CWSParser::createHSResponse(std::string &strKey)
{
    const char *pszFormt("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nServer: Humble version: %s\r\nSec-WebSocket-Accept: %s\r\nSec-WebSocket-Origin: null\r\n\r\n");
   
    std::string strRtn(CUtils::formatStr(pszFormt, m_strVersion.c_str(), strKey.c_str()));

    return strRtn;
}
CBuffer *CWSParser::handShake(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    WSHSExtendData *pExtendData(initParser(pSession));
    if (NULL == pExtendData)
    {
        bClose = true;
        return NULL;
    }

    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    int inParsed(http_parser_execute(&(pExtendData->stParser), pSetting,
        pBuf + pExtendData->uiOffset, iLens - pExtendData->uiOffset));
    if (HPE_OK != pExtendData->stParser.http_errno)
    {
        bClose = true;
        return NULL;
    }

    pExtendData->uiOffset += inParsed;
    if (!pExtendData->bOver)
    {
        return NULL;
    }

    std::string strKey;
    if (!checkParam(pExtendData, strKey))
    {
        bClose = true;
        return NULL;
    }

    iParsed = pExtendData->uiOffset;
    strKey = createKey(strKey);
    std::string strRtn(createHSResponse(strKey));
    CBuffer *pRtnBuf = new(std::nothrow) CCopyBuffer(strRtn.c_str(), strRtn.size());
    if (NULL == pRtnBuf)
    {
        bClose = true;
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    pSession->setExtendData(NULL);
    pSession->setHSStatus(H_OK_STATUS);
    H_SafeDelete(pExtendData);

    return pRtnBuf;
}

bool CWSParser::parseHead(const char *pBuf, const size_t &iLens, struct WebSockFrame *pFram, size_t &iParsed, bool &bCLose)
{
    if (FRAME_HEAD_BASE_LEN > iLens)
    {
        return false;
    }

    pFram->cFin = (pBuf[0] & 0x80) >> 7;
    pFram->cRsv1 = (pBuf[0] & 0x40) >> 6;
    pFram->cRsv2 = (pBuf[0] & 0x20) >> 5;
    pFram->cRsv3 = (pBuf[0] & 0x10) >> 4;
    pFram->emOpCode = (WebSockOpCode)(pBuf[0] & 0xF);
    pFram->cMask = (pBuf[1] & 0x80) >> 7;
    pFram->ucPayloadLen = pBuf[1] & 0x7F;
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
        pFram->acMaskKey[0] = pBuf[2];
        pFram->acMaskKey[1] = pBuf[3];
        pFram->acMaskKey[2] = pBuf[4];
        pFram->acMaskKey[3] = pBuf[5];
        pFram->uiDataLens = pFram->ucPayloadLen;
    }
    else if (PAYLOADLENS_126 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT16_LEN)
        {
            return false;
        }

        iParsed = FRAME_HEAD_EXT16_LEN;
        pFram->acMaskKey[0] = pBuf[4];
        pFram->acMaskKey[1] = pBuf[5];
        pFram->acMaskKey[2] = pBuf[6];
        pFram->acMaskKey[3] = pBuf[7];
        pFram->uiDataLens = htons(*(u_short *)(pBuf + 2));
    }
    else if (PAYLOADLENS_127 == pFram->ucPayloadLen)
    {
        if (iLens < FRAME_HEAD_EXT64_LEN)
        {
            return false;
        }

        iParsed = FRAME_HEAD_EXT64_LEN;
        pFram->acMaskKey[0] = pBuf[10];
        pFram->acMaskKey[1] = pBuf[11];
        pFram->acMaskKey[2] = pBuf[12];
        pFram->acMaskKey[3] = pBuf[13];
        pFram->uiDataLens = (size_t)CUtils::ntohl64(*(uint64_t *)(pBuf + 2));
    }
    else
    {
        bCLose = true;
        return false;
    }

    return true;
}
void CWSParser::createHead(const unsigned short usCode, const size_t &iDataLens,
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
        uint64_t uiLens64 = CUtils::ntohl64((uint64_t)iDataLens);
        memcpy(acHead + 2, &uiLens64, sizeof(uiLens64));

        iOutLens = FRAME_HEAD_EXT64_LEN - 4;
    }
}
CAdjure *CWSParser::Pong(void)
{
    char *pPingBuf = new(std::nothrow) char[FRAME_HEAD_EXT64_LEN];
    if (NULL == pPingBuf)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    size_t iPingLens(H_INIT_NUMBER);
    createHead(WSOCK_PONG, H_INIT_NUMBER, pPingBuf, iPingLens);
    CBuffer *pObjBuf = new(std::nothrow) CBuffer(pPingBuf, iPingLens);
    if (NULL == pObjBuf)
    {
        H_SafeDelArray(pPingBuf);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    CRtnBufAdjure *pAdjure = new(std::nothrow) CRtnBufAdjure(pObjBuf);
    if (NULL == pAdjure)
    {
        H_SafeDelArray(pPingBuf);
        H_SafeDelete(pObjBuf);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    return pAdjure;
}
CAdjure *CWSParser::Pack(class CSession *pSession, const char *pBuf, const size_t &iLens, const bool &bCopy)
{
    H_PROTO_TYPE iProto(H_PROTO_NTOH(*((H_PROTO_TYPE*)pBuf)));
    size_t iMsgLens(iLens - sizeof(H_PROTO_TYPE));
    if (H_INIT_NUMBER == iMsgLens)
    {
        return new(std::nothrow) CCopyINetReadAdjure(pSession->getSock(), pSession->getType(),
            iProto, NULL, iMsgLens);
    }

    if (bCopy)
    {
        return new(std::nothrow) CCopyINetReadAdjure(pSession->getSock(), pSession->getType(),
            iProto, pBuf + sizeof(H_PROTO_TYPE), iMsgLens);
    }

    return new(std::nothrow) CNoCopyINetReadAdjure(pSession->getSock(), pSession->getType(),
        iProto, pBuf + sizeof(H_PROTO_TYPE), iMsgLens);
}
CAdjure *CWSParser::onFullPack(class CSession *pSession, const char *pBuf, const size_t &iLens, const bool bCopy, bool &bClose)
{
    if (sizeof(H_PROTO_TYPE) > iLens)
    {
        bClose = true;
        return NULL;
    }

    CAdjure *pAdjure(Pack(pSession, pBuf, iLens, bCopy));
    if (NULL == pAdjure)
    {
        bClose = true;
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
    }

    return pAdjure;
}
CAdjure *CWSParser::handlePack(class CSession *pSession, struct WebSockFrame *pFrame, const char *pUnMasked, bool &bClose)
{
    //完整的包
    if (H_INIT_NUMBER != pFrame->cFin
        && WSOCK_CONTINUATION != pFrame->emOpCode)
    {
        return onFullPack(pSession, pUnMasked, pFrame->uiDataLens, true, bClose);
    }

    CDynaBuffer *pExtendData((CDynaBuffer *)pSession->getExtendData());
    //起始帧（FIN为0，opcode非0)  TODO 测试
    if (H_INIT_NUMBER == pFrame->cFin
        && WSOCK_CONTINUATION != pFrame->emOpCode)
    {
        if (NULL == pExtendData)
        {
            pExtendData = new(std::nothrow) CDynaBuffer;
            if (NULL == pExtendData)
            {
                bClose = true;
                H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
                return NULL;
            }
            pSession->setExtendData(pExtendData);
        }

        if (!pExtendData->Assign(pUnMasked, pFrame->uiDataLens))
        {
            bClose = true;
            return NULL;
        }

        return &m_objContinueAdjure;
    }
    //中间帧（FIN为0，opcode为0）
    if (H_INIT_NUMBER == pFrame->cFin
        && WSOCK_CONTINUATION == pFrame->emOpCode)
    {
        if (!pExtendData->Append(pUnMasked, pFrame->uiDataLens))
        {
            bClose = true;
            return NULL;
        }

        return &m_objContinueAdjure;
    }
    //结束帧（FIN为1，opcode为0）
    if (H_INIT_NUMBER != pFrame->cFin
        && WSOCK_CONTINUATION == pFrame->emOpCode)
    {
        if (!pExtendData->Append(pUnMasked, pFrame->uiDataLens))
        {
            bClose = true;
            return NULL;
        }

        CAdjure *pAdjure(onFullPack(pSession, pExtendData->getBuffer(), pExtendData->getLens(), false, bClose));
        if (NULL != pAdjure)
        {
            pExtendData->Reset();
        }

        return pAdjure;
    }

    bClose = true;
    return NULL;
}
CAdjure *CWSParser::Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    size_t iHeadLens(H_INIT_NUMBER);
    WebSockFrame stFrame;
    bool bOk(parseHead(pBuf, iLens, &stFrame, iHeadLens, bClose));
    if (bClose || !bOk)
    {
        return NULL;
    }
    if (stFrame.uiDataLens + iHeadLens > iLens)
    {
        return NULL;
    }
    
    iParsed = stFrame.uiDataLens + iHeadLens;

    //控制帧
    switch (stFrame.emOpCode)
    {
        case WSOCK_CLOSE:
        {
            bClose = true;
            return NULL;
        }
        case WSOCK_PING:
        {
            CAdjure *pAdjure(Pong());
            if (NULL == pAdjure)
            {
                bClose = true;
            }

            return pAdjure;
        }
        default:
            break;
    }

    //解码
    char *pUnMasked((char*)pBuf + iHeadLens);
    if (stFrame.uiDataLens > H_INIT_NUMBER)
    {
        for (size_t i = 0; i < stFrame.uiDataLens; ++i)
        {
            pUnMasked[i] = pUnMasked[i] ^ stFrame.acMaskKey[i % 4];
        }
    }

    return handlePack(pSession, &stFrame, pUnMasked, bClose);
}

H_ENAMSP
