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

int init(void)
{
    CLog *pLog = CLog::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();

    std::string strConfFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(), "config", H_PATH_SEPARATOR, "config.ini");
    CIniFile objIni(strConfFile.c_str());

    if (!objIni.haveNode("Log"))
    {
        H_Printf("%s", "in config.ini not find node 'Log'");
        return H_RTN_FAILE;        
    }

    pLog->setPriority(objIni.getIntValue("Log", "priority"));
    std::string strLogFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(),
        "log", H_PATH_SEPARATOR, objIni.getStringValue("Log", "name"));
    pLog->setLogFile(strLogFile.c_str());
    pLog->Open();

    if (!objIni.haveNode("Main"))
    {
        H_Printf("%s", "in config.ini not find node 'Main'");
        return H_RTN_FAILE;        
    }
    
    pTick->setTick(objIni.getIntValue("Main", "tick"));
    pWorker->setThreadNum(objIni.getIntValue("Main", "thread"));
    pTick->setThreadNum(pWorker->getThreadNum());
    g_strSVId = objIni.getStringValue("Main", "id");

    return H_RTN_OK;
}

void initParser(void)
{
    CNetParser *pParser = CNetParser::getSingletonPtr();

    pParser->addParser(CDefParser::getSingletonPtr());
    pParser->addParser(CTcp1::getSingletonPtr());
    pParser->addParser(CTcp2::getSingletonPtr());
    pParser->addParser(CTcp3::getSingletonPtr());
    pParser->addParser(CHttp::getSingletonPtr());
    pParser->addParser(CWebSock::getSingletonPtr());
    pParser->addParser(CMQTT::getSingletonPtr());
}

void runSV(void)
{
    CLog *pLog = CLog::getSingletonPtr();
    CLinker *pLinker = CLinker::getSingletonPtr();
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CSender *pSender = CSender::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();

    pNet->setIntf(CLNetDisp::getSingletonPtr());
    pTick->setIntf(CLTick::getSingletonPtr());

    CThread::Creat(pLog);
    pLog->waitStart();
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
    H_LOG(LOGLV_INFO, "%s", "stop service successfully.");
    pLog->Join();
}

int runCommand(H_SOCK &sock, const char *pszCommand, const char *pszMode, const char *pszMsg)
{
    CClock objClock;
    CBinary objBinary1;
    objBinary1.setString(pszCommand);
    objBinary1.setString(pszMode);
    objBinary1.setString(pszMsg);

    std::string strBuf = objBinary1.getWritedBuf();
    CBinary objBinary2;
    objBinary2.setUint32((unsigned int)strBuf.size());
    objBinary2.setByte(strBuf.c_str(), (const unsigned int)strBuf.size());

    strBuf = objBinary2.getWritedBuf();
    objClock.reStart();
    int iRtn = send(sock, strBuf.c_str(), (int)strBuf.size(), 0);
    if (0 >= iRtn)
    {
        return H_RTN_FAILE;
    }    
    
    const int iHeadLens = sizeof(unsigned int);
    char acHead[iHeadLens] = {0};
    iRtn = recv(sock, acHead, sizeof(acHead), 0);
    if (iHeadLens != iRtn)
    {
        return H_RTN_FAILE;
    }
    unsigned int uiPackLens = ntohl(*(unsigned int*)acHead);
    char *pBuf = new char[uiPackLens + 1];
    H_Zero(pBuf, uiPackLens + 1);
    iRtn = recv(sock, pBuf, uiPackLens, 0);
    if (iRtn != (int)uiPackLens)
    {
        H_SafeDelArray(pBuf);
        return H_RTN_FAILE;
    }

    printf("==>use time: %sms  %s\n", H_ToString(objClock.Elapsed()).c_str(), pBuf);
    H_SafeDelArray(pBuf);
    
    return H_RTN_OK;
}

void onCommand(H_SOCK &cmdSock)
{
    char acCMD[H_ONEK];
    std::string strCmd;
    std::string strTask;
    bool bRunLua = false;

    while (true)
    {
        H_Zero(acCMD, sizeof(acCMD));
        std::cin.getline(acCMD, sizeof(acCMD) - 1);
        std::string strInput = H_Trim(std::string(acCMD));
        if (strInput.empty()
            || "quit" == strInput)
        {
            break;
        }

        //lua命令输入
        if (bRunLua)
        {
            if ("exit" == strInput)
            {
                bRunLua = false;
                printf("==>end input lua code.\n");
                continue;
            }

            if ("done" == strInput)
            {
                bRunLua = false;
                printf("==>end input lua code.\n");
                if (H_RTN_OK != runCommand(cmdSock, "do", strTask.c_str(), strCmd.c_str()))
                {
                    printf("==>connect closed.\n");
                    break;
                }
                continue;
            }

            strCmd += strInput + "\n";
            continue;
        }

        std::list<std::string> lstCmd;
        std::list<std::string>::iterator itCmd;
        //命令解析 去空
        H_Split(strInput, " ", lstCmd);
        for (itCmd = lstCmd.begin(); lstCmd.end() != itCmd;)
        {
            if (itCmd->empty())
            {
                itCmd = lstCmd.erase(itCmd);
            }
            else
            {
                itCmd++;
            }
        }

        itCmd = lstCmd.begin();
        //热更新
        if ("hotfix" == *itCmd)
        {
            if (lstCmd.size() != 3)
            {
                printf("==>command error.\n");
                continue;
            }

            itCmd++;
            strTask = *itCmd;
            itCmd++;
            std::string strFile = *itCmd;
            if (H_RTN_OK != runCommand(cmdSock, "hotfix", strTask.c_str(), strFile.c_str()))
            {
                printf("==>connect closed.\n");
                break;
            }
            continue;
        }

        //进入lua命令输入
        if ("do" == *itCmd)
        {
            if (lstCmd.size() != 2)
            {
                printf("==>command error.\n");
                continue;
            }

            strCmd.clear();
            itCmd++;
            strTask = *itCmd;
            bRunLua = true;
            printf("==>begin input lua code.\n");
            continue;
        }
        printf("==>unknown command.\n");
    }
}

//-d 进入命令行模式 Humble -d 15001
int main(int argc, char *argv[])
{
    //命令行
    bool bCmdMode = false;
    if (3 == argc)
    {
        if (0 == strcmp(argv[1], "-d"))
        {
            bCmdMode = true;
        }
    }
    if (bCmdMode)
    {
        unsigned short usPort = H_ToNumber<unsigned short>((const char*)argv[2]);
        printf("Humble version %d.%d.%d, cmd port %d\n", H_MAJOR, H_MINOR, H_RELEASE, usPort);
        printf("command:\n    quit\n    exit\n    hotfix taskname(or all) filename\n    do taskname .... done\n");

        const char *pszHost = "127.0.0.1";
        H_SOCK cmdSock = H_ClientSock(pszHost, usPort);
        if (H_INVALID_SOCK == cmdSock)
        {
            return H_RTN_FAILE;
        }

        onCommand(cmdSock);

        evutil_closesocket(cmdSock);

        return 0;
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
