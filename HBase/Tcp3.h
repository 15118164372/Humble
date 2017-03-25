
#ifndef H_TCP3_H_
#define H_TCP3_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//unsigned int + data
class CTcp3 : public CParser, public CSingleton<CTcp3>
{
public:
    CTcp3(void);
    ~CTcp3(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);

private:
    H_DISALLOWCOPY(CTcp3);

};

H_ENAMSP

#endif //H_TCP3_H_
