
#ifndef H_PARSER_WS_H_
#define H_PARSER_WS_H_

#include "Parser.h"
#include "Adjure_Task.h"

H_BNAMSP

//websocket
class CWSParser : public CParser
{
public:
    explicit CWSParser(const char *pszName);
    ~CWSParser(void);

    virtual bool checkOtherParam(std::map<std::string, std::string > *pHead);
    virtual std::string createHSResponse(std::string &strKey);
    CBuffer *handShake(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

    virtual CAdjure *onFullPack(class CSession *pSession, const char *pBuf, const size_t &iLens, const bool bCopy, bool &bClose);
    CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose);

private:
    #define FRAME_HEAD_BASE_LEN 6
    #define FRAME_HEAD_EXT16_LEN 8
    #define FRAME_HEAD_EXT64_LEN 14

    struct WSHSExtendData *initParser(class CSession *pSession); 
    bool checkParam(struct WSHSExtendData *pExtendData, std::string &strKey);
    std::string createKey(std::string &strKey);
    bool parseHead(const char *pBuf, const size_t &iLens, struct WebSockFrame *pFram, size_t &iParsed, bool &bCLose);
    void createHead(const unsigned short usCode, const size_t &iDataLens,
        char acHead[FRAME_HEAD_EXT64_LEN], size_t &iOutLens);
    CAdjure *Pong(void);
    CAdjure *Pack(class CSession *pSession, const char *pBuf, const size_t &iLens, const bool &bCopy);
    CAdjure *handlePack(class CSession *pSession, struct WebSockFrame *pFrame, const char *pUnMasked, bool &bClose);

private:
    void *m_pHttpParserSetting;
    std::string m_strVersion;
    CContinueAdjure m_objContinueAdjure;
};

H_ENAMSP

#endif//H_PARSER_WS_H_
