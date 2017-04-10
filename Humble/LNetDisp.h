
#ifndef H_LNETDISP_H_
#define H_LNETDISP_H_

#include "Reg2Lua.h"

H_BNAMSP

//网络服务接口实现
class CLNetDisp : public CSVIntf, public CSingleton<CLNetDisp>
{
public:
    CLNetDisp(void);
    ~CLNetDisp(void);

    void onStart(void);
    void onStop(void);
    void onTcpAccept(struct H_Session *pSession);
    void onTcpLinked(struct H_Session *pSession);
    void onTcpClose(struct H_Session *pSession);
    void onTcpRead(struct H_Session *pSession);
    void onUdpRead(H_SOCK &sock, const char *pHost, unsigned short usPort,
        const char *pBuf, const int &iLens);

private:
    H_DISALLOWCOPY(CLNetDisp);
    enum
    {
        LOnstart = 0,
        LOnStop,
        LOnTcpAccept,
        LOnTcpLinked,
        LOnTcpClose,
        LOnTcpRead,
        LOnUdpRead,

        LCount,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    CEvBuffer m_objEvBuffer;
    CBinary m_objBinary;
};

H_ENAMSP

#endif//H_LNETDISP_H_
