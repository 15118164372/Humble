
#include "Parser_RPC.h"
#include "EncryptUtils.h"
#include "Utils.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "RPCLink.h"
#include "Session.h"
#include "Log.h"

H_BNAMSP

bool CRPCParser::checkSign(void *pDocument, const char *pszBuf, unsigned short &usLens)
{
    rapidjson::Document *pDoc((rapidjson::Document*)pDocument);
    pDoc->Parse(pszBuf, (size_t)usLens);
    if (pDoc->HasParseError())
    {
        H_LOG(LOGLV_WARN, "json Parse error: %d", pDoc->GetParseError());
        return false;
    }
    //检查节点是否全
    if (!pDoc->HasMember("svid")
        || !pDoc->HasMember("svtype")
        || !pDoc->HasMember("tms")
        || !pDoc->HasMember("sign"))
    {
        H_LOG(LOGLV_WARN, "%s", "loss json member.");
        return false;
    }
    //检查时间差
    int64_t ilCur((int64_t)CUtils::nowMilSecond());
    int64_t ilTMS((*pDoc)["tms"].GetInt64());
    int64_t iDif(abs(ilCur - ilTMS));
    if (iDif > m_uiRPCTimeDeviation)
    {
        H_LOG(LOGLV_WARN, "timestamp error. now %s message tms %s",
            CUtils::toString(ilCur).c_str(), CUtils::toString(ilTMS).c_str());
        return false;
    }
    //检查签名
    std::string strSign(CUtils::formatStr("%d%d%s%s",
        (*pDoc)["svid"].GetInt(),
        (*pDoc)["svtype"].GetInt(),
        CUtils::toString(ilTMS).c_str(),
        m_strKey.c_str()));
    if (CEnUtils::md5Str(strSign.c_str(), strSign.size()) != (*pDoc)["sign"].GetString())
    {
        H_LOG(LOGLV_WARN, "%s", "sign error.");
        return false;
    }

    return true;
}
CBuffer *CRPCParser::Sign(bool &bClose)
{
    uint64_t ulTms(CUtils::nowMilSecond());
    std::string strSign(CUtils::formatStr("%d%d%s%s",
        m_iServiceId,
        m_iServiceType,
        CUtils::toString(ulTms).c_str(),
        m_strKey.c_str()));

    rapidjson::StringBuffer objBuf;
    rapidjson::Writer<rapidjson::StringBuffer> objWriter(objBuf);
    objWriter.StartObject();
    objWriter.Key("svid");
    objWriter.Int(m_iServiceId);
    objWriter.Key("svtype");
    objWriter.Int(m_iServiceType);
    objWriter.Key("tms");
    objWriter.Int64((int64_t)ulTms);
    objWriter.Key("sign");
    objWriter.String(CEnUtils::md5Str(strSign.c_str(), strSign.size()).c_str());
    objWriter.EndObject();

    const char *pszJson = objBuf.GetString();
    size_t uiJsonLens(strlen(pszJson));
    size_t iTotal(uiJsonLens + m_uiHeadLens);
    char *pBuf = new(std::nothrow) char[iTotal];
    if (NULL == pBuf)
    {
        bClose = true;
        H_LOG(LOGLV_WARN, "%s", H_ERR_MEMORY);
        return NULL;
    }

    unsigned short usHead(ntohs((u_short)uiJsonLens));
    memcpy(pBuf, &usHead, m_uiHeadLens);
    memcpy(pBuf + m_uiHeadLens, pszJson, uiJsonLens);
    
    CBuffer *pBuffer = new(std::nothrow) CBuffer(pBuf, iTotal);
    if (NULL == pBuffer)
    {
        bClose = true;
        H_SafeDelArray(pBuf);
        H_LOG(LOGLV_WARN, "%s", H_ERR_MEMORY);
        return NULL;
    }

    return pBuffer;
}
CBuffer *CRPCParser::handShake(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    if (m_uiHeadLens > iLens)
    {
        return NULL;
    }
    unsigned short usLens(ntohs(*((unsigned short*)pBuf)));
    if (H_INIT_NUMBER == usLens)
    {
        bClose = true;
        return NULL;
    }
    if (m_uiHeadLens + usLens > iLens)
    {
        return NULL;
    }

    iParsed = m_uiHeadLens + usLens;
    rapidjson::Document objDoc;
    if (!checkSign(&objDoc, pBuf + m_uiHeadLens, usLens))
    {
        bClose = true;
        return NULL;
    }

    SVIdType *pExtendData = new(std::nothrow) SVIdType;
    if (NULL == pExtendData)
    {
        bClose = true;
        H_LOG(LOGLV_WARN, "%s", H_ERR_MEMORY);
        return NULL;
    }

    //握手成功
    pSession->setHSStatus(H_OK_STATUS);
    pExtendData->iId = objDoc["svid"].GetInt();
    pExtendData->iType = objDoc["svtype"].GetInt();
    H_SOCK sock(pSession->getSock());

    m_pRPCLink->Register(pExtendData->iId, pExtendData->iType, sock);
    pSession->setExtendData(pExtendData);

    H_LOG(LOGLV_SYS, "register rpc link, id %d, type %d, sock %d", 
        pExtendData->iId, pExtendData->iType, sock);

    if (!pSession->Accept())
    {
        return NULL;
    }

    return Sign(bClose);
}

CAdjure *CRPCParser::Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    if (m_uiContentHeadLens > iLens)
    {
        return NULL;
    }
    RPCContentHead *pHead((RPCContentHead *)pBuf);
    size_t uiConLens((size_t)ntohl((u_long)pHead->uiLens));
    size_t uiTotal(uiConLens + m_uiContentHeadLens);
    if (uiTotal > iLens)
    {
        return NULL;
    }

    iParsed = uiTotal;
    CNetRPCAdjure *pAdjure = new(std::nothrow) CNetRPCAdjure(pSession->getSock(), pSession->getType());
    if (NULL == pAdjure)
    {
        bClose = true;
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    memcpy(pAdjure->getHead(), pBuf, m_uiContentHeadLens);
    pAdjure->getHead()->uiLens = uiConLens;
    pAdjure->setContent(pBuf + m_uiContentHeadLens, uiConLens);

    return pAdjure;
}

CBuffer *CRPCParser::creatPack(const char *pszFrom, const char *pszTo,
    const char *pszContent, const size_t &uiConLen)
{
    H_ASSERT((strlen(pszFrom) < H_TASKNAMELENS) && (strlen(pszTo) < H_TASKNAMELENS), "task name too long.");

    RPCContentHead stHead;
    H_Zero(&stHead, sizeof(stHead));
    stHead.uiLens = (size_t)ntohl((u_long)uiConLen);
    strcpy(stHead.acFrom, pszFrom);
    strcpy(stHead.acTo, pszTo);

    size_t uiTotal(uiConLen + sizeof(stHead));
    char *pBuf = new(std::nothrow) char[uiTotal];
    if (NULL == pBuf)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    memcpy(pBuf, &stHead, sizeof(stHead));
    memcpy(pBuf + sizeof(stHead), pszContent, uiConLen);
    CBuffer *pBuffer = new(std::nothrow) CBuffer(pBuf, uiTotal);
    if (NULL == pBuffer)
    {
        H_SafeDelArray(pBuf);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return NULL;
    }

    return pBuffer;
}

H_ENAMSP
