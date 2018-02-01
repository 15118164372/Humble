
#ifndef H_PARSER_RPC_H_
#define H_PARSER_RPC_H_

#include "Parser.h"
#include "Adjure_Task.h"

H_BNAMSP

struct SVIdType : public CObject
{
    int iId;
    int iType;
};

//rpc
class CRPCParser : public CParser
{
public:
    explicit CRPCParser(const unsigned int &uiRPCTimeDeviation) : CParser(H_PARSER_RPC, true), m_iServiceId(H_INIT_NUMBER),
        m_iServiceType(H_INIT_NUMBER), m_uiRPCTimeDeviation(uiRPCTimeDeviation), m_pRPCLink(NULL), 
        m_uiHeadLens(sizeof(unsigned short)), m_uiContentHeadLens(sizeof(RPCContentHead))
    {
        setRPC(true);
        setHSTimeOut(H_TIMEOUT_RPCHS);
    };
    ~CRPCParser(void)
    {};

    void setKey(const std::string &strKey)
    {
        m_strKey = strKey;
    };
    const char *getKey(void)
    {
        return m_strKey.c_str();
    };
    void setServiceId(const int &iId)
    {
        m_iServiceId = iId;
    };
    const int &getServiceId(void)
    {
        return m_iServiceId;
    };
    void setServiceType(const int &iType)
    {
        m_iServiceType = iType;
    };
    const int &getServiceType(void)
    {
        return m_iServiceType;
    };
    void setRPCLink(class CRPCLink *pRPCLink)
    {
        m_pRPCLink = pRPCLink;
    };

    CBuffer *handShake(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);
    CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

    CBuffer *Sign(bool &bClose);

    static CBuffer *creatPack(const char *pszFrom, const char *pszTo, 
        const char *pszContent, const size_t &uiConLen);

private:
    bool checkSign(void *pDocument, const char *pszBuf, unsigned short &usLens);

private:
    int m_iServiceId;
    int m_iServiceType;
    unsigned int m_uiRPCTimeDeviation;
    class CRPCLink *m_pRPCLink;
    size_t m_uiHeadLens;
    size_t m_uiContentHeadLens;
    std::string m_strKey;
};

H_ENAMSP

#endif//H_PARSER_RPC_H_
