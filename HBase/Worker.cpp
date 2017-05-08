
#include "Worker.h"
#include "Tick.h"
#include "WorkerDisp.h"

H_BNAMSP

void CWorkerTask::Run(void)
{
    switch (*m_pCMD)
    {
        case TCMD_INIT:
        {
            initTask();
        }
        break;

        case TCMD_RUN:
        {
            runTask();
        }
        break;

        case TCMD_DEL:
        {
            destroyTask();
            CWorkerTask *pTask(this);
            H_SafeDelete(pTask);
        }
        break;

        default:
            break;
    }
};

CWorker::CWorker(void) : CRecvTask<CWorkerTask>(H_QULENS_WORKER),
m_usIndex(H_INIT_NUMBER), m_uiStatus(H_INIT_NUMBER)
{
    setDel(false);
}

CWorker::~CWorker(void)
{

}

void CWorker::setBusy(void)
{
    H_AtomicSet(&m_uiStatus, 1);
}

void CWorker::runTask(CWorkerTask *pMsg)
{
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, pMsg->getName()->c_str());
    pMsg->Run();
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, NULL);
    pMsg->setStatus(H_INIT_NUMBER);
    H_AtomicSet(&m_uiStatus, H_INIT_NUMBER);
}

unsigned int CWorker::getStatus(void)
{
    return H_AtomicGet(&m_uiStatus);
}

H_ENAMSP
