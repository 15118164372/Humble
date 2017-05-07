
#ifndef H_LUATASK_H_
#define H_LUATASK_H_

#include "../HBase/HBase.h"

H_BNAMSP

//lua ·þÎñ
class CLuaTask : public CWorkerTask
{
public:
    CLuaTask(const char *pszFile, const char *pszName, const int iCapacity);
    ~CLuaTask(void);

    void initTask(void);
    void runTask(void);
    void destroyTask(void);

private:
    CLuaTask(void);
    H_DISALLOWCOPY(CLuaTask);
    enum
    {
        LTASK_INIT = 0,
        LTASK_RUN,
        LTASK_DESTROY,

        LTASK_COUNT,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    std::string m_strFile;
};

H_ENAMSP

#endif //H_LUATASK_H_
