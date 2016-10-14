#include "LNetDisp.h"
#include "LTick.h"

using namespace Humble;

#ifdef H_OS_WIN
#include "../vld/vld.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#ifdef H_MYSQL
#pragma comment(lib, "libmysql.lib")
#endif
#pragma comment(lib, "HBase.lib")
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
        bRtn = TRUE;
        break;

        //关闭控制台
    case CTRL_CLOSE_EVENT:
        bRtn = TRUE;
        break;

        //CTRL+BREAK 
    case CTRL_BREAK_EVENT:
        bRtn = TRUE;
        break;

        //用户退出
    case CTRL_LOGOFF_EVENT:
        bRtn = TRUE;
        break;

        //系统被关闭
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

int init(void)
{
    CLog *pLog = CLog::getSingletonPtr();
    CMail *pMail = CMail::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();

    std::string strConfFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(), "config", H_PATH_SEPARATOR, "config.ini");
    CIniFile objIni(strConfFile.c_str());

    if (objIni.haveNode("Log"))
    {
        pLog->setPriority(objIni.getIntValue("Log", "priority"));
        std::string strLogFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(), 
            "log", H_PATH_SEPARATOR, objIni.getStringValue("Log", "name"));
        pLog->setLogFile(strLogFile.c_str());
        pLog->Open();
    }
    else
    {
        H_Printf("%s", "in config.ini not find node 'Log'");
        return H_RTN_FAILE;
    }

    if (objIni.haveNode("Email"))
    {
        pMail->setServer(objIni.getStringValue("Email", "server"));
        pMail->setAuthType(objIni.getIntValue("Email", "auth"));
        pMail->setSender(objIni.getStringValue("Email", "sender"));
        pMail->setUserName(objIni.getStringValue("Email", "user"));
        pMail->setPassWord(objIni.getStringValue("Email", "psw"));
    }

    if (objIni.haveNode("Main"))
    {
        pTick->setTick(objIni.getIntValue("Main", "tick"));
        pWorker->setThreadNum(objIni.getIntValue("Main", "thread"));
        pTick->setThreadNum(pWorker->getThreadNum());
    }
    else
    {
        H_Printf("%s", "in config.ini not find node 'Main'");
        return H_RTN_FAILE;
    }

    return H_RTN_OK;
}

void initParser(void)
{
    CNetParser *pParser = CNetParser::getSingletonPtr();

    pParser->addParser(CDefParser::getSingletonPtr());
    pParser->addParser(CTcp1::getSingletonPtr());
    pParser->addParser(CTcp2::getSingletonPtr());
    pParser->addParser(CHttp::getSingletonPtr());
    pParser->addParser(CWebSock::getSingletonPtr());
    pParser->addParser(CMQTT::getSingletonPtr());
}

void runSV(void)
{
    CLog *pLog = CLog::getSingletonPtr();
    CMail *pMail = CMail::getSingletonPtr();
    CLinker *pLinker = CLinker::getSingletonPtr();
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CSender *pSender = CSender::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();

    pNet->setIntf(CLNetDisp::getSingletonPtr());
    pTick->setIntf(CLTick::getSingletonPtr());

    CThread::Creat(pLog);
    pLog->waitStart();
    if (pMail->getSetted())
    {
        CThread::Creat(pMail);
        pMail->waitStart();
    }
    CThread::Creat(pLinker);
    pLinker->waitStart();
    CThread::Creat(pNet);
    pNet->waitStart();
    CThread::Creat(pSender);
    pSender->waitStart();
    CThread::Creat(pWorker);
    pWorker->waitStart();
    CThread::Creat(pTick);
    pTick->waitStart();

    {
        H_LOG(LOGLV_INFO, "%s", "start service successfully.");
        CLckThis objLckThis(&g_objExitMu);
        pthread_cond_wait(&g_ExitCond, objLckThis.getMutex());
        H_LOG(LOGLV_INFO, "%s", "begin stop service.");
    }

    pTick->Join();
    pWorker->Join();
    pSender->Join();
    pNet->Join();
    pLinker->Join();
    if (pMail->getSetted())
    {
        pMail->Join();
    }
    H_LOG(LOGLV_INFO, "%s", "stop service successfully.");
    pLog->Join();
}

int main(int argc, char *argv[])
{
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

    if (H_RTN_OK != init())
    {
        return H_RTN_FAILE;
    }

    initParser();

    pthread_cond_init(&g_ExitCond, NULL);
    pthread_mutex_init(&g_objExitMu, NULL);

    runSV();

    pthread_mutex_destroy(&g_objExitMu);
    pthread_cond_destroy(&g_ExitCond);

    return H_RTN_OK;
}
