
#ifndef H_TCP1_H_
#define H_TCP1_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//��ͷ��websocket
/*
��Ϣ����С��126  char(��Ϣ����) + ��Ϣ
С�ڵ���65535    char(ֵ126) + unsigned short(��Ϣ����) + ��Ϣ
����65535        char(ֵ127) + unsigned int(��Ϣ����) + ��Ϣ
*/
class CTcp1 : public CParser, public CSingleton<CTcp1>
{
public:
    CTcp1(void);
    ~CTcp1(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);

private:
    H_DISALLOWCOPY(CTcp1);
    bool readHead(char *pBuffer, const size_t &iLens, size_t &iBufLens, size_t &iHeadLens);
    void creatHead(std::string *pOutBuf, const size_t &iLens);
};

H_ENAMSP

#endif //H_TCP1_H_
