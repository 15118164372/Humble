
#ifndef H_SERVICE_H_
#define H_SERVICE_H_

#include "Task.h"
#include "Adjure.h"
#include "HCond.h"
#include "LockThis.h"
#include "CirQueue.h"

H_BNAMSP

//�������
class CService : public CTask
{
public:
    explicit CService(const size_t &uiCapacity) : m_bFreeQuAdjure(true), m_bStop(false),
        m_iRunFlage(H_INIT_NUMBER), m_iWait(H_INIT_NUMBER), m_objAdjureQu(uiCapacity)
    {};
    ~CService(void)
    {
        Stop();

        if (m_bFreeQuAdjure)
        {
            CAdjure *pAdjure(NULL);
            while (NULL != (pAdjure = getAdjure()))
            {
                H_SafeDelete(pAdjure);
            }
        }
    };

    const bool &getStop(void)
    {
        return m_bStop;
    };
    //CTask�麯��ʵ��
    void Run(void)
    {
        onStart();
        ++m_iRunFlage;
        H_Printf("start service: %s", m_strServiceName.c_str());

        CAdjure *pAdjure(NULL);
        while (!m_bStop)
        {
            if (NULL != pAdjure)
            {
                onAdjure(pAdjure);
                afterAdjure(pAdjure);
            }

            CLckThis objLckThis(&m_objMutex);
            while (NULL == (pAdjure = getAdjure())
                && !m_bStop)
            {
                ++m_iWait;
                m_objCond.Wait(&objLckThis);
                --m_iWait;
            }
        }

        onStop();
        H_Printf("stop service: %s", m_strServiceName.c_str());
        --m_iRunFlage;
    };

    //������
    void setSVName(const char *pszName)
    {
        m_strServiceName = pszName;
    };
    const char *getSVName(void)
    {
        return m_strServiceName.c_str();
    };
    //�Ƿ��ͷŶ������������(CRunner����Ҫ�ͷ�)
    void setFreeQuAdjure(const bool &bFree)
    {
        m_bFreeQuAdjure = bFree;
    };

    //����ִ������
    void Adjure(CAdjure *pAdjure)
    {
        do
        {
            CLckThis objLckThis(&m_objMutex);
            m_objAdjureQu.Push(pAdjure);
        } while (false);

        if (m_iWait > H_INIT_NUMBER)
        {
            m_objCond.Signal();
        }
    };

    //ִ������
    virtual void onAdjure(CAdjure *pAdjure) {};
    virtual void afterAdjure(CAdjure *pAdjure) {};

    //ѭ������ǰִ��
    virtual void onStart(void) {};
    //ѭ���˳�ʱִ��
    virtual void onStop(void) 
    {
        CAdjure *pAdjure(NULL);
        while (NULL != (pAdjure = getAdjure()))
        {
            onAdjure(pAdjure);
            afterAdjure(pAdjure);
        }
    };

    //�˳�����
    void Stop(void)
    {
        if (m_bStop)
        {
            return;
        }

        m_bStop = true;
        unsigned int uiCount(H_INIT_NUMBER);
        while (H_INIT_NUMBER != m_iRunFlage)
        {
            m_objCond.Signal();

            H_Sleep(10);
            uiCount += 10;
            if (uiCount >= 5000)
            {
                H_Printf("stop service %s time out.", m_strServiceName.c_str());
                uiCount = H_INIT_NUMBER;
            }
        }
    };
    //�ȴ��������
    void waitStart(void)
    {
        unsigned int uiCount(H_INIT_NUMBER);
        while (H_INIT_NUMBER == m_iRunFlage)
        {
            H_Sleep(10);
            uiCount += 10;
            if (uiCount >= 5000)
            {
                H_Printf("start service %s time out.", m_strServiceName.c_str());
                uiCount = H_INIT_NUMBER;
            }
        }
    };

private:
    CService(void);
    CAdjure *getAdjure(void)
    {
        return (CAdjure *)m_objAdjureQu.Pop();
    };

private:
    bool m_bFreeQuAdjure;
    bool m_bStop;
    int m_iRunFlage;
    int m_iWait;
    CMutex m_objMutex;
    CCond m_objCond;
    std::string m_strServiceName;
    CCirQueue m_objAdjureQu;
};

H_ENAMSP

#endif//H_SERVICE_H_
