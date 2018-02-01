
#include "Runner.h"
#include "Log.h"
#include "WorkerMgr.h"
#include "Utils.h"

H_BNAMSP

CRunner::CRunner(void) : CService(H_QULENS_RUNNER), m_usToIndex(H_INIT_NUMBER), m_uiWorkerNum(H_INIT_NUMBER),
    m_uiRunTime(H_INIT_NUMBER), m_uiAlarmTime(H_INIT_NUMBER), m_uiAdjustLoad(H_INIT_NUMBER),
    m_pAllRunner(NULL)
{
    setFreeQuAdjure(false);
}
CRunner::~CRunner(void)
{
}

void CRunner::logAlarm(const unsigned short &usAdjure, const double &dRunTime, CWorker *pWorker, CAdjure *pAdjure)
{
    switch (usAdjure)
    {
        case MSG_NET_READ_I:
        {
            H_LOG(LOGLV_WARN, "task %s, event %d protocol %d run time %f ms.",
                pWorker->getName(), usAdjure, ((CINetReadAdjure *)pAdjure)->getProto(), dRunTime);
        }
        break;
        case MSG_NET_READ_HTTPD:
        {
            H_LOG(LOGLV_WARN, "task %s, event %d protocol %s run time %f ms.",
                pWorker->getName(), usAdjure, ((CTaskHttpdAdjure *)pAdjure)->getPath(), dRunTime);
        }
        break;
        default:
            H_LOG(LOGLV_WARN, "task %s, event %d run time %f ms.",
                pWorker->getName(), usAdjure, dRunTime);
            break;
    }
}
void CRunner::onAdjure(CAdjure *pAdjure)
{
    double dRunTime;
    CAdjure *pTaskAdjure;
    unsigned short usAdjure(H_INIT_NUMBER);
    CWorker *pWorker(((CRunWorkerAdjure *)pAdjure)->getWorker());
    CMutex *pLckWorker(pWorker->getWorkerLck());
    for (int iCount = H_INIT_NUMBER; iCount < H_MAXRUNNUM; ++iCount)
    {
        pLckWorker->Lock();
        pTaskAdjure = pWorker->getAdjure();
        pLckWorker->unLock();
        if (NULL == pTaskAdjure)
        {
            break;
        }

        usAdjure = pTaskAdjure->getAdjure();
        switch (usAdjure)
        {
            case MSG_TASK_DEL:
            {
                addWorkerNum(-1);
                pWorker->Run(pTaskAdjure);
                H_SafeDelete(pTaskAdjure);
                return;
            }
            break;
            case MSG_TASK_INIT:
            {
                addWorkerNum(1);
                pWorker->Run(pTaskAdjure);
            }
            break;
            default:
            {
                m_objClock.reStart();
                pWorker->Run(pTaskAdjure);
                dRunTime = m_objClock.Elapsed();
                H_AtomicAdd(&m_uiRunTime, (unsigned int)(1000 * dRunTime));
                if (m_uiAlarmTime > H_INIT_NUMBER
                    && dRunTime > m_uiAlarmTime)
                {
                    logAlarm(usAdjure, dRunTime, pWorker, pTaskAdjure);
                }
            }
            break;
        }
        H_SafeDelete(pTaskAdjure);
    }

    CRunner *pRunner(this);
    //调整负载
    if (H_INIT_NUMBER != H_AtomicGet(&m_uiAdjustLoad))
    {
        if (getWorkerNum() > 1)
        {
            pRunner = adjustLoad(pWorker, m_usToIndex);
        }
        H_AtomicSet(&m_uiAdjustLoad, H_INIT_NUMBER);
    }

    //调整全局队列
    pLckWorker->Lock();
    adjustInGloble(pWorker, pRunner);
    pLckWorker->unLock();
}
void CRunner::adjustInGloble(CWorker *pWorker, CRunner *pRunner)
{
    if (H_INIT_NUMBER == pWorker->getAdjureNum())
    {
        pWorker->setInGloble(false);
        return;
    }

    pRunner->addWorker(pWorker);
}
CRunner *CRunner::adjustLoad(CWorker *pWorker, const unsigned short &usToIndex)
{
    //当前工作线程任务数减一
    addWorkerNum(-1);
    //新工作线程务数加一
    CRunner *pRunner = &m_pAllRunner[usToIndex];
    pRunner->addWorkerNum(1);

    CMutex *pLckWorker(pWorker->getWorkerLck());

    pLckWorker->Lock();
    unsigned short usOldIndex(pWorker->getIndex());
    pWorker->setIndex(usToIndex);
    pLckWorker->unLock();

    H_LOG(LOGLV_SYS, "move task %s run thread. %d--->%d", pWorker->getName(), usOldIndex, usToIndex);
    return pRunner;
}
void CRunner::adjustLoad(const unsigned short &usToIndex)
{
    m_usToIndex = usToIndex;
    H_AtomicSet(&m_uiAdjustLoad, 1);
}
void CRunner::setIndex(const unsigned short &usIndex)
{
    setSVName(CUtils::formatStr("%s%d", H_SERVICE_RUNNER, usIndex).c_str());
}

void CRunner::setAllRunner(CRunner *pAllRunner)
{
    m_pAllRunner = pAllRunner;
}

void CRunner::addWorkerNum(const int iNum)
{
    H_AtomicAdd(&m_uiWorkerNum, iNum);
}

unsigned int CRunner::getWorkerNum(void)
{
    return H_AtomicGet(&m_uiWorkerNum);
}

unsigned int CRunner::getRunTime(void)
{
    return H_AtomicGet(&m_uiRunTime);
}

void CRunner::resetRunTime(void)
{
    H_AtomicSet(&m_uiRunTime, H_INIT_NUMBER);
};

void CRunner::setAlarmTime(const unsigned int &uiTime)
{
    m_uiAlarmTime = uiTime;
}

H_ENAMSP
