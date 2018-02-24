
#ifndef H_LWORKER_H_
#define H_LWORKER_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLWorker : public CWorker
{
public:
    CLWorker(CHumble *pHumble, const char *pszFile, const char *pszName, const size_t &uiCapacity);
    ~CLWorker(void);

    void reSet(const char *pszFile, const char *pszName, const size_t &uiCapacity);

    void initTask(void);
    void destroyTask(void);

    void onTimeOut(CTaskTimeOutAdjure *pTimeOut);
    void onAccepted(CTaskNetEvAdjure *pNetEv);
    void onConnected(CTaskNetEvAdjure *pNetEv);
    void onClosed(CTaskNetEvAdjure *pNetEv);
    void onNetReadI(CINetReadAdjure *pNetRead);
    void onNetReadHttpd(CTaskHttpdAdjure *pNetRead);
    void onNetRPC(CNetRPCAdjure *pRPCAdjure);
    void onRPCCall(CRPCCallAdjure *pRPCCall);
    void onRPCRtn(CRPCRtnAdjure *pRPCRtn);
    void onDebug(CDebugAdjure *pDebug);
    void onUnNorProc(CAdjure *pAdjure);

private:
    void initLua(void);
    void freeLua(void);

private:
    enum
    {
        LFUNC_TASK_INIT = 0, //任务初始化
        LFUNC_TASK_DEL,      //任务删除

        LFUNC_TIME_TIMEOUT,  //超时

        LFUNC_NET_ACCEPT,    //accept成功
        LFUNC_NET_CONNECT,   //connect成功
        LFUNC_NET_CLOSE,     //连接关闭
        LFUNC_NET_READ_I,    //网络可读(数字请求码)
        LFUNC_NET_READ_HTTPD,//网络可读(http)

        LFUNC_NET_RPC,       //网络rpc调用

        LFUNC_TASK_RPCCALL,  //任务间rpc调用
        LFUNC_TASK_RPCRTN,   //任务间rpc返回

        LFUNC_TASK_DEBUG,    //debug

        LFUNC_TASK_BIND_HTTPC,
        LFUNC_TASK_BIND_I,

        LFUNC_COUNT,
    };

    CHumble *m_pHumble;
    struct lua_State *m_pState;
    luabridge::LuaRef *m_pLFunc[LFUNC_COUNT];
    std::string m_strFile;    
};

H_ENAMSP

#endif //H_LWORKER_H_

