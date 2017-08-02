
#include "WebSocket.h"

H_BNAMSP

SINGLETON_INIT(CWebSocket)
CWebSocket objWebSocket;

CWebSocket::CWebSocket(void)
{
    setName(H_PARSER_WS);
}

CWebSocket::~CWebSocket(void)
{
}

H_ENAMSP
