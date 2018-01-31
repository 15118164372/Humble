
#ifndef H_PARSER_DEFAULT_H_
#define H_PARSER_DEFAULT_H_

#include "Parser.h"

H_BNAMSP

//tcp默认连接解析 H_HEAD_TYPE + H_PROTO_TYPE + Message    H_HEAD_TYPE = sizeof(H_PROTO_TYPE) + Message Len
class CDefaultParser : public CParser
{
public:
    CDefaultParser(void) : CParser(H_PARSER_DEFAULT, false), m_uiHeadLens(sizeof(H_HEAD_TYPE) + sizeof(H_PROTO_TYPE))
    {};
    ~CDefaultParser(void) {};

    CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

private:
    size_t m_uiHeadLens;
};

H_ENAMSP

#endif//H_PARSER_DEFAULT_H_
