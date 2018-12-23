
#include "LWorker.h"

using namespace Humble;

CHumble *g_pHumble;
CMutex g_objExitMutex;
CCond g_objExitCond;
CCoreDump g_objDump();

#ifdef WIN32
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "libevent.lib")
    #pragma comment(lib, "libevent_core.lib")
    #pragma comment(lib, "libevent_extras.lib")
    #pragma comment(lib, "libmysql.lib")
    #pragma comment(lib, "HLBind.lib")
    #pragma comment(lib, "HBase.lib")
#endif

bool checkKey(std::string  &strParam)
{
    if (strParam.size() <= 2)
    {
        return false;
    }

    std::string strTmp(strParam.substr(0, 2));
    if (strTmp != "--")
    {
        return false;
    }

    return true;
}
int parseParam(int argc, char *argv[], std::map<std::string, std::string> &mapParam)
{
    if (argc <= 1)
    {
        return H_RTN_OK;
    }

    std::string strTmp;
    std::string strKey;
    std::string strVal;
    std::list<std::string> lstTmp;
    std::list<std::string>::iterator itTmp;
    for (int i = 1; i < argc; ++i)
    {
        strTmp = argv[i];
        strTmp = CUtils::Trim(strTmp);
        if (strTmp.empty())
        {
            continue;
        }
        
        CUtils::Split(strTmp, "=", lstTmp);
        if (lstTmp.size() > 2)
        {
            H_Printf("%s", "param error.");
            return H_RTN_FAILE;
        }

        itTmp = lstTmp.begin();
        strKey = CUtils::Trim(*itTmp);
        if (!checkKey(strKey))
        {
            H_Printf("%s", "param error.");
            return H_RTN_FAILE;
        }

        if (1 == lstTmp.size())
        {
            mapParam[strKey] = "";
            continue;
        }

        itTmp++;
        strVal = CUtils::Trim(*itTmp);
        mapParam[strKey] = strVal;
    }

    return H_RTN_OK;
}

bool getParam(const char *pszKey, std::map<std::string, std::string> &mapParam, std::string &strVal)
{
    std::map<std::string, std::string>::iterator itParam;
    itParam = mapParam.find(pszKey);
    if (mapParam.end() == itParam)
    {
        return false;
    }

    strVal = itParam->second;
    return true;
}

#ifdef H_OS_WIN
BOOL WINAPI consoleHandler(DWORD msgType)
{
    BOOL bRtn(FALSE);

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
        H_LOG(LOGLV_SYS, "catch console signal %d.", msgType);
        g_objExitCond.Signal();
    }

    return bRtn;
}
#else
void sigHandEntry(int iSigNum)
{
    H_LOG(LOGLV_SYS, "catch signal %d.", iSigNum);
    g_objExitCond.Signal();
}
void writePid(int pId)
{
    std::string strPidPath(CUtils::getProPath() + ".pid");
    std::string strPid(CUtils::toString(pId));
    FILE *pFile(fopen(strPidPath.c_str(), "w"));
    if (NULL == pFile)
    {
        H_Printf("%s", "write pid file error.");
        return;
    }

    fwrite(strPid.c_str(), 1, strPid.size(), pFile);
    fclose(pFile);
}
#endif

CHumble *initParam(void)
{
    std::string strConfFile(CUtils::formatStr("%s%s%s%s", 
        CUtils::getProPath().c_str(), "config", H_PATH_SEPARATOR, "config.ini"));
    CIniFile objIni(strConfFile.c_str());
    if (!objIni.haveNode("Main"))
    {
        H_Printf("%s", "in config.ini not find node 'Main'");
        return NULL;
    }

    //�����߳���
    unsigned short usCore(CUtils::coreCount());

    int iSVId(objIni.getIntValue("Main", "id"));//������ID
    int iSVType(objIni.getIntValue("Main", "type"));//����������
    std::string strRPCKey(objIni.getStringValue("Main", "rpckey"));//RPC KEY
    std::string strScript(objIni.getStringValue("Main", "script"));//�ű�����·��
    unsigned short usPriority((unsigned short)objIni.getIntValue("Main", "priority"));//��־����
    unsigned int uiLoadDiffer((unsigned int)objIni.getIntValue("Main", "loaddiffer"));//�����̸߳��ز�(����) 0��Ч
    unsigned int uiAlarmTime((unsigned int)objIni.getIntValue("Main", "alarmtime"));//����ִ�к�ʱ�澯ֵ(΢��) 0��Ч
    unsigned short usWorkerNum((unsigned short)objIni.getIntValue("Main", "workernum"));//�����߳���
    unsigned short usNetNum((unsigned short)objIni.getIntValue("Main", "netnum"));//�����߳���
    unsigned int uiRPCTimeDeviation((unsigned int)objIni.getIntValue("Main", "rtdeviation"));//����rpc���������ʱ�����(ms) 0��Ч

    usWorkerNum = (H_INIT_NUMBER == usWorkerNum ? usCore : usWorkerNum);
    usNetNum = (H_INIT_NUMBER == usNetNum ? 1 : usNetNum);

    H_Printf("service id %d, type %d", iSVId, iSVType);
    H_Printf("rpc key %s", strRPCKey.c_str());
    H_Printf("load differ %d ms, rpc time deviation %d ms,alarm time %d ��s", uiLoadDiffer, uiRPCTimeDeviation, uiAlarmTime);
    H_Printf("net worker thread %d, runner thread %d", usNetNum, usWorkerNum);
    H_Printf("script folder %s", strScript.c_str());

    CHumble *pHumble = new(std::nothrow) CHumble(usPriority, usWorkerNum, usNetNum, uiAlarmTime, uiLoadDiffer, uiRPCTimeDeviation);
    if (NULL == pHumble)
    {
        H_Printf("%s", H_ERR_MEMORY);
        return NULL;
    }

    pHumble->setRPCKey(strRPCKey);
    pHumble->setScriptPath(strScript.c_str());
    pHumble->setServiceId(iSVId);
    pHumble->setServiceType(iSVType);

    return pHumble;
}

