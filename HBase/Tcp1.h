
#ifndef H_TCP1_H_
#define H_TCP1_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//��ͷ��websocket
/*
��Ϣ����С��126  char(��Ϣ����) + unsigned short + ��Ϣ
С�ڵ���65535    char(ֵ126) + unsigned short(��Ϣ����) + unsigned short + ��Ϣ
����65535        char(ֵ127) + unsigned int(��Ϣ����) + unsigned short + ��Ϣ
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
