
#include "HTimer.h"
#include "Utils.h"
#include "HThread.h"
#include "Linker.h"
#include "MsgTrigger.h"
#include "WorkerMgr.h"
#include "Log.h"

H_BNAMSP

#define H_ADJUSTTIME 30 * 1000
#define H_RELINKTIME 2 * 1000

CTimerTask::CTimerTask(class CTimer *pTimer, class CMsgTrigger *pMsgTrigger, 
    class CWorkerMgr *pWorkerMgr, class CLinker *pLinker) :
    m_bStop(false), m_iRunFlage(H_INIT_NUMBER), m_pTimer(pTimer),
    m_pMsgTrigger(pMsgTrigger), m_pWorkerMgr(pWorkerMgr), m_pLinker(pLinker)
{}
CTimerTask::~CTimerTask(void)
{}

void CTimerTask::Run(void)
{
    ++m_iRunFlage;
    uint64_t ulCurTime(CUtils::nowMilSecond());
    uint64_t ulLastAdjustLoad(ulCurTime);
    uint64_t ulLastReLink(ulCurTime);
    CPriQuNode *pNode(NULL);
    H_Printf("%s", "start service: timer");

    while (!m_bStop)
    {
        //触发超时消息
        while (NULL != (pNode = m_pTimer->getNode(ulCurTime)))
        {
            m_pMsgTrigger->triggerTimeOut((CWorker*)(pNode->getData()), pNode->getId());
            H_SafeDelete(pNode);
        }

        if (ulCurTime >= ulLastAdjustLoad + H_ADJUSTTIME)
        {
            ulLastAdjustLoad = ulCurTime;
            m_pWorkerMgr->adjustLoad();
        }
        if (ulCurTime >= ulLastReLink + H_RELINKTIME)
        {
            ulLastReLink = ulCurTime;
            m_pLinker->reLink();
        }

        H_Sleep(1);
        ulCurTime = CUtils::nowMilSecond();
    }

    H_Printf("%s", "stop service: timer");
    --m_iRunFlage;
}
void CTimerTask::Stop(void)
{
    if (m_bStop)
    {
        return;
    }

    m_bStop = true;
    unsigned int uiCount(H_INIT_NUMBER);
    while (H_INIT_NUMBER != m_iRunFlage)
    {
        H_Sleep(10);
        uiCount += 10;
        if (uiCount >= 5000)
        {
            H_Printf("%s", "stop service timer task time out.");
            uiCount = H_INIT_NUMBER;
        }
    }
}

CTimer::CTimer(class CMsgTrigger *pMsgTrigger, class CWorkerMgr *pWorkerMgr, class CLinker *pLinker) :
    m_objTimerTask(this, pMsgTrigger, pWorkerMgr, pLinker)
{}
CTimer::~CTimer(void)
{}

void CTimer::timeOut(class CWorker *pWorker, const uint64_t &ulTime, const uint64_t &ulId)
{
    uint64_t ulMark(CUtils::nowMilSecond() + ulTime);
    CPriQuNode *pNode = new(std::nothrow) CPriQuNode(pWorker, ulMark, ulId);
    if (NULL == pNode)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    m_objPriorityQu.Push(pNode);
}
CPriQuNode *CTimer::getNode(const uint64_t &ulMark)
{
    return m_objPriorityQu.Pop(ulMark);
}

void CTimer::Start(void)
{
    CThread::Creat(&m_objTimerTask);
}
void CTimer::Stop(void)
{
    m_objTimerTask.Stop();
}

H_ENAMSP
