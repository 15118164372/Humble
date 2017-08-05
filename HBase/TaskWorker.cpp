
#include "TaskWorker.h"
#include "TaskGlobleQu.h"
#include "MSGDispatch.h"
#include "Sender.h"
#include "Log.h"

H_BNAMSP

void CTaskWorker::Run(H_MSG *pMsg)
{
    switch (pMsg->usEnevt)
    {
        case MSG_TASK_INIT:
        {
            initTask();
        }
        break;
        case MSG_TASK_DEL:
        {
            destroyTask();

            //工作线程任务数减一
            m_objLock.Lock();
            TaskQueue *pTaskQueue(CTaskGlobleQu::getSingletonPtr()->getQueue(m_usIndex));
            H_AtomicAdd(&(pTaskQueue->uiTaskNum), -1);
            m_objLock.unLock();

            CMSGDispatch::getSingletonPtr()->removeEvent(m_strName.c_str());
            CTaskWorker *pTask(this);
            H_SafeDelete(pTask);
        }
        break;
        case MSG_NET_ACCEPT:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            onAccept(pLink);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_LINKED:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            onLinked(pLink);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_CLOSE:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            onClosed(pLink);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_TIME_FRAME:
        {
            H_TICK *pTick((H_TICK *)pMsg->pEvent);
            onFrame(pTick);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_TIME_SEC:
        {
            H_TICK *pTick((H_TICK *)pMsg->pEvent);
            onSec(pTick);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_READ:
        {
            H_TCPBUF *pTcpBuf((H_TCPBUF *)pMsg->pEvent);
            onNetRead(pTcpBuf);
            H_SafeDelArray(pTcpBuf->stBinary.pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_CMD:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_CMD *pCmd((H_CMD *)(pMsg->pEvent + sizeof(H_LINK)));
            size_t iOutLens(H_INIT_NUMBER);
            const char *pRtn(onCMD(pCmd->acCommand, pCmd->acMsg, iOutLens));
            if (NULL != pRtn)
            {
                CSender::getSingletonPtr()->sendCMDRtn(pLink->sock, pRtn, iOutLens);
            }
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_RPCCALL:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_RPC *pRPC((H_RPC *)(pMsg->pEvent + sizeof(H_LINK)));
            char *pRPCMsg(pMsg->pEvent + sizeof(H_LINK) + sizeof(H_RPC));
            size_t iMsgLens((size_t)ntohl((u_long)pRPC->uiMsgLens));
            size_t iOutLens(H_INIT_NUMBER);
            const char *pRtn(onRPCCall(pLink, pRPC->acRPC, pRPCMsg, iMsgLens, iOutLens));
            if (NULL != pRtn && 0 != ntohl((u_long)pRPC->uiId))
            {
                CSender::getSingletonPtr()->sendRPCRtn(pLink->sock, pRPC, pRtn, iOutLens);
            }
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_NET_RPCRTN:
        {
            unsigned int uiId(*((unsigned int*)pMsg->pEvent));
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(uiId)));
            onRPCRtn(uiId, pBinary);
            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_TASK_RPCCALL:
        {
            H_RPC *pRPC((H_RPC *)pMsg->pEvent);
            char *pRPCMsg(pMsg->pEvent + sizeof(H_RPC));
            size_t iOutLens(H_INIT_NUMBER);
            const char *pRtn(onTaskRPCCall(pRPC->acRPC, pRPCMsg, pRPC->uiMsgLens, iOutLens));
            if (NULL != pRtn && 0 != pRPC->uiId)
            {
                CSender::getSingletonPtr()->sendTaskRPCRtn(pRPC, pRtn, iOutLens);
            }
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_TASK_RPCRTN:
        {
            unsigned int uiId(*((unsigned int*)pMsg->pEvent));
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(uiId)));
            onTaskRPCRtn(uiId, pBinary);
            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        default:
            break;
    }
}

H_ENAMSP
