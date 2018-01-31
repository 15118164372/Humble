
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "EventService.h"
#include "Parser.h"
#include "MsgTrigger.h"
#include "WorkerMgr.h"
#include "Session.h"

H_BNAMSP

//网络读写
class CNetWorker : public CEventService
{
public:
    CNetWorker(void);
    ~CNetWorker(void);

    void setMsgTrigger(CMsgTrigger *pMsgTrigger);
    CMsgTrigger *getMsgTrigger(void);
    void setWorkerMgr(CWorkerMgr *pWorkerMgr);
    CWorkerMgr *getWorkerMgr(void);
    void setNetMgr(class CNetMgr *pNetMgr);
    class CNetMgr *getNetMgr(void);

    //添加socket进循环
    void addSock(class CLinkInfo *pLinkInfo, class CParser *pParser, 
        const H_SOCK &sock, const unsigned short &usType);
    //绑定socket消息到任务
    void bindWorker(const H_SOCK &uiSock, class CWorker *pWorker);
    void unBindWorker(const H_SOCK &uiSock);

    //执行请求
    void onAdjure(CAdjure *pAdjure);
    void afterAdjure(CAdjure *pAdjure);

    void dispAdjure(CAdjure *pAdjure);
    class CWorker *getBindWorker(const H_SOCK &uiSock);
    void removeBind(const H_SOCK &uiSock);
    void setLinkStatus(class CLinkInfo *pLinkInfo, const LinkState emState);
    void handleAdjure(class CWorker *pBindWorker, CAdjure *pAdjure, const H_SOCK &uiSock);

private:
    void addSock(CAddSockInAdjure *pAdjure);
    void bindWorker(CBindToTaskAdjure *pAdjure);
    void unBindWorker(CToSockAdjure *pAdjure);
    void *handShakeMonitor(class CSession *pSession, const unsigned int &uiMs);
    CSession *newSession(class CParser *pParser, class CLinkInfo *pLinkInfo, 
        H_SOCK &sock, const unsigned short &usType);
    void *newSockEvent(class CSession *pSession, H_SOCK &sock);

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<H_SOCK, class CWorker *>::iterator bindit;
    typedef std::unordered_map<H_SOCK, class CWorker *> bind_map;
#else
    typedef std::tr1::unordered_map<H_SOCK, class CWorker *>::iterator bindit;
    typedef std::tr1::unordered_map<H_SOCK, class CWorker *> bind_map;
#endif

    int m_iNoDelay;
    CMsgTrigger *m_pMsgTrigger;
    class CWorkerMgr *m_pWorkerMgr;
    class CNetMgr *m_pNetMgr;
    bind_map m_mapBind;
};

H_ENAMSP

#endif//H_NETWORKER_H_
