
#ifndef H_DISPATCHER_H_
#define H_DISPATCHER_H_

#include "Runner.h"
#include "Adjure_Task.h"
#include "MsgTrigger.h"

H_BNAMSP

//�������
class CWorkerMgr : public CObject
{
public:
    explicit CWorkerMgr(CMsgTrigger *pMsgTrigger);
    ~CWorkerMgr(void);

    //����������Ϣ
    void addAdjureToTask(CWorker *pWorker, CAdjure *pAdjure)
    {
        CMutex *pLckWorker(pWorker->getWorkerLck());

        pLckWorker->Lock();
        pWorker->addAdjure(pAdjure);
        if (!pWorker->getInGloble())
        {
            m_pAllRunner[pWorker->getIndex()].addWorker(pWorker);
            pWorker->setInGloble(true);
        }
        pLckWorker->unLock();
    };
    //��ȡ����
    CWorker *getWorker(const char *pszName);
    //WorkerPool
    CWorker *popPool(void);
    void pushPool(CWorker *pWorker);
    //����ע��
    void regTask(CWorker *pWorker);
    //����ɾ��
    void unRegTask(const char *pszName);    
    //��ȡ����������
    void getAllName(std::list<std::string> *pvcName);

    //�˳�
    void Stop(void);
    //usRunnerNum �����߳���  uiAlarmTime����ִ�и澯ʱ��(ms)  uiAdjustTime Runnerִ�е���ʱ���
    void Start(const unsigned short &usRunnerNum, const unsigned int &uiAlarmTime, const unsigned int &uiAdjustTime);

    //�����̸߳���,����ʱ������ִ�к�ʱ
    void adjustLoad(void);

private:
    CWorkerMgr(void);
    //ע������ʱ������������һ�����߳�
    unsigned short calIndex(const char *pszName);
    void destroyAll(void);

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<std::string, CWorker*>::iterator taskit;
    typedef std::unordered_map<std::string, CWorker*> task_map;
#else
    typedef std::tr1::unordered_map<std::string, CWorker*>::iterator taskit;
    typedef std::tr1::unordered_map<std::string, CWorker*> task_map;
#endif

    unsigned short m_usRunnerNum;
    unsigned int m_uiAdjustTime;
    CRunner *m_pAllRunner;    
    CMsgTrigger *m_pMsgTrigger;
    task_map m_mapTask;
    CRWLock m_objTaskLock;
    CSafeQueue m_objWorkerPool;
};

H_ENAMSP

#endif//H_DISPATCHER_H_
