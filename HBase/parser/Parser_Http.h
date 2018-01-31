
#ifndef H_PARSER_HTTP_H_
#define H_PARSER_HTTP_H_

#include "Parser.h"

H_BNAMSP

//http½âÎöÆ÷
class CHttpParser : public CParser
{
public:
    CHttpParser(const char *pszName);
    ~CHttpParser(void);

    CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

private:
    struct HttpExtendData *initParser(class CSession *pSession);
    bool parseUrl(class CTaskHttpdAdjure *pAdjure, const char *pBuf, const size_t &iLens);
    const char *getUrlTypeName(int &iType);

private:
    CHttpParser(void);
    void *m_pHttpParserSetting;
    unsigned short m_usType;
};

H_ENAMSP

#endif//H_PARSER_HTTP_H_
