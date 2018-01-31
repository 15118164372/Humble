
#ifndef H_PARSER_DEBUG_H_
#define H_PARSER_DEBUG_H_

#include "Parser.h"

H_BNAMSP

//Debug½âÎöÆ÷ 
class CDebugParser : public CParser
{
public:
    CDebugParser(void) : CParser(H_PARSER_DEBUG, false), m_uiHeadLens(sizeof(unsigned short))
    {};
    ~CDebugParser(void) 
    {};

    CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);
    CAdjure *Parse(const H_SOCK &uiSock, const unsigned short &usType,
        const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

private:
    size_t m_uiHeadLens;
};

H_ENAMSP

#endif//H_PARSER_DEBUG_H_
