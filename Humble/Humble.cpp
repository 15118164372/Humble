
#include "Cmd.h"

using namespace Humble;

#ifdef H_OS_WIN
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#ifdef H_MYSQL
#pragma comment(lib, "libmysql.lib")
#endif
#pragma comment(lib, "HBase.lib")
#pragma comment(lib, "HLBind.lib")
#endif

CCoreDump g_objDump();

H_BNAMSP
void H_SetPackPath(struct lua_State *pLState)
{
    std::list<std::string> lstDirs;

    H_GetSubDirName(g_strScriptPath.c_str(), lstDirs);
    luabridge::LuaRef objPack = luabridge::getGlobal(pLState, "package");
    std::string strPack = objPack["path"];
    std::string strVal = H_FormatStr("%s?.lua", g_strScriptPath.c_str());

    std::list<std::string>::iterator itDir;
    for (itDir = lstDirs.begin(); lstDirs.end() != itDir; ++itDir)
    {
        strVal = H_FormatStr("%s;%s%s/?.lua", strVal.c_str(), g_strScriptPath.c_str(), itDir->c_str());
    }

    objPack["path"] = H_FormatStr("%s;%s", strPack.c_str(), strVal.c_str());
}

void H_RegOther(struct lua_State *pLState)
{

}
H_ENAMSP

#ifdef H_OS_WIN
BOOL WINAPI consoleHandler(DWORD msgType)
{
    BOOL bRtn = FALSE;

    switch (msgType)
    {
        //CTRL+C
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        bRtn = TRUE;
        break;

    default:
        bRtn = FALSE;
        break;
    }

    if (bRtn)
    {
        H_LOG(LOGLV_INFO, "catch console signal %d.", msgType);
        pthread_cond_broadcast(&g_ExitCond);
    }

    return bRtn;
}
#else
void sigHandEntry(int iSigNum)
{
    H_LOG(LOGLV_INFO, "catch signal %d.", iSigNum);

    CLckThis objLckThis(&g_objExitMu);
    pthread_cond_broadcast(&g_ExitCond);
}
#endif

int setParam(void)
{
    CParserMgr::getSingletonPtr()->addParser(CHttp::getSingletonPtr());
    CParserMgr::getSingletonPtr()->addParser(CTcp1::getSingletonPtr());
    CParserMgr::getSingletonPtr()->addParser(CTcp2::getSingletonPtr());

    std::string strConfFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(), "config", H_PATH_SEPARATOR, "config.ini");
    CIniFile objIni(strConfFile.c_str());
    if (!objIni.haveNode("Log"))
    {
        H_Printf("%s", "in config.ini not find node 'Log'");
        return H_RTN_FAILE;
    }
    if (!objIni.haveNode("Main"))
    {
        H_Printf("%s", "in config.ini not find node 'Main'");
        return H_RTN_FAILE;
    }

    CLog *pLog = CLog::getSingletonPtr();
    pLog->setPriority(objIni.getIntValue("Log", "priority"));
    std::string strLogFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(),
        "log", H_PATH_SEPARATOR, objIni.getStringValue("Log", "name"));
    pLog->setLogFile(strLogFile.c_str());
    pLog->Open();
    
    g_iSVId = objIni.getIntValue("Main", "id");
    unsigned short unNetNum((unsigned short)objIni.getIntValue("Main", "netnum"));
    CNetWorkerMgr::getSingletonPtr()->startWorker(unNetNum);
    CSender::getSingletonPtr()->startSender(unNetNum);
    CTick::getSingletonPtr()->setTime((unsigned int)objIni.getIntValue("Main", "tick"));
    CTaskMgr::getSingletonPtr()->setThreadNum((unsigned short)objIni.getIntValue("Main", "workernum"));

    return H_RTN_OK;
}

