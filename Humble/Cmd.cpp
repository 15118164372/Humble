
#include "Cmd.h"

H_BNAMSP

bool cmdMode(const int &argc, char *argv[])
{
    if (3 == argc)
    {
        if (0 == strcmp(argv[1], "-d"))
        {
            return true;
        }
    }

    return false;
}

bool sendCmd(H_SOCK sock, const char *pszCommand, const char *pszTask, const char *pszMsg)
{
    if (strlen(pszTask) >= H_TASKNAMELENS)
    {
        H_Printf("%s", "task name len too long.");
        return false;
    }
    if (strlen(pszMsg) >= H_ONEK)
    {
        H_Printf("%s", "message len too long.");
        return false;
    }
    if (H_INVALID_SOCK == sock)
    {
        return false;
    }

    H_PROTOTYPE iProto(H_NTOH(PROTO_CMD));
    unsigned int uiLens((unsigned int)ntohl((u_long)(sizeof(H_CMD) + sizeof(H_PROTOTYPE))));
    char acBuf[sizeof(uiLens) + sizeof(H_CMD) + sizeof(H_PROTOTYPE)] = { 0 };

    H_CMD stCMD;
    H_Zero(&stCMD, sizeof(stCMD));
    memcpy(stCMD.acCommand, pszCommand, strlen(pszCommand));
    memcpy(stCMD.acMsg, pszMsg, strlen(pszMsg));
    memcpy(stCMD.acTask, pszTask, strlen(pszTask));

    memcpy(acBuf, &uiLens, sizeof(uiLens));
    memcpy(acBuf + sizeof(uiLens), &iProto, sizeof(H_PROTOTYPE));
    memcpy(acBuf + sizeof(uiLens) + sizeof(H_PROTOTYPE), &stCMD, sizeof(stCMD));

    if (!H_SockWrite(sock, acBuf, sizeof(acBuf)))
    {
        return false;
    }
    
    uiLens = H_INIT_NUMBER;
    if (0 >= recv(sock, (char*)&uiLens, sizeof(uiLens), 0))
    {
        return false;
    }
    uiLens = (unsigned int)(ntohl((u_long)uiLens));
    char *pBuf = new(std::nothrow) char[uiLens + 1];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    H_Zero(pBuf, uiLens + 1);
    if (0 >= recv(sock, pBuf, uiLens, 0))
    {
        H_SafeDelArray(pBuf);
        return false;
    }

    printf("==>%s\n\n", pBuf);
    H_SafeDelArray(pBuf);

    return true;
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
                if (!sendCmd(cmdSock, "do", strTask.c_str(), strCmd.c_str()))
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
                continue;
            }

            itCmd++;
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
            if (!sendCmd(cmdSock, "hotfix", strTask.c_str(), strFile.c_str()))
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

H_ENAMSP
