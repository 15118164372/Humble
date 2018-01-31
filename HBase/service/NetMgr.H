
#ifndef H_NETMGR_H_
#define H_NETMGR_H_

#include "NetWork.h"
#include "Sender.h"
#include "Linker.h"
#include "NetListener.h"
#include "HThread.h"

H_BNAMSP

//网络管理
class CNetMgr : public CObject
{
public:
    CNetMgr(CMsgTrigger *pMsgTrigger, CWorkerMgr *pWorkerMgr, CParserMgr *pParserMgr, class CRPCLink *pRPCLink, const unsigned short &usNetNum) :
        m_usNetNum(usNetNum), m_pRPCLink(pRPCLink),  m_objListener(pParserMgr, this), m_objLinker(pParserMgr, this)
    {
        m_pAllSender = new(std::nothrow) CSender[m_usNetNum];
        H_ASSERT(NULL != m_pAllSender, H_ERR_MEMORY);

        m_pAllNetWorker = new(std::nothrow) CNetWorker[m_usNetNum];
        H_ASSERT(NULL != m_pAllNetWorker, H_ERR_MEMORY);

        for (unsigned short usI = 0; usI < m_usNetNum; ++usI)
        {
            m_pAllNetWorker[usI].setNetMgr(this);
            m_pAllNetWorker[usI].setMsgTrigger(pMsgTrigger);
            m_pAllNetWorker[usI].setWorkerMgr(pWorkerMgr);
        }
    };
    ~CNetMgr(void)
    {
        H_SafeDelArray(m_pAllNetWorker);
        H_SafeDelArray(m_pAllSender);
    };

    //启动网络
    void startNet(void)
    {
        CThread::Creat(&m_objLinker);
        m_objLinker.waitStart();

        for (unsigned short usI = 0; usI < m_usNetNum; ++usI)
        {
            CThread::Creat(&m_pAllNetWorker[usI]);
            m_pAllNetWorker[usI].waitStart();

            CThread::Creat(&m_pAllSender[usI]);
            m_pAllSender[usI].waitStart();
        }

        CThread::Creat(&m_objListener);
        m_objListener.waitStart();
    };

    //停止网络
    void stopNet(void)
    {
        m_objListener.Stop();

        for (unsigned short usI = 0; usI < m_usNetNum; ++usI)
        {
            m_pAllNetWorker[usI].Stop();
            m_pAllSender[usI].Stop();
        }

        m_objLinker.Stop();
    };

    //新加监听
    void addListener(const char *pszParser, const unsigned short &usType,
        const char *pszHost, const unsigned short &usPort)
    {
        m_objListener.addListener(pszParser, usType, pszHost, usPort);
    };
    //新加连接
    void addLinker(class CWorker *pWorker, const char *pszParser, const unsigned short &usType,
        const char *pszHost, const unsigned short &usPort, const bool &bKeepAlive)
    {
        m_objLinker.addLinker(pWorker, pszParser, usType, pszHost, usPort, bKeepAlive);
    };
    //关闭连接
    void closeLink(const H_SOCK &uiSock)
    {
        CUtils::shutDown(uiSock);
    };
    //绑定socket消息到任务
    void bindWorker(const H_SOCK &uiSock, class CWorker *pWorker)
    {
        getNetWorker(uiSock)->bindWorker(uiSock, pWorker);
    };
    void unBindWorker(const H_SOCK &uiSock)
    {
        getNetWorker(uiSock)->unBindWorker(uiSock);
    };
    //发送消息
    void sendMsg(const H_SOCK &uiSock, const char *pszBuf, const size_t &iLens)
    {
        m_pAllSender[getIndex(uiSock)].Send(uiSock, pszBuf, iLens);
    };
    void sendMsg(const H_SOCK &uiSock, CBuffer *pBuffer)
    {
        if (NULL == pBuffer)
        {
            return;
        }

        m_pAllSender[getIndex(uiSock)].Send(uiSock, pBuffer);
    };
    void broadCast(const unsigned short &usIndex, CBroadCastAdjure *pAdjure)
    {
        m_pAllSender[usIndex].broadCast(pAdjure);
    };
    CNetWorker *getNetWorker(const H_SOCK &uiSock)
    {
        return &m_pAllNetWorker[getIndex(uiSock)];
    };
    CLinker *getLinker(void)
    {
        return &m_objLinker;
    };
    class CRPCLink *getRPCLink(void)
    {
        return m_pRPCLink;
    };
    H_INLINE unsigned short getIndex(const H_SOCK &uiSock)
    {
        return (1 == m_usNetNum ? H_INIT_NUMBER : CUtils::hashNumber(uiSock) % m_usNetNum);
    };
    const unsigned short &getNetNum(void)
    {
        return m_usNetNum;
    };

private:
    CNetMgr(void);
    unsigned short m_usNetNum;
    CNetWorker *m_pAllNetWorker;
    CSender *m_pAllSender;
    class CRPCLink *m_pRPCLink;
    CNetListener m_objListener;
    CLinker m_objLinker;
};

H_ENAMSP

#endif//H_NETMGR_H_
