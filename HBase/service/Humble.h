
#ifndef H_HUMBLE_H_
#define H_HUMBLE_H_

#include "NetMgr.H"
#include "WorkerMgr.h"
#include "HMailer.h"
#include "HTimer.h"
#include "RPCLink.h"
#include "Parser_Default.h"
#include "Parser_Debug.h"
#include "Parser_Http.h"
#include "Parser_RPC.h"
#include "Parser_WS.h"

H_BNAMSP

//整个服务管理
class CHumble : public CObject
{
public:
    //uiInterval调整负载时间间隔
    CHumble(const unsigned short &usLogLV, const unsigned short &usRunnerNum, const unsigned short &usNetNum,
        const unsigned int &uiAlarmTime, const unsigned int &uiAdjustTime, const unsigned int &uiRPCTimeDeviation) :
        m_usRunnerNum(usRunnerNum), m_uiAdjustTime(uiAdjustTime), m_uiAlarmTime(uiAlarmTime), m_iServiceId(H_INIT_NUMBER), 
        m_iServiceType(H_INIT_NUMBER), m_objHttpdParser(H_PARSER_HTTPD), m_objHttpcParser(H_PARSER_HTTPC),
        m_objRPCParser(uiRPCTimeDeviation), m_objWSParser(H_PARSER_WS), m_objWorkerMgr(&m_objMsgTrigger),
        m_objNetMgr(&m_objMsgTrigger, &m_objWorkerMgr, &m_objParserMgr, &m_objRPCLink,usNetNum),
        m_objTimer(&m_objMsgTrigger, &m_objWorkerMgr, m_objNetMgr.getLinker())
    {
        m_strProPath = CUtils::getProPath();

        //日志
        g_pLog = new(std::nothrow) CLog();
        H_ASSERT(NULL != g_pLog, H_ERR_MEMORY);
        g_pLog->setPriority(usLogLV);

        m_objMsgTrigger.setWorkerMgr(&m_objWorkerMgr);

        //注册解析器
        m_objRPCParser.setRPCLink(&m_objRPCLink);
        m_objParserMgr.addParser(&m_objDefParser);
        m_objParserMgr.addParser(&m_objHttpdParser);
        m_objParserMgr.addParser(&m_objHttpcParser);
        m_objParserMgr.addParser(&m_objDebugParser);
        m_objParserMgr.addParser(&m_objRPCParser);
        m_objParserMgr.addParser(&m_objWSParser);
    };
    ~CHumble(void)
    {
        H_SafeDelete(g_pLog);
    };

    //启动
    void Satrt(void)
    {
        CThread::Creat(g_pLog);
        g_pLog->waitStart();
        CThread::Creat(&m_objMailer);
        m_objMailer.waitStart();
        m_objWorkerMgr.Start(m_usRunnerNum, m_uiAlarmTime, m_uiAdjustTime);
        m_objNetMgr.startNet();
        m_objTimer.Start();
    };
    //请求停止
    void Stop(void)
    {
        m_objTimer.Stop();
        m_objNetMgr.stopNet();
        m_objWorkerMgr.Stop();
        m_objMailer.Stop();
        g_pLog->Stop();
    };
    CWorker *popPool(void)
    {
        return m_objWorkerMgr.popPool();
    };

    //任务注册
    void regTask(CWorker *pWorker)
    {
        m_objWorkerMgr.regTask(pWorker);
    };
    //任务删除
    void unRegTask(const char *pszName)
    {
        H_ASSERT((strlen(pszName) < H_TASKNAMELENS), "task name too long.");

        m_objWorkerMgr.unRegTask(pszName);
    };
    CWorker *getTask(const char *pszName)
    {
        H_ASSERT((strlen(pszName) < H_TASKNAMELENS), "task name too long.");

        return m_objWorkerMgr.getWorker(pszName);
    };
    //获取所有任务名
    std::list<std::string> *getAllName(CWorker *pWorker)
    {
        m_objWorkerMgr.getAllName(pWorker->storageStr());

        return pWorker->storageStr();
    };
    //网络RPC
    void netRPC(const H_SOCK uiSock, const char *pszFrom, const char *pszTo, const char *pszContent, const size_t uiConLen)
    {
        CBuffer *pBuf(CRPCParser::creatPack(pszFrom, pszTo, pszContent, uiConLen));
        if (NULL == pBuf)
        {
            return;
        }

        m_objNetMgr.sendMsg(uiSock, pBuf);
    };
    //服务间RPC调用
    void taskRPC(CWorker *pFrom, CWorker *pTo,
        const char *pszRPCName, const char *pszRPCParam, const size_t iParamLens, const uint64_t ulId)
    {
        CRPCCallAdjure *pAdjure = new(std::nothrow) CRPCCallAdjure(pFrom, pszRPCName, pszRPCParam, iParamLens, ulId);
        if (NULL == pAdjure)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }

        m_objWorkerMgr.addAdjureToTask(pTo, pAdjure);
    };
    void taskRPCRtn(CWorker *pTo, const char *pszRPCParam, const size_t iParamLens, const uint64_t ulId)
    {
        CRPCRtnAdjure *pAdjure = new(std::nothrow) CRPCRtnAdjure(pszRPCParam, iParamLens, ulId);
        if (NULL == pAdjure)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }

        m_objWorkerMgr.addAdjureToTask(pTo, pAdjure);
    };
    //超时注册
    void regTimeOut(CWorker *pWorker, const unsigned int uiTime, const uint64_t ulId)
    {
        m_objTimer.timeOut(pWorker, uiTime, ulId);
    };
    //accept 成功
    void regAcceptEvent(CWorker *pWorker, const unsigned short usType)
    {
        m_objMsgTrigger.regAcceptEvent(pWorker, usType);
    };
    //connect成功 注册
    void regConnectEvent(CWorker *pWorker, const unsigned short usType)
    {
        m_objMsgTrigger.regConnectEvent(pWorker, usType);
    };
    //socket关闭  注册
    void regCloseEvent(CWorker *pWorker, const unsigned short usType)
    {
        m_objMsgTrigger.regCloseEvent(pWorker, usType);
    };
    //网络可读
    void regIProto(CWorker *pWorker, H_PROTO_TYPE iProto)
    {
        m_objMsgTrigger.regIProto(pWorker, iProto);
    };
    void regHttpdProto(CWorker *pWorker, const char *pszProto)
    {
        m_objMsgTrigger.regHttpdProto(pWorker, pszProto);
    };

    //发送邮件请求
    void sendMail(CMailAdjure *pMail)
    {
        m_objMailer.sendMail(pMail);
    };
    //设置日志级别
    void setLogPriority(const unsigned short usLV)
    {
        g_pLog->setPriority(usLV);
    };
    unsigned short getLogPriority(void)
    {
        return g_pLog->getPriority();
    };

    //新加监听
    void addListener(const char *pszParser, const unsigned short usType,
        const char *pszHost, const unsigned short usPort)
    {
        m_objNetMgr.addListener(pszParser, usType, pszHost, usPort);
    };
    //添加主动连接
    void addLinker(class CWorker *pWorker, const char *pszParser, const unsigned short usType,
        const char *pszHost, const unsigned short usPort, const uint64_t ulId, const bool bKeepAlive)
    {
        m_objNetMgr.addLinker(pWorker, pszParser, usType, pszHost, usPort, ulId, bKeepAlive);
    };
    //关闭连接
    void closeLink(const H_SOCK uiSock)
    {
        m_objNetMgr.closeLink(uiSock);
    };
    //绑定socket消息到任务
    void bindWorker(CWorker *pWorker, const H_SOCK uiSock)
    {
        m_objNetMgr.bindWorker(uiSock, pWorker);
    };
    void unBindWorker(const H_SOCK uiSock)
    {
        m_objNetMgr.unBindWorker(uiSock);
    };
    //发送消息
    void sendMsg(const H_SOCK uiSock, const char *pszBuf, const size_t iLens)
    {
        m_objNetMgr.sendMsg(uiSock, pszBuf, iLens);
    };
    void sendMsg2(const H_SOCK uiSock, CBuffer *pBuffer)
    {
        m_objNetMgr.sendMsg(uiSock, pBuffer);
    };
    void broadCast(std::vector<H_SOCK> &vcSocks, const char *pszBuf, const size_t &iLens)
    {
        unsigned short usNetNum(m_objNetMgr.getNetNum());
        if (1 == usNetNum)
        {
            broadCastSingle(vcSocks, pszBuf, iLens);
            return;
        }

        broadCastMulti(vcSocks, pszBuf, iLens, usNetNum);
    };
    //rpc连接
    std::list<H_SOCK> *getLinkById(CWorker *pWorker, const int iSVId)
    {
        m_objRPCLink.getLinkById(iSVId, pWorker->storageSock());

        return pWorker->storageSock();
    };
    H_SOCK getALinkById(const int iSVId)
    {
        return m_objRPCLink.getALinkById(iSVId);
    };
    std::list<H_SOCK> *getLinkByType(CWorker *pWorker, const int iSVType)
    {
        m_objRPCLink.getLinkByType(iSVType, pWorker->storageSock());

        return pWorker->storageSock();
    };
    H_SOCK getALinkByType(const int &iSVType)
    {
        return m_objRPCLink.getALinkByType(iSVType);
    };
    //一些参数
    void setServiceId(const int iId)
    {
        m_iServiceId = iId;
        m_objRPCParser.setServiceId(iId);
    };
    int getServiceId(void)
    {
        return m_iServiceId;
    };
    void setServiceType(const int iType)
    {
        m_iServiceType = iType;
        m_objRPCParser.setServiceType(iType);
    };
    int getServiceType(void)
    {
        return m_iServiceType;
    };
    const char *getProPath(void)
    {
        return m_strProPath.c_str();
    };
    void setScriptPath(const char *pPath)
    {
        m_strScriptPath = CUtils::formatStr("%s%s%s", m_strProPath.c_str(), pPath, H_PATH_SEPARATOR);
    };
    const char *getScriptPath(void)
    {
        return m_strScriptPath.c_str();
    };
    void setRPCKey(const std::string &strKey)
    {
        m_strRPCKey = strKey;
        m_objRPCParser.setKey(strKey);
    };
    const char *getRPCKey(void)
    {
        return m_strRPCKey.c_str();
    };

