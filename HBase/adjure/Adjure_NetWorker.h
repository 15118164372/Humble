
#ifndef H_ADJURE_NETWORKER_H_
#define H_ADJURE_NETWORKER_H_

#include "Adjure.h"
#include "Utils.h"

H_BNAMSP

enum
{
    NETWORKERADJ_ADDLISTENER = 0,    //��Ӽ���
    NETWORKERADJ_ADDSOCK,            //�������
    NETWORKERADJ_BINDWORKER,         //��socket������
    NETWORKERADJ_UNBINDWORKER,       //�����
    NETWORKERADJ_COPYSEND,           //����
    NETWORKERADJ_NOCOPYSEND,         //����
    NETWORKERADJ_BROADCAST,          //�㲥
    NETWORKERADJ_ADDLINKER,          //
    NETWORKERADJ_RELINK,
};

//������Ϣ
class CNetInfo : public CObject
{
public:
    CNetInfo(class CEventService *pEventService, class CParser *pParser, 
        const unsigned short &usType, const char *pszHost, const unsigned short &usPort) :
        m_usPort(usPort), m_usType(usType), m_pParser(pParser), m_pEventService(pEventService), m_strHost(pszHost)
    {
        m_bIsIpV6 = CUtils::IsIpV6(pszHost);
    };
    ~CNetInfo(void)
    {};

    //�Ƿ�ΪIpV6
    const bool &isIpV6(void)
    {
        return m_bIsIpV6;
    };
    //�󶨵�Ip
    const char *getHost(void)
    {
        return m_strHost.c_str();
    };
    //�󶨵Ķ˿�
    const unsigned short &getPort(void)
    {
        return m_usPort;
    };
    //��������
    const unsigned short &getType(void)
    {
        return m_usType;
    };
    //���ݽ�����
    class CParser *getParser(void)
    {
        return m_pParser;
    };
    void setEventService(class CEventService *pEventService)
    {
        m_pEventService = pEventService;
    };
    class CEventService *getEventService(void)
    {
        return m_pEventService;
    };

private:
    CNetInfo(void);
    bool m_bIsIpV6;
    unsigned short m_usPort;
    unsigned short m_usType;
    class CParser *m_pParser;
    class CEventService *m_pEventService;
    std::string m_strHost;
};

enum LinkState
{
    LS_WAITLINK = 0,
    LS_LINKED,
    LS_CLOSED,
};

//������Ϣ
class CLinkInfo : public CNetInfo
{
public:
    CLinkInfo(class CEventService *pEventService, class CParser *pParser, 
        const unsigned short &usType, const char *pszHost, const unsigned short &usPort) :
        CNetInfo(pEventService, pParser, usType, pszHost, usPort), m_bKeepAlive(true), 
        m_emLinkState(LS_WAITLINK), m_pBind(NULL)
    {};
    ~CLinkInfo(void)
    {};

    LinkState getLinkState(void)
    {
        return m_emLinkState;
    };
    void setLinkState(const LinkState &emState)
    {
        m_emLinkState = emState;
    };

    void setKeepAlive(const bool &bKeepAlive)
    {
        m_bKeepAlive = bKeepAlive;
    };
    bool getKeepAlive(void)
    {
        return m_bKeepAlive;
    };
    void setBind(class CWorker *pBind)
    {
        m_pBind = pBind;
    };
    class CWorker *getBind(void)
    {
        return m_pBind;
    };

private:
    CLinkInfo(void);
    bool m_bKeepAlive;
    LinkState m_emLinkState;
    class CWorker *m_pBind;
};

//NETWORKERADJ_ADDLISTENER
class CAddListenAdjure : public CAdjure
{
public:
    CAddListenAdjure(CNetInfo *pListenInfo) : CAdjure(NETWORKERADJ_ADDLISTENER),
        m_pListenInfo(pListenInfo)
    {};
    ~CAddListenAdjure(void)
    {};

    CNetInfo *getListenInfo(void)
    {
        return m_pListenInfo;
    };

private:
    CAddListenAdjure(void);
    CNetInfo *m_pListenInfo;
};

//NETWORKERADJ_ADDLINK
class CAddSockInAdjure : public CAdjure
{
public:
    CAddSockInAdjure(class CLinkInfo *pLinkInfo, class CParser *pParser, 
        const H_SOCK &sock, const unsigned short &usType) : CAdjure(NETWORKERADJ_ADDSOCK),
        m_usType(usType), m_pLinkInfo(pLinkInfo), m_pParser(pParser), m_uiSock(sock)
    {};
    ~CAddSockInAdjure(void)
    {};

