
#ifndef H_WEBSOCKET_H_
#define H_WEBSOCKET_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//websocket  不支持分帧
class CWebSocket : public CParser, public CSingleton<CWebSocket>
{
public:
    CWebSocket(void);
    ~CWebSocket(void);

private:

};

H_ENAMSP

#endif //H_WEBSOCKET_H_
