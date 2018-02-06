
#ifndef H_ADJURE_TASK_H_
#define H_ADJURE_TASK_H_

#include "Adjure.h"
#include "HEnum.h"

H_BNAMSP

#define H_HTTP_PROTOKEY "Path"

struct RPCContentHead
{
    unsigned int uiLens;
    char acFrom[H_TASKNAMELENS];
    char acTo[H_TASKNAMELENS];
};

//任务初始化
class CTaskInitAdjure : public CAdjure
{
public:
    CTaskInitAdjure(void) : CAdjure(MSG_TASK_INIT)
    {};
    ~CTaskInitAdjure(void)
    {};
};

//任务删除
class CTaskDelAdjure : public CAdjure
{
public:
    CTaskDelAdjure(void) : CAdjure(MSG_TASK_DEL)
    {};
    ~CTaskDelAdjure(void)
    {};
};

//超时
class CTaskTimeOutAdjure : public CAdjure
{
public:
    explicit CTaskTimeOutAdjure(const uint64_t &ulId) : CAdjure(MSG_TIME_TIMEOUT), m_ulId(ulId)
    {};
    ~CTaskTimeOutAdjure(void)
    {};

    const uint64_t &getId(void)
    {
        return m_ulId;
    };

private:
    CTaskTimeOutAdjure(void);
    uint64_t m_ulId;
};

//MSG_NET_READ_RTNBUF
class CRtnBufAdjure : public CAdjure
{
public:
    explicit CRtnBufAdjure(CBuffer *pBuffer) : CAdjure(MSG_NET_READ_RTNBUF), m_pBuffer(pBuffer)
    {};
    ~CRtnBufAdjure(void) 
    {};

    CBuffer *getPack(void)
    {
        return m_pBuffer;
    };

private:
    CRtnBufAdjure(void);
    CBuffer *m_pBuffer;
};

//MSG_NET_READ_CONTINUE
class CContinueAdjure : public CAdjure
{
public:
    CContinueAdjure(void) : CAdjure(MSG_NET_READ_CONTINUE)
    {};
    ~CContinueAdjure(void)
    {};
};

//socket事件
class CTaskNetEvAdjure : public CAdjure
{
public:
    CTaskNetEvAdjure(const unsigned short &usEvent, const H_SOCK &uiSock, const unsigned short &usType) :
        CAdjure(usEvent), m_usType(usType), m_uiSock(uiSock), m_ulId(H_INIT_NUMBER)
    {};
    ~CTaskNetEvAdjure(void)
    {};

    const H_SOCK &getSock(void)
    {
        return m_uiSock;
    };
    const unsigned short &getType(void)
    {
        return m_usType;
    };
    void setBindId(const uint64_t &ulId)
    {
        m_ulId = ulId;
    };
    const uint64_t &getBindId(void)
    {
        return m_ulId;
    };

private:
    CTaskNetEvAdjure(void);
    unsigned short m_usType;
    H_SOCK m_uiSock;
    uint64_t m_ulId;
};

//网络可读(数字请求码)
class CINetReadAdjure : public CTaskNetEvAdjure
{
public:
    CINetReadAdjure(const H_SOCK &uiSock, const unsigned short &usType, H_PROTO_TYPE &iProto) :
        CTaskNetEvAdjure(MSG_NET_READ_I, uiSock, usType), m_iProto(iProto)
    {};
    ~CINetReadAdjure(void)
    {};

    const H_PROTO_TYPE &getProto(void)
    {
        return m_iProto;
    };
    virtual CBuffer *getPack(void)
    {
        return NULL;
    };

private:
    CINetReadAdjure(void);
    H_PROTO_TYPE m_iProto;
};

class CCopyINetReadAdjure : public CINetReadAdjure
{
public:
    CCopyINetReadAdjure(const H_SOCK &uiSock, const unsigned short &usType,
        H_PROTO_TYPE &iProto, const char *pBuf, const size_t &iLens) :
        CINetReadAdjure(uiSock, usType, iProto), m_objPack(pBuf, iLens)
    {};
    ~CCopyINetReadAdjure(void)
    {};

    CBuffer *getPack(void)
    {
        return &m_objPack;
    };

private:
    CCopyINetReadAdjure(void);
    CCopyBuffer m_objPack;
};

class CNoCopyINetReadAdjure : public CINetReadAdjure
{
public:
    CNoCopyINetReadAdjure(const H_SOCK &uiSock, const unsigned short &usType,
        H_PROTO_TYPE &iProto, const char *pBuf, const size_t &iLens) :
        CINetReadAdjure(uiSock, usType, iProto), m_objPack(pBuf, iLens, sizeof(H_PROTO_TYPE))
    {};
    ~CNoCopyINetReadAdjure(void)
    {};

    CBuffer *getPack(void)
    {
        return &m_objPack;
    };

private:
    CNoCopyINetReadAdjure(void);
    CSkippedBuffer m_objPack;
};

class CHttpAdjure : public CTaskNetEvAdjure
{
public:
    CHttpAdjure(const unsigned short &usEvent, const H_SOCK &uiSock, const unsigned short &usType) :
        CTaskNetEvAdjure(usEvent, uiSock, usType)
    {};
    ~CHttpAdjure(void)
    {};