int initData(void)
{
    struct lua_State *pLState = luaL_newstate();
    if (NULL == pLState)
    {
        H_Printf("%s", "luaL_newstate error.");
        return H_RTN_FAILE;
    }

    luaL_openlibs(pLState);
    H_RegAll(pLState);

    std::string strLuaFile = g_strScriptPath + "start.lua";
    if (H_RTN_OK != luaL_dofile(pLState, strLuaFile.c_str()))
    {        
        H_Printf("%s", lua_tostring(pLState, -1));
        lua_close(pLState);
        return H_RTN_FAILE;
    }

    lua_close(pLState);

    return H_RTN_OK;
}

void runSV(void)
{
    CThread::Creat(CLog::getSingletonPtr());
    CLog::getSingletonPtr()->waitStart();
    CThread::Creat(CLinker::getSingletonPtr());
    CLinker::getSingletonPtr()->waitStart();
    CThread::Creat(CNetListener::getSingletonPtr());
    CNetListener::getSingletonPtr()->waitStart();
    CThread::Creat(CTick::getSingletonPtr());
    CTick::getSingletonPtr()->waitStart();
    
    if (H_RTN_OK == initData())
    {
        H_LOG(LOGLV_INFO, "%s", "start service successfully.");
        CLckThis objLckThis(&g_objExitMu);
        pthread_cond_wait(&g_ExitCond, objLckThis.getMutex());
        H_LOG(LOGLV_INFO, "%s", "begin stop service.");
    }

    CTick::getSingletonPtr()->Join();
    CNetListener::getSingletonPtr()->Join();
    CTaskMgr::getSingletonPtr()->stopWorker();
    CSender::getSingletonPtr()->stopSender();
    CNetWorkerMgr::getSingletonPtr()->stopWorker();
    CLinker::getSingletonPtr()->Join();    
    CLog::getSingletonPtr()->Join();
}

//-d 进入命令行模式 Humble -d port
int main(int argc, char *argv[])
{    
    if (cmdMode(argc, argv))
    {
        unsigned short usPort = H_ToNumber<unsigned short>((const char*)argv[2]);
        printf("Humble version %d.%d.%d, cmd port %d\n", H_MAJOR, H_MINOR, H_RELEASE, usPort);
        printf("command:\n    quit\n    exit\n    hotfix taskname(or all) filename\n    do taskname .... done\n");

        H_SOCK cmdSock = H_ClientSock("127.0.0.1", usPort);
        if (H_INVALID_SOCK == cmdSock)
        {
            return H_RTN_FAILE;
        }

        onCommand(cmdSock);

        evutil_closesocket(cmdSock);

        return H_RTN_OK;
    }

    g_strProPath = H_GetProPath();
    g_strScriptPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "script", H_PATH_SEPARATOR);

#ifdef H_OS_WIN
    (void)SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
#else
    signal(SIGPIPE, SIG_IGN);//若某一端关闭连接，而另一端仍然向它写数据，第一次写数据后会收到RST响应，此后再写数据，内核将向进程发出SIGPIPE信号
    signal(SIGINT, sigHandEntry);//终止进程
    signal(SIGHUP, sigHandEntry);//终止进程
    signal(SIGTSTP, sigHandEntry);//ctrl+Z
    signal(SIGTERM, sigHandEntry);//终止一个进程
    signal(SIGKILL, sigHandEntry);//立即结束程序
    signal(SIGABRT, sigHandEntry);//中止一个程序
    signal(H_SIGNAL_EXIT, sigHandEntry);
    H_Printf("exit service by command \"kill -%d %d\".", H_SIGNAL_EXIT, getpid());
#endif
    
    if (H_RTN_OK != setParam())
    {
        return H_RTN_FAILE;
    }

    pthread_cond_init(&g_ExitCond, NULL);
    pthread_mutex_init(&g_objExitMu, NULL);

    runSV();

    pthread_mutex_destroy(&g_objExitMu);
    pthread_cond_destroy(&g_ExitCond);

    return H_RTN_OK;
}