private:
    CHumble(void);
    void broadCastSingle(std::vector<H_SOCK> &vcSocks, const char *pszBuf, const size_t &iLens)
    {
        CBroadCastAdjure *pAdjure = new(std::nothrow) CBroadCastAdjure(pszBuf, iLens);
        if (NULL == pAdjure)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }

        *(pAdjure->getSocks()) = vcSocks;
        m_objNetMgr.broadCast(H_INIT_NUMBER, pAdjure);
    };
    void broadCastMulti(std::vector<H_SOCK> &vcSocks, const char *pszBuf, const size_t &iLens, unsigned short &usNetNum)
    {
        CBroadCastAdjure **pAdjure = new(std::nothrow) CBroadCastAdjure*[usNetNum];
        if (NULL == pAdjure)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }
        H_Zero(pAdjure, sizeof(CBroadCastAdjure *) * usNetNum);

        unsigned short usIndex(H_INIT_NUMBER);
        for (std::vector<H_SOCK>::iterator it = vcSocks.begin(); vcSocks.end() != it; ++it)
        {
            usIndex = m_objNetMgr.getIndex(*it);
            if (NULL == pAdjure[usIndex])
            {
                pAdjure[usIndex] = new(std::nothrow) CBroadCastAdjure(pszBuf, iLens);
                if (NULL == pAdjure[usIndex])
                {
                    freeBroadCastAdjure(pAdjure, usNetNum);
                    H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
                    return;
                }
            }

            pAdjure[usIndex]->getSocks()->push_back(*it);
        }

        for (unsigned short usI = H_INIT_NUMBER; usI < usNetNum; ++usI)
        {
            if (NULL == pAdjure[usI])
            {
                continue;
            }

            m_objNetMgr.broadCast(usI, pAdjure[usI]);
        }

        H_SafeDelArray(pAdjure);
    };
    void freeBroadCastAdjure(CBroadCastAdjure **pAdjure, unsigned short &usNum)
    {
        for (unsigned short usI = H_INIT_NUMBER; usI < usNum; ++usI)
        {
            H_SafeDelete(pAdjure[usI]);
        }

        H_SafeDelArray(pAdjure);
    };

private:
    unsigned short m_usRunnerNum;
    unsigned int m_uiAdjustTime;
    unsigned int m_uiAlarmTime;
    int m_iServiceId;
    int m_iServiceType;
    std::string m_strProPath;
    std::string m_strScriptPath;
    std::string m_strRPCKey;

    //解析器
    CDebugParser m_objDebugParser;
    CHttpParser m_objHttpdParser;
    CHttpParser m_objHttpcParser;
    CDefaultParser m_objDefParser;
    CRPCParser m_objRPCParser;
    CWSParser m_objWSParser;
    CParserMgr m_objParserMgr;

    CMsgTrigger m_objMsgTrigger;
    CWorkerMgr m_objWorkerMgr;
    CRPCLink m_objRPCLink;
    CNetMgr m_objNetMgr;
    CTimer m_objTimer;
    CMailer m_objMailer;
};

H_ENAMSP

#endif//H_HUMBLE_H_
