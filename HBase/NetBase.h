
#ifndef H_EVBASE_H_
#define H_EVBASE_H_

#include "Task.h"
#include "Atomic.h"
#include "Singleton.h"

H_BNAMSP

//准备停止
enum
{
    RSTOP_NONE = 0,//初始状态
    RSTOP_RUN,//准备执行
    RSTOP_RAN,//已经执行
};

enum
{
    ORDER_RSTOP = 0,
    ORDER_STOP,    
    ORDER_CLOSESOCK,
    ORDER_CLOSEBYTYPE,

    ORDER_ADDLISTENER,
    ORDER_DELLISTENER,
    ORDER_ADDLINK,
    ORDER_ADDLINKEV,
    ORDER_DELLINK,

    ORDER_ADDUDP,
    ORDER_DELUDP,
};

struct H_Session
{
    bool bLinker;
    unsigned short usSockType;
    unsigned int uiSession;
    unsigned int uiStatus;
    struct bufferevent *pBev;
    class CNetBase *pNetBase;
    class CParser *pParser;
    H_SOCK sock;
    H_Session(void) : bLinker(false), usSockType(H_INIT_NUMBER), uiSession(H_INIT_NUMBER),
        uiStatus(H_INIT_NUMBER), pBev(NULL), pNetBase(NULL), pParser(NULL), sock(H_INVALID_SOCK)
    { };
};

struct H_Order
{
    unsigned short usCmd;
    unsigned short usSockType;
    unsigned short usPort;
    unsigned int uiSession;
    H_SOCK sock;
    char acHost[H_IPLENS];
};

//libevent 网络封装
class CNetBase : public CTask
{
public:
    CNetBase(void);
    ~CNetBase(void);

    virtual void onOrder(H_Order *pOrder) {};
    virtual void onStart(void) {};
    virtual void onReadyStop(void) {};
    virtual void onClose(H_Session *pSession) {};
    virtual void onLinked(H_Session *pSession, const bool &bAccept) {};
    virtual void onRead(H_Session *pSession) {};

    void Run(void);
    void waitStart(void);
    void readyStop(void);
    void Join(void);

    void closeSock(const H_SOCK sock, const unsigned int uiSession);
    void closeByType(const unsigned short usSockType);

    struct event_base *getBase(void)
    {
        return m_pBase;
    };
    H_Session *addTcpEv(H_SOCK &sock, const unsigned short &usSockType, const bool bAccept);

    void setReadyStop(const unsigned int uiStatus);
    unsigned int getReadyStop(void);
    H_Session *getSession(const H_SOCK &sock);
    void delSession(H_SOCK &sock);
    void addSession(H_SOCK &sock, H_Session *pSesson, const bool &bAccept);
    void removByType(const unsigned short &usSockType);    

protected:
    void sendOrder(const void *pBuf, const size_t iLens);

public:
    static void orderReadCB(struct bufferevent *bev, void *arg);
    static void tcpReadCB(struct bufferevent *bev, void *arg);
    static void tcpEventCB(struct bufferevent *bev, short, void *arg);

private:
    H_DISALLOWCOPY(CNetBase);
    void freeSession(H_Session *pSession);    

private:
#ifdef H_OS_WIN 
    #define sessionit std::unordered_map<H_SOCK, H_Session *>::iterator
    #define session_map std::unordered_map<H_SOCK, H_Session *>
#else
    #define sessionit std::tr1::unordered_map<H_SOCK, H_Session *>::iterator
    #define session_map std::tr1::unordered_map<H_SOCK, H_Session *>
#endif

private:
    unsigned int m_uiCount;
    unsigned int m_uiReadyStop;
    unsigned int m_uiSession;
    struct bufferevent *m_pOrderBev;
    struct event_base *m_pBase;    
    H_SOCK m_sockOrder[2];
    CAtomic m_objOrderLock;
    session_map m_mapSession;
};

H_ENAMSP

#endif //H_EVBASE_H_
