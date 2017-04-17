
#include "Worker.h"
#include "Tick.h"

H_BNAMSP

CWorker::CWorker(void) : CRecvTask<CWorkerTask>(H_QULENS_WORKER),
    m_usIndex(H_INIT_NUMBER), m_uiStatus(WS_FREE)
{
    setDel(false);
}

CWorker::~CWorker(void)
{

}

void CWorker::setBusy(void)
{
    H_AtomicSet(&m_uiStatus, WS_BUSY);
}

void CWorker::runTask(CWorkerTask *pMsg)
{
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, pMsg->getName()->c_str());
    pMsg->Run();
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, NULL);    
    pMsg->subRef();
    H_AtomicSet(&m_uiStatus, WS_FREE);
}

unsigned int CWorker::getStatus(void)
{
    return H_AtomicGet(&m_uiStatus);
}

H_ENAMSP
