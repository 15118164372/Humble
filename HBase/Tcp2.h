
#ifndef H_TCP2_H_
#define H_TCP2_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//unsigned int + unsigned short + data
class CTcp2 : public CParser, public CSingleton<CTcp2>
{
public:
    CTcp2(void);
    ~CTcp2(void);

    H_Binary parsePack(H_Session *, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &);
    H_Binary createPack(H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);
    void Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CTcp2);

};

H_ENAMSP

#endif //H_TCP2_H_
