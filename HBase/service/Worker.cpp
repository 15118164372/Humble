
#include "Worker.h"
#include "WorkerMgr.h"
#include "Log.h"

H_BNAMSP

bool CWorker::addAdjure(CAdjure *pAdj)
{
    if (m_bFreeze)
    {
        H_LOG(LOGLV_WARN, "add task to freeze worker %s.", getName());
        return false;
    }

    return m_objAdjureQu.Push(pAdj);
}

void CWorker::Run(CAdjure *pAdjure)
{
    if (!pAdjure->getNorProc())
    {
        onUnNorProc(pAdjure);
        return;
    }

    switch (pAdjure->getAdjure())
    {
        case MSG_TASK_INIT://任务初始化
        {
            initTask();
        }
        break;
        case MSG_TASK_DEL://任务删除
        {
            destroyTask();

            m_objWorkerLck.Lock();
            cleanAdjureQu();
            setInGloble(false);
            m_objWorkerLck.unLock();

            m_pWorkerMgr->pushPool(this);
        }
        break;
        case MSG_TIME_TIMEOUT://超时
        {
            onTimeOut((CTaskTimeOutAdjure*)pAdjure);
        }
        break;
        case MSG_NET_ACCEPT://accept成功
        {
            onAccepted((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_CONNECT://connect成功
        {
            onConnected((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_CLOSE://连接关闭
        {
            onClosed((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_READ_I://网络可读(数字请求码)
        {
            onNetReadI((CINetReadAdjure *)pAdjure);
        }
        break;
        case MSG_NET_READ_HTTPD://网络可读(字符串请求码)
        {
            onNetReadHttpd((CTaskHttpdAdjure *)pAdjure);
        }
        break;
        case MSG_NET_RPC://网络rpc调用
        {
            onNetRPC((CNetRPCAdjure *)pAdjure);
        }
        break;
        case MSG_TASK_RPCCALL://任务间rpc调用
        {
            onRPCCall((CRPCCallAdjure *)pAdjure);
        }
        break;
        case MSG_TASK_RPCRTN://任务间rpc返回
        {
            onRPCRtn((CRPCRtnAdjure *)pAdjure);
        }
        break;
        case MSG_TASK_DEBUG://debug
        {
            onDebug((CDebugAdjure *)pAdjure);
        }
        break;
        default:
            break;
    }
}

H_ENAMSP
