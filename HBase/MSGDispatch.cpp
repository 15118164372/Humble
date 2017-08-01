
#include "MSGDispatch.h"
#include "TaskWorker.h"
#include "Funcs.h"
#include "HStruct.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CMSGDispatch)
CMSGDispatch objMSGDispatch;

CMSGDispatch::CMSGDispatch(void)
{

}

CMSGDispatch::~CMSGDispatch(void)
{

}

void CMSGDispatch::regEvent(unsigned short usEvent, class CTaskWorker *pTask, const short sType)
{
    H_ASSERT(usEvent >= 0 && usEvent < MSG_COUNT, "subscript out of range");
    
    if (MSG_NET_ACCEPT == usEvent
        || MSG_NET_LINKED == usEvent
        || MSG_NET_CLOSE == usEvent)
    {
        H_ASSERT(sType >= 0, "require an sock type.");
        pTask->addType(usEvent, sType);
    }

    bool bHave(false);
    std::list<class CTaskWorker *>::iterator itTask;
    H_EVENTDISP *pDisp(&m_acEvent[usEvent]);

    pDisp->objLock.wLock();
    for (itTask = pDisp->lstTask.begin(); pDisp->lstTask.end() != itTask; ++itTask)
    {
        if (*(pTask->getName()) ==  *((*itTask)->getName()))
        {
            bHave = true;
            break;
        }
    }
    if (!bHave)
    {
        pDisp->lstTask.push_back(pTask);
    }    
    pDisp->objLock.wunLock();

    H_LOG(LOGLV_INFO, "%s register event %d", pTask->getName()->c_str(), usEvent);
}

void CMSGDispatch::unRegTime(unsigned short &usEvent, class CTaskWorker *pTask)
{
    H_ASSERT(usEvent == MSG_TIME_FRAME || usEvent == MSG_TIME_SEC, "only support frame and second event.");

    std::list<class CTaskWorker *>::iterator itTask;
    H_EVENTDISP *pDisp(&m_acEvent[usEvent]);

    pDisp->objLock.wLock();
    for (itTask = pDisp->lstTask.begin(); pDisp->lstTask.end() != itTask; ++itTask)
    {
        if (*(pTask->getName()) == *((*itTask)->getName()))
        {
            pDisp->lstTask.erase(itTask);
            H_LOG(LOGLV_INFO, "%s unregister event %d", pTask->getName()->c_str(), usEvent);
            break;
        }
    }
    pDisp->objLock.wunLock();
}

void CMSGDispatch::sendEvent(unsigned short usEvent, void *pMsg, const size_t &iLens)
{
    H_ASSERT(usEvent >= 0 && usEvent < MSG_COUNT, "subscript out of range");

    H_MSG *pEv;
    std::list<class CTaskWorker *>::iterator itTask;
    H_EVENTDISP *pDisp(&m_acEvent[usEvent]);

    pDisp->objLock.rLock();
    for (itTask = pDisp->lstTask.begin(); pDisp->lstTask.end() != itTask; ++itTask)
    {
        if (MSG_NET_ACCEPT == usEvent
            || MSG_NET_LINKED == usEvent
            || MSG_NET_CLOSE == usEvent)
        {
            if (!(*itTask)->haveType(usEvent, ((H_LINK *)pMsg)->usType))
            {
                continue;
            }
        }

        pEv = new(std::nothrow) H_MSG;
        H_ASSERT(NULL != pMsg, "malloc memory error.");
        pEv->pEvent = new(std::nothrow) char[iLens];
        H_ASSERT(NULL != pEv->pEvent, "malloc memory error.");

        pEv->usEnevt = usEvent;
        memcpy(pEv->pEvent, pMsg, iLens);

        if (!(*itTask)->getChan()->Send((void*)pEv))
        {
            H_SafeDelArray(pEv->pEvent);
            H_SafeDelete(pEv);
            H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
        }
    }
    pDisp->objLock.runLock();
}

void CMSGDispatch::removeEvent(const char *pszName)
{
    H_EVENTDISP *pDisp;
    std::list<class CTaskWorker *>::iterator itTask;
    for (int i = 0; i < MSG_COUNT; i++)
    {
        pDisp = &m_acEvent[i];
        pDisp->objLock.wLock();
        for (itTask = pDisp->lstTask.begin(); pDisp->lstTask.end() != itTask; ++itTask)
        {
            if (0 == strcmp((*itTask)->getName()->c_str(), pszName))
            {
                pDisp->lstTask.erase(itTask);
                H_LOG(LOGLV_INFO, "%s unregister event %d", pszName, i);
                break;
            }
        }
        pDisp->objLock.wunLock();
    }
}

void CMSGDispatch::regNetProto(H_PROTOTYPE &iProto, class CChan *pChan)
{
    netprotoit itNet;

    m_objNetLock.wLock();
    itNet = m_mapNetProto.find(iProto);
    H_ASSERT(m_mapNetProto.end() == itNet, "repeat register proto.");
    m_mapNetProto[iProto] = pChan;
    m_objNetLock.wunLock();

    H_LOG(LOGLV_INFO, "%s register proto %d", pChan->getTask()->getName()->c_str(), iProto);
}

class CChan *CMSGDispatch::getNetProto(H_PROTOTYPE &iProto)
{
    class CChan *pChan(NULL);
    netprotoit itNet;

    m_objNetLock.rLock();
    itNet = m_mapNetProto.find(iProto);
    if (m_mapNetProto.end() != itNet)
    {
        pChan = itNet->second;
    }
    m_objNetLock.runLock();

    return pChan;
}

void CMSGDispatch::regStrProto(const char *pszUrl, class CChan *pChan)
{
    strprotoit itNet;

    m_objStrLock.wLock();
    itNet = m_mapStrProto.find(pszUrl);
    H_ASSERT(m_mapStrProto.end() == itNet, "repeat register proto.");
    m_mapStrProto[pszUrl] = pChan;
    m_objStrLock.wunLock();

    H_LOG(LOGLV_INFO, "%s register proto %s", pChan->getTask()->getName()->c_str(), pszUrl);
}

class CChan *CMSGDispatch::getStrProto(const char *pszUrl)
{
    class CChan *pChan(NULL);
    strprotoit itNet;

    m_objStrLock.rLock();
    itNet = m_mapStrProto.find(pszUrl);
    if (m_mapStrProto.end() != itNet)
    {
        pChan = itNet->second;
    }
    m_objStrLock.runLock();

    return pChan;
}

H_ENAMSP
