
#include "DebugInput.h"
#include "NETAddr.h"
#include "HThread.h"
#include "Utils.h"
#include "writer.h"
#include "stringbuffer.h"

H_BNAMSP

CDebugInput::CDebugInput(const char *pszHost, unsigned short &usPort) : m_Sock(H_INVALID_SOCK)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        H_Printf("%s", "setAddr error.");
        return;
    }

    m_Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == m_Sock)
    {
        H_Printf("%s", "creat socket error.");
        return;
    }
    if (0 != connect(m_Sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        H_Printf("connect %s on port %d error.", pszHost, usPort);
        CUtils::closeSock(m_Sock);
        m_Sock = H_INVALID_SOCK;
        return;
    }
}

int CDebugInput::EnterInput(void)
{
    if (H_INVALID_SOCK == m_Sock)
    {
        return H_RTN_FAILE;
    }

    CThread::Creat(&m_objSender);
    m_objSender.waitStart();
    CThread::Creat(&m_objShow);
    m_objShow.waitStart();
    m_objShow.addDebug(m_Sock);

    char acInput[H_ONEK];
    std::string strCmd;
    std::string strTask;
    bool bRunLua = false;
    printf("%s\n\n", "enter command:");

    while (true)
    {
        H_Zero(acInput, sizeof(acInput));
        std::cin.getline(acInput, sizeof(acInput) - 1);
        std::string strInput = CUtils::Trim(std::string(acInput));
        if (strInput.empty())
        {
            continue;
        }
        if ("quit" == strInput)
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
                if (!sendCmd("do", strTask.c_str(), strCmd.c_str()))
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
        CUtils::Split(strInput, " ", lstCmd);
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
            if (!sendCmd("hotfix", strTask.c_str(), strFile.c_str()))
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

    CUtils::shutDown(m_Sock);
    m_objSender.Stop();
    m_objShow.Stop();

    return H_RTN_OK;
}

bool CDebugInput::sendCmd(const char *pszCmd, const char *pszTo, const char *pszContent)
{
    rapidjson::StringBuffer objBuf;
    rapidjson::Writer<rapidjson::StringBuffer> objWriter(objBuf);
    objWriter.StartObject();
    objWriter.Key("cmd");
    objWriter.String(pszCmd);
    objWriter.Key("task");
    objWriter.String(pszTo);
    objWriter.Key("content");
    objWriter.String(pszContent);
    objWriter.EndObject();
    
    std::string strJson(objBuf.GetString());
    if (strJson.size() > static_cast<size_t>((std::numeric_limits<unsigned short>::max)()))
    {
        H_Printf("%s", "too large.");
        return false;
    }

    unsigned short usHead(ntohs((u_short)strJson.size()));
    size_t iTotal(sizeof(usHead) + strJson.size());
    char *psendBuf = new(std::nothrow) char[iTotal];
    if (NULL == psendBuf)
    {
        H_Printf("%s", H_ERR_MEMORY);
        return false;
    }

    memcpy(psendBuf, &usHead, sizeof(usHead));
    memcpy(psendBuf + sizeof(usHead), strJson.c_str(), strJson.size());
    CBuffer *pBuffer = new(std::nothrow) CBuffer(psendBuf, iTotal);
    if (NULL == pBuffer)
    {
        H_SafeDelArray(psendBuf);
        H_Printf("%s", H_ERR_MEMORY);
        return false;
    }

    m_objSender.Send(m_Sock, pBuffer);
    return true;
}

H_ENAMSP
