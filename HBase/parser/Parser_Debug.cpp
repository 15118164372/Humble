
#include "Parser_Debug.h"
#include "document.h"
#include "Adjure_Task.h"
#include "Session.h"

H_BNAMSP

CAdjure *CDebugParser::Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    return Parse(pSession->getSock(), pSession->getType(), pBuf, iLens, iParsed, bClose);
}
CAdjure *CDebugParser::Parse(const H_SOCK &uiSock, const unsigned short &usType,
    const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
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
    objDoc.Parse(pBuf + m_uiHeadLens, (size_t)usLens);
    if (objDoc.HasParseError())
    {
        bClose = true;
        H_Printf("json Parse error: %d", objDoc.GetParseError());
        return NULL;
    }

    if (!objDoc.HasMember("cmd")
        || !objDoc.HasMember("task")
        || !objDoc.HasMember("content"))
    {
        bClose = true;
        H_Printf("%s", "loss json member.");
        return NULL;
    }

    CDebugAdjure *pDebug = new(std::nothrow) CDebugAdjure(uiSock, usType, objDoc["task"].GetString(),
        pBuf + m_uiHeadLens, (size_t)usLens);
    if (NULL == pDebug)
    {
        bClose = true;
        H_Printf("%s", H_ERR_MEMORY);
        return NULL;
    }

    return pDebug;
}

H_ENAMSP
