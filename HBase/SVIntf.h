
#ifndef H_SVINTF_H_
#define H_SVINTF_H_

#include "Macros.h"

H_BNAMSP

class CSVIntf
{
public:
    CSVIntf(void) {};
    virtual ~CSVIntf(void) {};

    virtual void onStart(void) {};
    virtual void onStop(void) {};
    virtual void onTcpAccept(struct H_Session *pSession) {};
    virtual void onTcpLinked(struct H_Session *pSession) {};
    virtual void onTcpClose(struct H_Session *pSession) {};
    virtual void onTcpRead(struct H_Session *pSession) {};
    virtual void onTime(const unsigned int &uiTick, const unsigned int &uiCount) {};
    virtual void onUdpRead(H_SOCK &sock, const char *pHost, unsigned short usPort,
        const char *pBuf, const int &iLens) {};
};

H_ENAMSP

#endif//H_SVINTF_H_
