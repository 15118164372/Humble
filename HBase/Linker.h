
#ifndef H_LINKER_H_
#define H_LINKER_H_

#include "RecvTask.h"
#include "Singleton.h"

H_BNAMSP

struct H_Link
{
    unsigned short usPort;
    unsigned int uiID;
    char *pHost;
};

//创建socket连接
class CLinker : public CRecvTask<H_Link>, public CSingleton<CLinker>
{
public:
    CLinker(void);
    ~CLinker(void);

    void runTask(H_Link *pMsg);

    void addLink(const unsigned int &uiID, const char *pszHost, const unsigned short &usPort);

private:
    H_DISALLOWCOPY(CLinker);
};

H_ENAMSP

#endif//H_LINKER_H_
