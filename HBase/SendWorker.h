
#ifndef H_SENDWORKER_H_
#define H_SENDWORKER_H_

#include "HStruct.h"
#include "TaskLazy.h"

H_BNAMSP

struct H_NetMsg
{
    H_Binary stBuf;
    H_SOCK sock;
};

class CSendWorker : public CTaskLazy<H_NetMsg>
{
public:
    CSendWorker(void);
    ~CSendWorker(void);

    void runTask(H_NetMsg *pMsg);
    void addSend(H_SOCK &sock, const char *pszBuf, const size_t &iLens, const bool &bCopy);
};

H_ENAMSP

#endif//H_SENDWORKER_H_
