
#include "Parser_Http.h"
#include "Session.h"
#include "Log.h"
#include "Adjure_NetWorker.h"
#include "Adjure_Task.h"
#include "http-parser/http_parser.h"

H_BNAMSP

struct HttpExtendData : public CObject
{
    bool bOver;
    unsigned short usType;
    CSession *pSession;
    CHttpAdjure *pAdjure;
    char *pUrl;
    size_t uiUrlLens;
    size_t uiOffset;
    http_parser stParser;
    std::string strHeaderField;
    HttpExtendData(void) : bOver(false), usType(H_INIT_NUMBER), pSession(NULL), pAdjure(NULL), pUrl(NULL),
        uiUrlLens(H_INIT_NUMBER), uiOffset(H_INIT_NUMBER)
    {};
    ~HttpExtendData(void)
    {
        H_SafeDelete(pAdjure);
    };
    void Clean(void)
    {
        bOver = false;
        pUrl = NULL;
        uiUrlLens = H_INIT_NUMBER;
        uiOffset = H_INIT_NUMBER;
        strHeaderField.clear();
    };
};

static int onBeginCB(http_parser *pParser)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->Clean();
    if (HTTP_REQUEST == pExtendData->usType)
    {
        pExtendData->pAdjure = new(std::nothrow) CTaskHttpdAdjure(pExtendData->pSession->getSock(), pExtendData->pSession->getType());
        if (NULL == pExtendData->pAdjure)
        {
            return H_RTN_ERROR;
        }

        return H_RTN_OK;
    }
    
    pExtendData->pAdjure = new(std::nothrow) CTaskHttcdAdjure(pExtendData->pSession->getSock(), pExtendData->pSession->getType());
    if (NULL == pExtendData->pAdjure)
    {
        return H_RTN_ERROR;
    }
    pExtendData->pAdjure->setBindId(pExtendData->pSession->getLinkInfo()->getBindId());

    return H_RTN_OK;
}
static int onUrlCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->pUrl = (char*)pszAt;
    pExtendData->uiUrlLens = iLens;
    return H_RTN_OK;
}
static int onResStatus(http_parser *pParser, const char *pszAt, size_t iLens)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    ((CTaskHttcdAdjure *)(pExtendData->pAdjure))->setStatus(pszAt, iLens);
    return H_RTN_OK;
}
static int onHeaderFieldCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->strHeaderField.assign(pszAt, iLens);
    return H_RTN_OK;
}
static int onHeaderValueCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->pAdjure->addHead(pExtendData->strHeaderField, pszAt, iLens);
    return H_RTN_OK;
}
static int onHeadersCompleteCB(http_parser *pParser)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    if (HTTP_REQUEST == pExtendData->usType)
    {
        ((CTaskHttpdAdjure *)(pExtendData->pAdjure))->setMethod(http_method_str((http_method)pParser->method));
    }    
    return H_RTN_OK;
}
static int onBodyCB(http_parser *pParser, const char *pszAt, size_t iLens)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->pAdjure->addBody(pszAt, iLens);
    return H_RTN_OK;
}
static int onCompleteCB(http_parser *pParser)
{
    HttpExtendData *pExtendData((HttpExtendData *)pParser->data);
    pExtendData->bOver = true;
    return H_RTN_OK;
}

CHttpParser::CHttpParser(const char *pszName) : CParser(pszName, false), m_pHttpParserSetting(NULL)
{
    m_usType = (0 == strcmp(pszName, H_PARSER_HTTPD) ? HTTP_REQUEST : HTTP_RESPONSE);
    m_pHttpParserSetting = new(std::nothrow) http_parser_settings;
    H_ASSERT(NULL != m_pHttpParserSetting, H_ERR_MEMORY);

    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    pSetting->on_message_begin = onBeginCB;
    pSetting->on_url = onUrlCB;
    pSetting->on_status = onResStatus;
    pSetting->on_header_field = onHeaderFieldCB;
    pSetting->on_header_value = onHeaderValueCB;
    pSetting->on_headers_complete = onHeadersCompleteCB;
    pSetting->on_body = onBodyCB;
    pSetting->on_message_complete = onCompleteCB;
}
CHttpParser::~CHttpParser(void)
{
    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    H_SafeDelete(pSetting);
    m_pHttpParserSetting = NULL;
}

struct HttpExtendData *CHttpParser::initParser(class CSession *pSession)
{
    HttpExtendData *pExtendData((HttpExtendData *)pSession->getExtendData());
    if (NULL == pExtendData)
    {
        pExtendData = new(std::nothrow) HttpExtendData;
        if (NULL == pExtendData)
        {
            return NULL;
        }

        http_parser_init(&pExtendData->stParser, (http_parser_type)m_usType);
        pExtendData->stParser.data = pExtendData;
        pExtendData->pSession = pSession;
        pExtendData->usType = m_usType;
        pSession->setExtendData(pExtendData);
    }

    return pExtendData;
}
const char *CHttpParser::getUrlTypeName(int &iType)
{
    switch (iType)
    {
        case UF_SCHEMA:
            return "Schema";
        case UF_HOST:
            return "Host";
        case UF_PORT:
            return "Port";
        case UF_PATH:
            return H_HTTP_PROTOKEY;
        case UF_QUERY:
            return "Query";
        case UF_FRAGMENT:
            return "Fragment";
        case UF_USERINFO:
            return "UserInfo";
        default:
            break;
    }

    return NULL;
}
bool CHttpParser::parseUrl(class CTaskHttpdAdjure *pAdjure, const char *pBuf, const size_t &iLens)
{
    http_parser_url stUrl;
    http_parser_url_init(&stUrl);
    if (HPE_OK != http_parser_parse_url(pBuf, iLens, 0, &stUrl))
    {
        return false;
    }

    for (int i = 0; i < UF_MAX; ++i)
    {
        if (H_INIT_NUMBER == stUrl.field_data[i].len)
        {
            continue;
        }
        const char *pszName(getUrlTypeName(i));
        if (NULL == pszName)
        {
            continue;
        }

        pAdjure->addUrlInfo(pszName, pBuf + stUrl.field_data[i].off, stUrl.field_data[i].len);
    }
    
    return true;
}
CAdjure *CHttpParser::Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    HttpExtendData *pExtendData = initParser(pSession);
    if (NULL == pExtendData)
    {
        bClose = true;
        return NULL;
    }  

    http_parser_settings *pSetting((http_parser_settings *)m_pHttpParserSetting);
    size_t uiNParsed(http_parser_execute(&(pExtendData->stParser), pSetting, 
        pBuf + pExtendData->uiOffset, iLens - pExtendData->uiOffset));
    if (HPE_OK != pExtendData->stParser.http_errno)
    {
        bClose = true;
        return NULL;
    }

    pExtendData->uiOffset += uiNParsed;
    if (!pExtendData->bOver)
    {
        return NULL;
    }

    if (HTTP_REQUEST == pExtendData->usType)
    {
        CTaskHttpdAdjure *pAdjure((CTaskHttpdAdjure *)pExtendData->pAdjure);
        if (NULL != pExtendData->pUrl
            && H_INIT_NUMBER != pExtendData->uiUrlLens)
        {
            if (!parseUrl(pAdjure, pExtendData->pUrl, pExtendData->uiUrlLens))
            {
                bClose = true;
                return NULL;
            }
        }
    }

    CAdjure *pRtn(pExtendData->pAdjure);
    pExtendData->pAdjure = NULL;
    iParsed = pExtendData->uiOffset;

    return pRtn;
}

H_ENAMSP
