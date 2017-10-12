
#ifndef H_MSGDISPATCH_H_
#define H_MSGDISPATCH_H_

#include "Singleton.h"
#include "RWLock.h"
#include "HEnum.h"
#include "HStruct.h"

H_BNAMSP

class CMSGDispatch : public CSingleton<CMSGDispatch>
{
private:
    struct H_EVENTDISP
    {
        CRWLock objLock;
        std::list<class CTaskWorker *> lstTask;
    };
public:
    CMSGDispatch(void);
    ~CMSGDispatch(void);

    //usType 网络事件使用  mqtt相同事件只能一个任务注册
    void regEvent(unsigned short usEvent, class CTaskWorker *pTask, const short sType = -1);
    //usEvent MSG_TIME_FRAME
    void unRegTime(unsigned short &usEvent, class CTaskWorker *pTask);
    void sendEvent(unsigned short usEvent, void *pMsg, const size_t &iLens);
    //usEvent usEvent >= MSG_MQTT_CONNECT && usEvent <= MSG_MQTT_DISCONNECT
    void sendMQTTEvent(unsigned short &usEvent, H_LINK &stLink, H_Binary &stBinary);
    void removeEvent(const char *pszName);

    void regNetProto(H_PROTOTYPE &iProto, class CChan *pChan);
    class CChan *getNetProto(H_PROTOTYPE &iProto);

    void regStrProto(const char *pszUrl, class CChan *pChan);
    class CChan *getStrProto(const char *pszUrl);

private:
    H_DISALLOWCOPY(CMSGDispatch);

private:
#ifdef H_OS_WIN
    #define netprotoit std::unordered_map<H_PROTOTYPE , class CChan *>::iterator
    #define netproto_map std::unordered_map<H_PROTOTYPE , class CChan *>

    #define strprotoit std::unordered_map<std::string , class CChan *>::iterator
    #define strproto_map std::unordered_map<std::string , class CChan *>
#else
    #define netprotoit std::tr1::unordered_map<H_PROTOTYPE , class CChan *>::iterator
    #define netproto_map std::tr1::unordered_map<H_PROTOTYPE , class CChan *>

    #define strprotoit std::tr1::unordered_map<std::string , class CChan *>::iterator
    #define strproto_map std::tr1::unordered_map<std::string , class CChan *>
#endif
    
    H_EVENTDISP m_acEvent[MSG_COUNT];
    netproto_map m_mapNetProto;  
    strproto_map m_mapStrProto;
    CRWLock m_objNetLock;
    CRWLock m_objStrLock;
};

H_ENAMSP

#endif//H_MSGDISPATCH_H_
