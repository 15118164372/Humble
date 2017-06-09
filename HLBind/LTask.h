
#ifndef H_LTASK_H_
#define H_LTASK_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLTask : public CTaskWorker
{
public:
    CLTask(const char *pszFile, const char *pszName, const int iCapacity);
    ~CLTask(void);

    void initTask(void);
    void destroyTask(void);
    void onAccept(H_LINK *pLink);
    void onLinked(H_LINK *pLink);
    void onClosed(H_LINK *pLink);
    void onNetRead(H_TCPBUF *pTcpBuf);
    void onFrame(H_TICK *pTick);
    void onSec(H_TICK *pTick);
    const char *onCMD(const char *pszCmd, const char *pszInfo, size_t &iOutLens);
    const char *onRPCCall(H_LINK *pLink, const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens);
    void onRPCRtn(const unsigned int &uiId, H_Binary *pBinary);
    const char *onTaskRPCCall(const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens);
    void onTaskRPCRtn(const unsigned int &uiId, H_Binary *pBinary);

private:
    CLTask(void);
    H_DISALLOWCOPY(CLTask);

    enum
    {
        LFUNC_INITTASK = 0,
        LFUNC_DELTASK,
        LFUNC_ONNETEVENT,
        LFUNC_ONNETREAD,
        LFUNC_ONTIME,
        LFUNC_ONCMD,
        LFUNC_RPCCALL,
        LFUNC_RPCRTN,

        LFUNC_COUNT,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    std::string m_strFile;
    H_Binary m_stBinary;
    H_LSTATE m_stState;
    H_CURLINK m_curRPCLink;
};

H_ENAMSP

#endif//H_LTASK_H_