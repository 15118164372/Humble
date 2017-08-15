
#ifndef H_LINKER_H_
#define H_LINKER_H_

#include "HStruct.h"
#include "TaskLazy.h"
#include "Singleton.h"

H_BNAMSP

struct H_LinkCMD
{
    char cCmd;
    bool bLinked;
    unsigned short usPort;    
    class CParser *pParser;    
    char  acHost[32]; 
    H_LINK stLink;
};

//创建socket连接
class CLinker : public CTaskLazy<H_LinkCMD>, public CSingleton<CLinker>
{
public:
    CLinker(void);
    ~CLinker(void);

    void runTask(H_LinkCMD *pMsg);
    void runSurplus(H_LinkCMD *)
    {};

    void addLink(const char *pszParser, const unsigned short &usType, const char *pszHost, const unsigned short &usPort);
    void reLink(void);
    void linkClosed(H_SOCK &sock);
    void removeLink(H_SOCK &sock);

private:
    void addLink(H_LinkCMD *pMsg);
    void reLink(H_LinkCMD *);
    void linkClosed(H_LinkCMD *pMsg);
    void removeLink(H_LinkCMD *pMsg);

private:
    enum
    {
        CMD_ADDLINK = 0,
        CMD_RELINK,
        CMD_CLOSED,
        CMD_REMOVE,
    };

private:
    H_DISALLOWCOPY(CLinker);

private:
    std::vector<H_LinkCMD *> m_vcLink;
};

H_ENAMSP

#endif//H_LINKER_H_
