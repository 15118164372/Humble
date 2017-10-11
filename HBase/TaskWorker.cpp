
#include "TaskWorker.h"
#include "NetWorkerMgr.h"
#include "TaskGlobleQu.h"
#include "MSGDispatch.h"
#include "Sender.h"
#include "MQTT.h"
#include "Log.h"

H_BNAMSP

H_PROTOTYPE CTaskWorker::Run(H_MSG *pMsg)
{
    H_PROTOTYPE iProto(H_INIT_NUMBER);

    switch (pMsg->usEnevt)
    {
        //任务初始化 删除
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

        //网络连接断开
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

        //时间相关
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

        //网络可读
        case MSG_NET_READ:
        {
            H_TCPBUF *pTcpBuf((H_TCPBUF *)pMsg->pEvent);
            iProto = pTcpBuf->iProto;
            onNetRead(pTcpBuf);
            H_SafeDelArray(pTcpBuf->stBinary.pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;

        //cmd
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

        //网络rpc
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

        //任务rpc
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

        //mqtt
        case MSG_MQTT_CONNECT:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_CONNECT_Info stCONNECTInfo;
            if (!CMQTT::getSingletonPtr()->parseCONNECT(pBinary, stFixedHead, stCONNECTInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTCONNECT(pLink, &stFixedHead, &stCONNECTInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PUBLISH:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_PUBLISH_Info stPUBLISHInfo;
            if (!CMQTT::getSingletonPtr()->parsePUBLISH(pBinary, stFixedHead, stPUBLISHInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPUBLISH(pLink, &stFixedHead, &stPUBLISHInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PUBACK:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_PUBACK_Info stPUBACKInfo;
            if (!CMQTT::getSingletonPtr()->parsePUBACK(pBinary, stFixedHead, stPUBACKInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPUBACK(pLink, &stFixedHead, &stPUBACKInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PUBREC:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_PUBREC_Info stPUBRECInfo;
            if (!CMQTT::getSingletonPtr()->parsePUBREC(pBinary, stFixedHead, stPUBRECInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPUBREC(pLink, &stFixedHead, &stPUBRECInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PUBREL:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_PUBREL_Info stPUBRELInfo;
            if (!CMQTT::getSingletonPtr()->parsePUBREL(pBinary, stFixedHead, stPUBRELInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPUBREL(pLink, &stFixedHead, &stPUBRELInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PUBCOMP:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_PUBCOMP_Info stPUBCOMPInfo;
            if (!CMQTT::getSingletonPtr()->parsePUBCOMP(pBinary, stFixedHead, stPUBCOMPInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPUBCOMP(pLink, &stFixedHead, &stPUBCOMPInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_SUBSCRIBE:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_SUBSCRIBE_Info stSUBSCRIBEInfo;
            if (!CMQTT::getSingletonPtr()->parseSUBSCRIBE(pBinary, stFixedHead, stSUBSCRIBEInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTSUBSCRIBE(pLink, &stFixedHead, &stSUBSCRIBEInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_UNSUBSCRIBE:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            MQTT_UNSUBSCRIBE_Info stUNSUBSCRIBEInfo;
            if (!CMQTT::getSingletonPtr()->parseUNSUBSCRIBE(pBinary, stFixedHead, stUNSUBSCRIBEInfo))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTUNSUBSCRIBE(pLink, &stFixedHead, &stUNSUBSCRIBEInfo);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_PINGREQ:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            if (!CMQTT::getSingletonPtr()->parsePINGREQ(pBinary, stFixedHead))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTPINGREQ(pLink, &stFixedHead);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;
        case MSG_MQTT_DISCONNECT:
        {
            H_LINK *pLink((H_LINK *)pMsg->pEvent);
            H_Binary *pBinary((H_Binary *)(pMsg->pEvent + sizeof(H_LINK)));

            MQTT_FixedHead stFixedHead;
            if (!CMQTT::getSingletonPtr()->parseDISCONNECT(pBinary, stFixedHead))
            {
                CNetWorkerMgr::getSingletonPtr()->closeLink(pLink->sock);
            }
            else
            {
                onMQTTDISCONNECT(pLink, &stFixedHead);
            }

            H_SafeDelArray(pBinary->pBufer);
            H_SafeDelArray(pMsg->pEvent);
        }
        break;

        default:
            break;
    }

    return iProto;
}

H_ENAMSP
