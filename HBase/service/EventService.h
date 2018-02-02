
#ifndef H_EVENTSERVICE_H_
#define H_EVENTSERVICE_H_

#include "Task.h"
#include "Adjure.h"
#include "Buffer.h"
#include "CirQueue.h"

H_BNAMSP

enum READ_RETURN
{
    READ_DATA_RECEIVED,    //������
    READ_NO_DATA_RECEIVED, //������
    READ_ERROR,            //����
    READ_MEMORY_ERROR      //�ڴ治�㣬�ر�����
};

class CEventService : public CTask
{
public:
    explicit CEventService(const size_t &uiCapacity);
    ~CEventService(void);

    //������
    void setSVName(const char *pszName);
    const char *getSVName(void);
    //�Ƿ��ͷŶ������������(CRunner ����Ҫ�ͷ�)
    void setFreeQuAdjure(const bool &bFree);

    //CTask�麯��ʵ��
    void Run(void);

    //����ִ������
    void Adjure(CAdjure *pAdjure);

    //ִ������
    virtual void onAdjure(CAdjure *pAdjure) {};
    virtual void afterAdjure(CAdjure *pAdjure) {};
    //ѭ������ǰִ��
    virtual void onStart(void) {};
    //ѭ���˳�ʱִ��
    virtual void onStop(void);

    //�˳�����
    void Stop(void);
    //�ȴ��������
    void waitStart(void);

    CAdjure *getAdjure(void);

    static int sockPair(H_SOCK acSock[2]);
    void *getLoop(void)
    {
        return m_pBase;
    };
    const bool &getStop(void)
    {
        return m_bStop;
    };
    static READ_RETURN sockRead(H_SOCK &sock, CDynaBuffer *pSockBuf);

private:
    CEventService(void);
    static int creatListener(H_SOCK &fdListener);

private:
    bool m_bStop;
    bool m_bFreeQuAdjure;
    void *m_pStopEvent;
    void *m_pAdjureEvent;
    void *m_pBase;
    int m_iRunFlage;
    H_SOCK m_sockStop[2];
    H_SOCK m_sockAdjure[2];
    std::string m_strServiceName;
    CSafeQueue m_objAdjureQu;
};

H_ENAMSP

#endif//H_EVENTSERVICE_H_
