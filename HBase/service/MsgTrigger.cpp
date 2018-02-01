
#include "MsgTrigger.h"
#include "WorkerMgr.h"

H_BNAMSP

void CMsgTrigger::removeEvent(CWorker *pWorker)
{
    unRegAcceptEvent(pWorker);
    unRegConnectEvent(pWorker);
    unRegCloseEvent(pWorker);
}

void CMsgTrigger::triggerTimeOut(CWorker *pWorker, const uint64_t &ulId)
{
    CTaskTimeOutAdjure *pAdjure = new(std::nothrow) CTaskTimeOutAdjure(ulId);
    if (NULL == pAdjure)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    m_pWorkerMgr->addAdjureToTask(pWorker, pAdjure);
}

void CMsgTrigger::regNetEvent(CRWLock &objNetEventLck, netevent_map &mapEvent, CWorker *pWorker, const unsigned short &usType)
{
    objNetEventLck.wLock();
    neteventit it(mapEvent.find(pWorker));
    if (mapEvent.end() == it)
    {
        std::vector<unsigned short> vcTmp;
        vcTmp.push_back(usType);
        mapEvent[pWorker] = vcTmp;
    }
    else
    {
        if (it->second.end() == std::find(it->second.begin(), it->second.end(), usType))
        {
            it->second.push_back(usType);
        }
    }
    objNetEventLck.wunLock();
}
void CMsgTrigger::unRegNetEvent(CRWLock &objNetEventLck, netevent_map &mapEvent, CWorker *pWorker)
{
    objNetEventLck.wLock();
    neteventit it(mapEvent.find(pWorker));
    if (mapEvent.end() != it)
    {
        mapEvent.erase(it);
    }
    objNetEventLck.wunLock();
}
void CMsgTrigger::triggerNetEvent(CWorker *pBindWorker, CRWLock &objNetEventLck, netevent_map &mapEvent,
    const unsigned short usEvent, const H_SOCK &uiSock, const unsigned short &usType, const uint64_t &ulId)
{
    if (NULL != pBindWorker)
    {
        CTaskNetEvAdjure *pNetEv = new(std::nothrow) CTaskNetEvAdjure(usEvent, uiSock, usType);
        if (NULL != pNetEv)
        {
            pNetEv->setBindId(ulId);
            m_pWorkerMgr->addAdjureToTask(pBindWorker, pNetEv);
        }

        return;
    }

    objNetEventLck.rLock();
    for (neteventit it = mapEvent.begin(); mapEvent.end() != it; ++it)
    {
        if (it->second.end() != std::find(it->second.begin(), it->second.end(), usType))
        {
            CTaskNetEvAdjure *pNetEv = new(std::nothrow) CTaskNetEvAdjure(usEvent, uiSock, usType);
            if (NULL == pNetEv)
            {
                H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
                continue;
            }

            pNetEv->setBindId(ulId);
            m_pWorkerMgr->addAdjureToTask(it->first, pNetEv);
        }
    }
    objNetEventLck.runLock();
}

void CMsgTrigger::regIProto(CWorker *pWorker, H_PROTO_TYPE &iProto)
{
    m_objIProtoLck.wLock();
    iprotoit it(m_mapIProto.find(iProto));
    H_ASSERT(m_mapIProto.end() == it, "proto repeat register.");
    m_mapIProto[iProto] = pWorker;
    m_objIProtoLck.wunLock();

    H_LOG(LOGLV_SYS, "%s register integer protocol %d", pWorker->getName(), iProto);
}
void CMsgTrigger::triggerIProto(CINetReadAdjure *pAdjure)
{
    CWorker *pWorker(NULL);

    m_objIProtoLck.rLock();
    iprotoit it(m_mapIProto.find(pAdjure->getProto()));
    if (m_mapIProto.end() != it)
    {
        pWorker = it->second;
    }
    m_objIProtoLck.runLock();

    if (NULL == pWorker)
    {
        H_SafeDelete(pAdjure);
        return;
    }

    m_pWorkerMgr->addAdjureToTask(pWorker, pAdjure);
}

void CMsgTrigger::regHttpdProto(CWorker *pWorker, const char *pszProto)
{
    m_objStrProtoLck.wLock();
    strprotoit it(m_mapStrProto.find(pszProto));
    H_ASSERT(m_mapStrProto.end() == it, "proto repeat register.");
    m_mapStrProto[pszProto] = pWorker;
    m_objStrProtoLck.wunLock();

    H_LOG(LOGLV_SYS, "%s register string protocol %s", pWorker->getName(), pszProto);
}
void CMsgTrigger::triggerHttpdProto(CTaskHttpdAdjure *pAdjure)
{
    CWorker *pWorker(NULL);

    m_objStrProtoLck.rLock();
    strprotoit it(m_mapStrProto.find(pAdjure->getPath()));
    if (m_mapStrProto.end() != it)
    {
        pWorker = it->second;
    }
    m_objStrProtoLck.runLock();

    if (NULL == pWorker)
    {
        H_SafeDelete(pAdjure);
        return;
    }

    m_pWorkerMgr->addAdjureToTask(pWorker, pAdjure);
}
void CMsgTrigger::triggerDebug(CDebugAdjure *pAdjure)
{
    std::string strToTask = pAdjure->getToTask();
    if (strToTask.empty())
    {
        return;
    }

    std::list<std::string> lstToTask;
    if ("all" == strToTask)
    {
        //所有任务
        m_pWorkerMgr->getAllName(&lstToTask);
    }
    else
    {
        CUtils::Split(strToTask, ",", lstToTask);
    }

    CWorker *pWorker;
    CDebugAdjure *pDebug;
    for (std::list<std::string>::iterator it = lstToTask.begin(); lstToTask.end() != it; ++it)
    {
        if ((*it).empty())
        {
            continue;
        }
        pWorker = m_pWorkerMgr->getWorker((*it).c_str());
        if (NULL == pWorker)
        {
            continue;
        }

        pDebug = new(std::nothrow) CDebugAdjure(pAdjure->getSock(), pAdjure->getType(), (*it).c_str(), 
            pAdjure->getDebug().c_str(), pAdjure->getDebug().size());
        if (NULL == pDebug)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            continue;
        }

        m_pWorkerMgr->addAdjureToTask(pWorker, pDebug);
    }
}

void CMsgTrigger::triggerNetRPC(CNetRPCAdjure *pAdjure)
{
    CWorker *pWorker(m_pWorkerMgr->getWorker(pAdjure->getHead()->acTo));
    if (NULL == pWorker)
    {
        H_SafeDelete(pAdjure);
        return;
    }

    m_pWorkerMgr->addAdjureToTask(pWorker, pAdjure);
}

H_ENAMSP
