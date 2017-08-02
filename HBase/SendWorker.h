
#ifndef H_SENDWORKER_H_
#define H_SENDWORKER_H_

#include "HStruct.h"
#include "TaskLazy.h"

H_BNAMSP

struct H_NetSendMsg
{
    H_Binary stBuf;
    unsigned int *pRef;
    H_SOCK sock;
    H_NetSendMsg(void) : pRef(NULL), sock(H_INVALID_SOCK)
    {};
};

class CSendWorker : public CTaskLazy<H_NetSendMsg>
{
public:
    CSendWorker(void);
    ~CSendWorker(void);

    void runTask(H_NetSendMsg *pMsg);
    void addSend(H_SOCK &sock, const char *pszBuf, const size_t &iLens, const bool &bCopy);
    bool broadCast(H_SOCK &sock, unsigned int *pRef, const char *pszBuf, const size_t &iLens);
};

H_ENAMSP

#endif//H_SENDWORKER_H_
