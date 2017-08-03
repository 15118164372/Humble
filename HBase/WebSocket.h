
#ifndef H_WEBSOCKET_H_
#define H_WEBSOCKET_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

/*
websocket version:13 不支持分帧 
返回数据除了控制帧全为 WSOCK_BINARYFRAME = 0x02
*/
class CWebSocket : public CParser, public CSingleton<CWebSocket>
{
public:
    CWebSocket(void);
    ~CWebSocket(void);

    bool handShake(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &bCLose);
    H_Binary parsePack(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &bCLose);
    H_Binary createPack(H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);
    void Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);

private:
    #define FRAME_HEAD_BASE_LEN 6
    #define FRAME_HEAD_EXT16_LEN 8
    #define FRAME_HEAD_EXT64_LEN 14

private:
    //找key
    std::string parseKey(char *pMsg, const size_t &iLens);
    std::string createChallengeKey(std::string &strKey);
    std::string createHandshakeResponse(std::string &strKey);
    bool parseHead(char *pAllBuf, const size_t &iLens, struct WebSockFram *pFram, size_t &iParsed, bool &bCLose);
    void createHead(const unsigned short usCode, const size_t &iDataLens,
        char acHead[FRAME_HEAD_EXT64_LEN], size_t &iOutLens);

private:
    size_t m_iShakeHandsEndFlagLens;
    size_t m_iKeyLens;
    size_t m_iVesionLens;
    std::string m_strVersion;
};

H_ENAMSP

#endif //H_WEBSOCKET_H_
