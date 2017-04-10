
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "NetBase.h"
#include "SVIntf.h"
#include "Singleton.h"

H_BNAMSP

//网络接收服务
class CNetWorker : public CNetBase, public CSingleton<CNetWorker>
{
public:
    CNetWorker(void);
    ~CNetWorker(void);

    void onOrder(H_Order *pOrder);
    void onStart(void);
    void onReadyStop(void);
    void onClose(H_Session *pSession);
    void onLinked(H_Session *pSession, const bool &bAccept);
    void onRead(H_Session *pSession);

    void setIntf(CSVIntf *pIntf);
    CSVIntf *getIntf(void);
    
    unsigned int addListener(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    void delListener(const unsigned int uiID);
    unsigned int addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    void delTcpLink(const unsigned int uiID);
    //udp
    H_SOCK  addUdp(const char *pszHost, const unsigned short usPort);
    void delUdp(H_SOCK sock);

    void addLinkEv(const H_SOCK &sock, const unsigned int &uiID);
    luabridge::H_LBinary *getLBinary(void)
    {
        return &m_stBinary;
    };

public:
    static void acceptCB(struct evconnlistener *, H_SOCK sock, struct sockaddr *,
        int, void *arg);
    static void monitorCB(H_SOCK, short, void *arg);  
    static void udpCB(H_SOCK sock, short sEv, void *arg);

private:
    struct event *monitorLink(struct H_TcpLink *pTcpLink);

private:
    H_DISALLOWCOPY(CNetWorker);
#ifdef H_OS_WIN 
    #define listenerit std::unordered_map<unsigned int, struct H_Listener*>::iterator
    #define listener_map std::unordered_map<unsigned int, struct H_Listener*>
    #define tcplinkit std::unordered_map<unsigned int, struct H_TcpLink*>::iterator
    #define tcplink_map std::unordered_map<unsigned int, struct H_TcpLink*>
    #define udpit std::unordered_map<H_SOCK, struct event *>::iterator
    #define udp_map std::unordered_map<H_SOCK, struct event *>
#else
    #define listenerit std::tr1::unordered_map<unsigned int,struct  H_Listener*>::iterator
    #define listener_map std::tr1::unordered_map<unsigned int, struct H_Listener*>
    #define tcplinkit std::tr1::unordered_map<unsigned int, struct H_TcpLink*>::iterator
    #define tcplink_map std::tr1::unordered_map<unsigned int, struct H_TcpLink*>
    #define udpit std::tr1::unordered_map<H_SOCK, struct event *>::iterator
    #define udp_map std::tr1::unordered_map<H_SOCK, struct event *>
#endif

private:
    unsigned int m_uiID;
    CSVIntf *m_pIntf;
    luabridge::H_LBinary m_stBinary;
    listener_map m_mapListener;
    tcplink_map m_mapTcpLink;
    udp_map m_mapUdp;
};

H_ENAMSP

#endif//H_NETWORKER_H_