    unsigned short getType(void)
    {
        return m_usType;
    };
    CLinkInfo *getLinkInfo(void)
    {
        return m_pLinkInfo;
    };
    class CParser *getParser(void)
    {
        return m_pParser;
    };
    H_SOCK getSock(void)
    {
        return m_uiSock;
    };

private:
    CAddSockInAdjure(void);
    unsigned short m_usType;
    CLinkInfo *m_pLinkInfo;
    class CParser *m_pParser;
    H_SOCK m_uiSock;
};

//NETWORKERADJ_CLOSELINK  NETWORKERADJ_UNBINDWORKER
class CToSockAdjure : public CAdjure
{
public:
    CToSockAdjure(const unsigned short usAdjure, const H_SOCK &uiSock) : CAdjure(usAdjure),
        m_uiSock(uiSock)
    {};
    ~CToSockAdjure(void)
    {};

    H_SOCK getSock(void)
    {
        return m_uiSock;
    };
private:
    CToSockAdjure(void);
    H_SOCK m_uiSock;
};

//NETWORKERADJ_BINDWORKER
class CBindToTaskAdjure : public CToSockAdjure
{
public:
    CBindToTaskAdjure(class CWorker *pWorker, const H_SOCK &uiSock) : CToSockAdjure(NETWORKERADJ_BINDWORKER, uiSock),
        m_pWorker(pWorker)
    {};
    ~CBindToTaskAdjure() 
    {};

    class CWorker *getWorker(void)
    {
        return m_pWorker;
    };

private:
    CBindToTaskAdjure(void);
    class CWorker *m_pWorker;
};

//NETWORKERADJ_COPYSEND
class CCopySendAdjure : public CToSockAdjure
{
public:
    CCopySendAdjure(const H_SOCK &uiSock, const char *pszBuf, const size_t &iLens) : CToSockAdjure(NETWORKERADJ_COPYSEND, uiSock),
        m_objPack((char*)pszBuf, iLens)
    {};
    ~CCopySendAdjure(void)
    {};

    CBuffer *getPack(void)
    {
        return &m_objPack;
    };

private:
    CCopySendAdjure(void);
    CCopyBuffer m_objPack;
};

//NETWORKERADJ_NOCOPYSEND
class CNoCopySendAdjure : public CToSockAdjure
{
public:
    CNoCopySendAdjure(const H_SOCK &uiSock, CBuffer *pBuffer) : CToSockAdjure(NETWORKERADJ_NOCOPYSEND, uiSock),
        m_pPack(pBuffer)
    {};
    ~CNoCopySendAdjure(void)
    {};

    CBuffer *getPack(void)
    {
        return m_pPack;
    };

private:
    CNoCopySendAdjure(void);
    CBuffer *m_pPack;
};

//NETWORKERADJ_BROADCAST
class CBroadCastAdjure : public CAdjure
{
public:
    CBroadCastAdjure(const char *pszBuf, const size_t &iLens) : CAdjure(NETWORKERADJ_BROADCAST),
        m_objPack(pszBuf, iLens)
    {};
    ~CBroadCastAdjure(void)
    {};

    void addSock(const H_SOCK &sock)
    {
        m_vcSocks.push_back(sock);
    };
    std::vector<H_SOCK> *getSocks(void)
    {
        return &m_vcSocks;
    };
    CBuffer *getPack(void)
    {
        return &m_objPack;
    };

private:
    CCopyBuffer m_objPack;
    std::vector<H_SOCK> m_vcSocks;
};

//NETWORKERADJ_ADDLINKER
class CAddLinkerAdjure : public CAdjure
{
public:
    CAddLinkerAdjure(CLinkInfo *pLinkInfo) : CAdjure(NETWORKERADJ_ADDLINKER), m_pLinkInfo(pLinkInfo)
    {};
    ~CAddLinkerAdjure(void)
    {};

    CLinkInfo *getLinkInfo(void)
    {
        return m_pLinkInfo;
    };

private:
    CAddLinkerAdjure(void);
    CLinkInfo *m_pLinkInfo;
};

H_ENAMSP

#endif//H_ADJURE_NETWORKER_H_
