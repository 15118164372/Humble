
#ifndef H_MSGTRIGGER_H_
#define H_MSGTRIGGER_H_

#include "Adjure_Task.h"
#include "RWLock.h"
#include "Worker.h"
#include "Log.h"

H_BNAMSP

//消息派发
class CMsgTrigger : public CObject
{
public:
    CMsgTrigger(void) : m_pWorkerMgr(NULL)
    {};
    ~CMsgTrigger(void) 
    {};

    void setWorkerMgr(class CWorkerMgr *pWorkerMgr)
    {
        m_pWorkerMgr = pWorkerMgr;
    };
    void removeEvent(CWorker *pWorker);

    //超时事件
    void triggerTimeOut(CWorker *pWorker, const uint64_t &ulId);
    //连接成功
    void regAcceptEvent(CWorker *pWorker, const unsigned short &usType)
    {
        regNetEvent(m_objAcceptEventLck, m_mapAcceptEvent, pWorker, usType);
        H_LOG(LOGLV_SYS, "%s register net accept event, type %d", pWorker->getName(), usType);
    };
    void triggerAccept(CWorker *pBindWorker, const H_SOCK &uiSock, const unsigned short &usType)
    {
        triggerNetEvent(pBindWorker, m_objAcceptEventLck, m_mapAcceptEvent, MSG_NET_ACCEPT, uiSock, usType);
    };
    //连接成功
    void regConnectEvent(CWorker *pWorker, const unsigned short &usType)
    {
        regNetEvent(m_objConnectEventLck, m_mapConnectEvent, pWorker, usType);
        H_LOG(LOGLV_SYS, "%s register net linked event, type %d", pWorker->getName(), usType);
    };
    void triggerConnect(CWorker *pBindWorker, const H_SOCK &uiSock, const unsigned short &usType)
    {
        triggerNetEvent(pBindWorker, m_objConnectEventLck, m_mapConnectEvent, MSG_NET_CONNECT, uiSock, usType);
    };
    //close
    void regCloseEvent(CWorker *pWorker, const unsigned short &usType)
    {
        regNetEvent(m_objCloseEventLck, m_mapCloseEvent, pWorker, usType);
        H_LOG(LOGLV_SYS, "%s register net close event, type %d", pWorker->getName(), usType);
    };
    void triggerClosed(CWorker *pBindWorker, const H_SOCK &uiSock, const unsigned short &usType)
    {
        triggerNetEvent(pBindWorker, m_objCloseEventLck, m_mapCloseEvent, MSG_NET_CLOSE, uiSock, usType);
    };
    //网络可读(数字请求码) MSG_NET_READ_I
    void regIProto(CWorker *pWorker, H_PROTO_TYPE &iProto);
    void triggerIProto(CINetReadAdjure *pAdjure);

    //网络可读(字符串请求码) MSG_NET_READ_HTTPD
    void regHttpdProto(CWorker *pWorker, const char *pszProto);
    void triggerHttpdProto(CTaskHttpdAdjure *pAdjure);

    //MSG_TASK_DEBUG
    void triggerDebug(CDebugAdjure *pAdjure);
    //MSG_NET_RPC
    void triggerNetRPC(CNetRPCAdjure *pAdjure);

private:
    struct WorkerCMP
    {
        bool operator()(CWorker *pWorker1, CWorker *pWorker2) const
        {
            return pWorker1->getStrName() == pWorker2->getStrName();
        }
    };
    struct WorkerHash
    {
        size_t operator()(CWorker *pWorker) const
        {
            return (size_t)CUtils::hashStr(pWorker->getName());
        }
    };

#ifdef H_OS_WIN
    //网络事件  MSG_NET_LINKED MSG_NET_CLOSE
    typedef std::unordered_map<CWorker *, std::vector<unsigned short>, WorkerHash, WorkerCMP>::iterator neteventit;
    typedef std::unordered_map<CWorker *, std::vector<unsigned short>, WorkerHash, WorkerCMP> netevent_map;
    //网络可读(数字请求码) MSG_NET_READ_I
    typedef std::unordered_map<H_PROTO_TYPE, CWorker *>::iterator iprotoit;
    typedef std::unordered_map<H_PROTO_TYPE, CWorker *> iproto_map;
    //网络可读(字符串请求码) MSG_NET_READ_STR
    typedef std::unordered_map<std::string, CWorker *>::iterator strprotoit;
    typedef std::unordered_map<std::string, CWorker *> strproto_map;
#else
    //网络事件  MSG_NET_LINKED MSG_NET_CLOSE
    typedef std::tr1::unordered_map<CWorker *, std::vector<unsigned short>, WorkerHash, WorkerCMP>::iterator neteventit;
    typedef std::tr1::unordered_map<CWorker *, std::vector<unsigned short>, WorkerHash, WorkerCMP> netevent_map;
    //网络可读(数字请求码) MSG_NET_READ_I
    typedef std::tr1::unordered_map<H_PROTO_TYPE, CWorker *>::iterator iprotoit;
    typedef std::tr1::unordered_map<H_PROTO_TYPE, CWorker *> iproto_map;
    //网络可读(字符串请求码) MSG_NET_READ_STR
    typedef std::tr1::unordered_map<std::string, CWorker *>::iterator strprotoit;
    typedef std::tr1::unordered_map<std::string, CWorker *> strproto_map;
#endif

    //网络事件  MSG_NET_LINKED MSG_NET_CLOSE
    void regNetEvent(CRWLock &objNetEventLck, netevent_map &mapEvent, CWorker *pWorker, const unsigned short &usType);
    void unRegNetEvent(CRWLock &objNetEventLck, netevent_map &mapEvent, CWorker *pWorker);
    void triggerNetEvent(CWorker *pBindWorker, CRWLock &objNetEventLck, netevent_map &mapEvent,
        const unsigned short usEvent, const H_SOCK &uiSock, const unsigned short &usType);
    void unRegAcceptEvent(CWorker *pWorker)
    {
        unRegNetEvent(m_objAcceptEventLck, m_mapAcceptEvent, pWorker);
    };
    void unRegConnectEvent(CWorker *pWorker)
    {
        unRegNetEvent(m_objConnectEventLck, m_mapConnectEvent, pWorker);
    };
    void unRegCloseEvent(CWorker *pWorker)
    {
        unRegNetEvent(m_objCloseEventLck, m_mapCloseEvent, pWorker);
    };

private:
    //accept成功
    netevent_map m_mapAcceptEvent;
    CRWLock m_objAcceptEventLck;
    //连接成功
    netevent_map m_mapConnectEvent;
    CRWLock m_objConnectEventLck;
    //close
    netevent_map m_mapCloseEvent;
    CRWLock m_objCloseEventLck;

    //网络可读(数字请求码) MSG_NET_READ_I
    iproto_map m_mapIProto;
    CRWLock m_objIProtoLck;

    //网络可读(字符串请求码) MSG_NET_READ_STR
    strproto_map m_mapStrProto;
    CRWLock m_objStrProtoLck;

    class CWorkerMgr *m_pWorkerMgr;
};

H_ENAMSP

#endif//H_MSGTRIGGER_H_
