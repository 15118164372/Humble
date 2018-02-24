
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
        LFUNC_TASK_INIT = 0, //�����ʼ��
        LFUNC_TASK_DEL,      //����ɾ��

        LFUNC_TIME_TIMEOUT,  //��ʱ

        LFUNC_NET_ACCEPT,    //accept�ɹ�
        LFUNC_NET_CONNECT,   //connect�ɹ�
        LFUNC_NET_CLOSE,     //���ӹر�
        LFUNC_NET_READ_I,    //����ɶ�(����������)
        LFUNC_NET_READ_HTTPD,//����ɶ�(http)

        LFUNC_NET_RPC,       //����rpc����

        LFUNC_TASK_RPCCALL,  //�����rpc����
        LFUNC_TASK_RPCRTN,   //�����rpc����

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

