
#ifndef H_STRUCT_H_
#define H_STRUCT_H_

#include "Macros.h"

H_BNAMSP

struct H_Binary
{
    char *pBufer;
    size_t iLens;
    H_Binary(void) : pBufer(NULL), iLens(H_INIT_NUMBER)
    {}
};

struct H_LINK
{
    unsigned short usType;
    H_SOCK sock;
};

struct H_Session
{
    bool bReLink;
    struct bufferevent *pEv;
    class CNetWorker *pNetWorker;
    class CParser *pParser;
    H_LINK stLink;
};

struct H_TICK
{
    unsigned int uiMS;
    unsigned int uiCount;
};

struct H_TCPBUF
{
    H_PROTOTYPE iProto;
    H_Binary stBinary;
    H_LINK stLink;
};

struct H_CMD
{
    char acCommand[16];
    char acTask[H_TASKNAMELENS];
    char acMsg[H_ONEK];
};

struct H_RPC
{
    unsigned int uiId;
    unsigned int uiMsgLens;
    char acRPC[H_RPCNAMELENS];
    char acToTask[H_TASKNAMELENS];
    char acSrcTask[H_TASKNAMELENS];
};

struct H_RPCRTN
{
    unsigned int uiId;
    unsigned int uiMsgLens;
    char acToTask[H_TASKNAMELENS];
};

struct H_MSG
{
    unsigned short usEnevt;
    char *pEvent;
};

struct H_LSTATE
{
    void *pLState;
};

struct H_CURLINK
{
    H_SOCK sock;
    H_SOCK getSock(void)
    {
        return sock;
    };
};

H_ENAMSP

#endif//H_STRUCT_H_