int initTasks(void)
{
    //ע�����񣬼�����
    struct lua_State *pLState(luaL_newstate());
    if (NULL == pLState)
    {
        H_LOG(LOGLV_ERROR, "%s", "luaL_newstate error.");
        return H_RTN_FAILE;
    }

    luaL_openlibs(pLState);
    CReg2Lua::regAll(pLState);
    std::string strLuaFile(CUtils::formatStr("%s%s", g_pHumble->getScriptPath(), "start.lua"));
    if (H_RTN_OK != luaL_dofile(pLState, strLuaFile.c_str()))
    {
        H_LOG(LOGLV_ERROR, "%s", lua_tostring(pLState, -1));
        lua_close(pLState);
        return H_RTN_FAILE;
    }

    lua_close(pLState);
    return H_RTN_OK;
}

void runHumble(void)
{
    //�������ó�ʼ������
    g_pHumble = initParam();
    if (NULL == g_pHumble)
    {
        return;
    }
    //��������
    g_pHumble->Satrt();
    //ע�����񣬼�����
    if (H_RTN_OK == initTasks())
    {
        H_LOG(LOGLV_SYS, "%s", "start service successfully.");
        CLckThis objLckThis(&g_objExitMutex);
        g_objExitCond.Wait(&objLckThis);
        H_LOG(LOGLV_SYS, "%s", "begin stop service.");
    }
    //ֹͣ����
    g_pHumble->Stop();
    H_SafeDelete(g_pHumble);
}

int runDebug(std::map<std::string, std::string> &mapParam)
{
    std::string strVal;
    if (!getParam("--h", mapParam, strVal))
    {
        H_Printf("%s", "miss --h");
        return H_RTN_FAILE;
    }
    if (strVal.empty())
    {
        H_Printf("%s", "miss --h");
        return H_RTN_FAILE;
    }
    std::string strHost(strVal);

    if (!getParam("--p", mapParam, strVal))
    {
        H_Printf("%s", "miss --p");
        return H_RTN_FAILE;
    }
    if (strVal.empty())
    {
        H_Printf("%s", "miss --p");
        return H_RTN_FAILE;
    }
    unsigned short usPort(CUtils::toNumber<unsigned short>(strVal.c_str()));
    if (H_INIT_NUMBER == usPort)
    {
        H_Printf("%s", "--p value error.");
        return H_RTN_FAILE;
    }

    CDebugInput *pInput = new(std::nothrow) CDebugInput(strHost.c_str(), usPort);
    if (NULL == pInput)
    {
        H_Printf("%s", H_ERR_MEMORY);
        return H_RTN_FAILE;
    }

    int iRtn = pInput->EnterInput();
    H_SafeDelete(pInput);

    return iRtn;
}

void printUseage(void)
{
    printf("useage:\n");
    printf("./Humble\n");
    printf("./Humble --b                          run backstage\n");
    printf("./Humble --d --h=127.0.0.1 --p=15000  run debug command\n");
    printf("debug command:\n    quit\n    exit\n    hotfix tasknames(task1,task2...or all) modules(m1,m2...)\n    do tasknames .... done\n");
}

//Humble --b
int main(int argc, char *argv[])
{
    //��������
    std::map<std::string, std::string> mapParam;
    int iRtn(parseParam(argc, argv, mapParam));
    if (H_RTN_OK != iRtn)
    {
        printUseage();
        return iRtn;
    }

    //������
    std::string strVal;
    if (getParam("--d", mapParam, strVal))
    {
        if (H_RTN_OK != runDebug(mapParam))
        {
            printUseage();
            return H_RTN_FAILE;
        }

        return H_RTN_OK;
    }

#ifdef H_OS_WIN
    (void)SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);

    runHumble();

#else
    signal(SIGPIPE, SIG_IGN);//��ĳһ�˹ر����ӣ�����һ����Ȼ����д���ݣ���һ��д���ݺ���յ�RST��Ӧ���˺���д���ݣ��ں˽�����̷���SIGPIPE�ź�
    signal(SIGINT, sigHandEntry);//��ֹ����
    signal(SIGHUP, sigHandEntry);//��ֹ����
    signal(SIGTSTP, sigHandEntry);//ctrl+Z
    signal(SIGTERM, sigHandEntry);//��ֹһ������
    signal(SIGKILL, sigHandEntry);//������������
    signal(SIGABRT, sigHandEntry);//��ֹһ������
    signal(H_SIGNAL_EXIT, sigHandEntry);

    pid_t pId = getpid();
    H_Printf("exit service by command \"kill -%d %d\".", H_SIGNAL_EXIT, pId);
    writePid((int)pId);

    if (getParam("--b", mapParam, strVal))
    {
        pId = fork();
        if (H_INIT_NUMBER == pId)
        {
            runHumble();

            return H_RTN_OK;
        }
        else if (pId > H_INIT_NUMBER)
        {
            return H_RTN_OK;
        }
        else
        {
            H_Printf("%s", "fork process error!");
            return H_RTN_FAILE;
        }
    }
    else
    {
        runHumble();
    }
#endif

    return H_RTN_OK;
}
