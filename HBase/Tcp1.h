
#ifndef H_TCP1_H_
#define H_TCP1_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//包头类websocket
/*
消息长度小于126  char(消息长度) + unsigned short + 消息
小于等于65535    char(值126) + unsigned short(消息长度) + unsigned short + 消息
大于65535        char(值127) + unsigned int(消息长度) + unsigned short + 消息
*/
class CTcp1 : public CParser, public CSingleton<CTcp1>
{
public:
    CTcp1(void);
    ~CTcp1(void);

    H_Binary parsePack(char *pAllBuf, const size_t &iLens, size_t &iParsed);
    H_Binary createPack(H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);
    void Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CTcp1);
    bool readHead(char *pBuffer, const size_t &iLens, size_t &iBufLens, size_t &iHeadLens);
    size_t getHeadLens(const size_t &iLens);
};

H_ENAMSP

#endif //H_TCP1_H_
