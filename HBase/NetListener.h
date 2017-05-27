
#ifndef H_NETLISTENER_H_
#define H_NETLISTENER_H_

#include "NetBase.h"
#include "Singleton.h"

H_BNAMSP

//¼àÌý
class CNetListener : public CNetBase, public CSingleton<CNetListener>
{
public:
    CNetListener(void);
    ~CNetListener(void);

    size_t onOrder(CEvBuffer *pEvBuffer);
    void addListener(const char *pszParser, const unsigned short &usType, 
        const char *pszHost, const unsigned short &usPort);

    static void acceptCB(struct evconnlistener *, H_SOCK sock, struct sockaddr *, int, void *arg);
private:
    struct H_ListenAt
    {
        unsigned short usType;
        unsigned short usPort;
        char  acHost[32];
        char acParser[32];
    };

    struct H_Listener
    {
        unsigned short usType;
        struct evconnlistener *pEvListener;
        class CParser *pParser;
    };

private:
    H_DISALLOWCOPY(CNetListener);

private:
    std::vector<H_Listener *> m_vcListener;
    CAtomic m_objLock;
};

H_ENAMSP

#endif//H_NETLISTENER_H_
