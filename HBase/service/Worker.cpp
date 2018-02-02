
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
        case MSG_TASK_INIT://�����ʼ��
        {
            initTask();
        }
        break;
        case MSG_TASK_DEL://����ɾ��
        {
            destroyTask();

            m_objWorkerLck.Lock();
            cleanAdjureQu();
            setInGloble(false);
            m_objWorkerLck.unLock();

            m_pWorkerMgr->pushPool(this);
        }
        break;
        case MSG_TIME_TIMEOUT://��ʱ
        {
            onTimeOut((CTaskTimeOutAdjure*)pAdjure);
        }
        break;
        case MSG_NET_ACCEPT://accept�ɹ�
        {
            onAccepted((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_CONNECT://connect�ɹ�
        {
            onConnected((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_CLOSE://���ӹر�
        {
            onClosed((CTaskNetEvAdjure *)pAdjure);
        }
        break;
        case MSG_NET_READ_I://����ɶ�(����������)
        {
            onNetReadI((CINetReadAdjure *)pAdjure);
        }
        break;
        case MSG_NET_READ_HTTPD://����ɶ�(�ַ���������)
        {
            onNetReadHttpd((CTaskHttpdAdjure *)pAdjure);
        }
        break;
        case MSG_NET_RPC://����rpc����
        {
            onNetRPC((CNetRPCAdjure *)pAdjure);
        }
        break;
        case MSG_TASK_RPCCALL://�����rpc����
        {
            onRPCCall((CRPCCallAdjure *)pAdjure);
        }
        break;
        case MSG_TASK_RPCRTN://�����rpc����
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