    void addHead(std::string &strHead, const char *pszAt, const size_t &iLens)
    {
        m_mapHead[strHead] = std::string(pszAt, iLens);
    };
    std::map<std::string, std::string > *getHead(void)
    {
        return &m_mapHead;
    };    
    void addBody(const char *pszAt, const size_t &iLens)
    {
        m_strBody.append(pszAt, iLens);
    };
    std::string *getBody(void)
    {
        return &m_strBody;
    };
  
private:
    CHttpAdjure(void);
    std::string m_strBody;    
    std::map<std::string, std::string > m_mapHead;
};
//MSG_NET_READ_HTTPD 网络可读(http服务端)
class CTaskHttpdAdjure : public CHttpAdjure
{
public:
    CTaskHttpdAdjure(const H_SOCK &uiSock, const unsigned short &usType) : CHttpAdjure(MSG_NET_READ_HTTPD, uiSock, usType)
    {};
    ~CTaskHttpdAdjure(void)
    {};

    void addUrlInfo(const char *pszType, const char *pszAt, const size_t &iLens)
    {
        m_mapUrl[pszType] = std::string(pszAt, iLens);
        if (0 == strcmp(pszType, H_HTTP_PROTOKEY))
        {
            m_strPath.assign(pszAt, iLens);
        }
    };
    std::map<std::string, std::string > *getUrl(void)
    {
        return &m_mapUrl;
    };
    const char *getPath()
    {
        return m_strPath.c_str();
    };
    void setMethod(const char *pszMethod)
    {
        m_strMethod = pszMethod;
    };
    const char *getMethod(void)
    {
        return m_strMethod.c_str();
    };

private:
    std::string m_strPath;
    std::string m_strMethod;
    std::map<std::string, std::string > m_mapUrl;
};

//MSG_NET_READ_HTTPC
class CTaskHttcdAdjure : public CHttpAdjure
{
public:
    CTaskHttcdAdjure(const H_SOCK &uiSock, const unsigned short &usType) : 
        CHttpAdjure(MSG_NET_READ_HTTPC, uiSock, usType)
    {};
    ~CTaskHttcdAdjure(void)
    {};

    void setStatus(const char *pszAt, const size_t &iLens)
    {
        m_strStatus.assign(pszAt, iLens);
    };
    const char *getStatus(void)
    {
        return m_strStatus.c_str();
    };

private:
    std::string m_strStatus;
};

//MSG_TASK_DEBUG
class CDebugAdjure : public CTaskNetEvAdjure
{
public:
    CDebugAdjure(const H_SOCK &uiSock, const unsigned short &usType, const char *pszTo, const char *pszDebug, const size_t &iLens) : 
        CTaskNetEvAdjure(MSG_TASK_DEBUG, uiSock, usType), m_strToTask(pszTo), m_strDebug(pszDebug, iLens)
    {};
    ~CDebugAdjure(void)
    {};

    const std::string &getToTask(void)
    {
        return m_strToTask;
    };
    const std::string &getDebug(void)
    {
        return m_strDebug;
    };

private:
    CDebugAdjure(void);
    std::string m_strToTask;
    std::string m_strDebug;
};

class CRPCData : public CAdjure
{
public:
    CRPCData(const unsigned short &usEvent, const char *pszMsg, const size_t &iLens, const uint64_t &ulId) : 
        CAdjure(usEvent), m_ulId(ulId), m_objBuffer((char*)pszMsg, iLens)
    {};
    ~CRPCData(void)
    {};

    CBuffer *getPack(void)
    {
        return &m_objBuffer;
    };
    const uint64_t &getId(void)
    {
        return m_ulId;
    };

private:
    CRPCData(void);
    uint64_t m_ulId;
    CCopyBuffer m_objBuffer;
};

//MSG_TASK_RPCCALL,  任务间rpc调用
class CRPCCallAdjure : public CRPCData
{
public:
    CRPCCallAdjure(class CWorker *pFrom, const char *pszRPCName, const char *pszMsg, const size_t &iLens, const uint64_t &ulId) :
        CRPCData(MSG_TASK_RPCCALL, pszMsg, iLens, ulId), m_pFrom(pFrom), m_strRPCName(pszRPCName)
    {};
    ~CRPCCallAdjure(void)
    {};

    class CWorker *getFrom(void)
    {
        return m_pFrom;
    };
    const char *getRPCName(void)
    {
        return m_strRPCName.c_str();
    };

private:
    class CWorker *m_pFrom;
    std::string m_strRPCName;
};

//MSG_TASK_RPCRTN,   任务间rpc返回
class CRPCRtnAdjure : public CRPCData
{
public:
    CRPCRtnAdjure(const char *pszMsg, const size_t &iLens, const uint64_t &ulId) :
        CRPCData(MSG_TASK_RPCRTN, pszMsg, iLens, ulId)
    {};
    ~CRPCRtnAdjure(void)
    {};
};

//MSG_NET_RPC 网络rpc调用
class CNetRPCAdjure : public CTaskNetEvAdjure
{
public:
    CNetRPCAdjure(const H_SOCK &uiSock, const unsigned short &usType) : 
        CTaskNetEvAdjure(MSG_NET_RPC, uiSock, usType)
    {};
    ~CNetRPCAdjure(void)
    {};

    RPCContentHead *getHead(void)
    {
        return &m_stHead;
    };
    void setContent(const char *pszContent, size_t &uiLens)
    {
        m_strContent.append(pszContent, uiLens);
    };
    const std::string *getContent(void)
    {
        return &m_strContent;
    };

private:
    CNetRPCAdjure(void);
    RPCContentHead m_stHead;
    std::string m_strContent;
};

H_ENAMSP

#endif//H_ADJURE_TASK_H_
