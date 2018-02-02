
#ifndef H_EVENTSERVICE_H_
#define H_EVENTSERVICE_H_

#include "Task.h"
#include "Adjure.h"
#include "Buffer.h"
#include "CirQueue.h"

H_BNAMSP

enum READ_RETURN
{
    READ_DATA_RECEIVED,    //有数据
    READ_NO_DATA_RECEIVED, //无数据
    READ_ERROR,            //错误
    READ_MEMORY_ERROR      //内存不足，关闭连接
};

class CEventService : public CTask
{
public:
    explicit CEventService(const size_t &uiCapacity);
    ~CEventService(void);

    //服务名
    void setSVName(const char *pszName);
    const char *getSVName(void);
    //是否释放队列里面的命令(CRunner 不需要释放)
    void setFreeQuAdjure(const bool &bFree);

    //CTask虚函数实现
    void Run(void);

    //请求执行命令
    void Adjure(CAdjure *pAdjure);

    //执行命令
    virtual void onAdjure(CAdjure *pAdjure) {};
    virtual void afterAdjure(CAdjure *pAdjure) {};
    //循环启动前执行
    virtual void onStart(void) {};
    //循环退出时执行
    virtual void onStop(void);

    //退出服务
    void Stop(void);
    //等待启动完成
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
