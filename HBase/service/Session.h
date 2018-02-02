
#ifndef H_SESSION_H_
#define H_SESSION_H_

#include "HObject.h"
#include "Buffer.h"
#include "event2/event.h"

H_BNAMSP

//连接信息 每个连接对应一个
class CSession : public CObject
{
public:
    CSession(class CNetWorker *pNetWork, class CParser *pParser, class CLinkInfo *pLinkInfo, H_SOCK &sock, const unsigned short &usType) :
        m_bAccept(true), m_sHSStatus(H_INVALID_STATUS), m_usType(usType), m_pParser(pParser), m_pNetWorker(pNetWork), m_pLinkInfo(pLinkInfo),
        m_pEvent(NULL), m_pTimeEvent(NULL), m_pExtendData(NULL), m_Sock(sock)
    {
        if (NULL != pLinkInfo)
        {
            m_bAccept = false;
        }
    };
    ~CSession(void)
    {
        H_SafeDelete(m_pExtendData);
    };
    //连接类型
    void setType(const unsigned short &usType)
    {
        m_usType = usType;
    };
    H_INLINE const unsigned short &getType(void)
    {
        return m_usType;
    };
    //数据解析器
    void setParser(class CParser *pParser)
    {
        m_pParser = pParser;
    };
    H_INLINE class CParser *getParser(void)
    {
        return m_pParser;
    };
    //CNetWork*
    H_INLINE class CNetWorker *getNetWorker(void)
    {
        return m_pNetWorker;
    };
    //握手状态(H_OK_STATUS 已经完成握手)
    H_INLINE short *getHSStatus(void)
    {
        return &m_sHSStatus;
    }
    void setHSStatus(short sStatus)
    {
        m_sHSStatus = sStatus;
    };
    void resetHSStatus(void)
    {
        m_sHSStatus = H_INVALID_STATUS;
    };
    //CLinkInfo *
    H_INLINE class CLinkInfo *getLinkInfo(void)
    {
        return m_pLinkInfo;
    };
    H_INLINE const bool &Accept(void)
    {
        return m_bAccept;
    };
    void setEvent(void *pEvent)
    {
        m_pEvent = pEvent;
    };
    void *getEvent(void)
    {
        return m_pEvent;
    };
    void setTimeEvent(void *pEv)
    {
        m_pTimeEvent = pEv;
    };
    void *getTimeEvent(void)
    {
        return m_pTimeEvent;
    };
    H_INLINE CDynaBuffer *getDynaBuffer(void)
    {
        return &m_stDynaBuffer;
    };
    const H_SOCK &getSock(void)
    {
        return m_Sock;
    };
    void setExtendData(CObject *pExtendData)
    {
        m_pExtendData = pExtendData;
    };
    CObject *getExtendData(void)
    {
        return m_pExtendData;
    };
   
private:
    CSession(void);
    bool m_bAccept;
    short m_sHSStatus;
    unsigned short m_usType;
    class CParser *m_pParser;
    class CNetWorker *m_pNetWorker;
    class CLinkInfo *m_pLinkInfo;
    void *m_pEvent;
    void *m_pTimeEvent;
    CObject *m_pExtendData;
    H_SOCK m_Sock;
    CDynaBuffer m_stDynaBuffer;
};

H_ENAMSP

#endif//H_SESSION_H_
