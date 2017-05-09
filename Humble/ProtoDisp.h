
#ifndef H_PROTODISP_H_
#define H_PROTODISP_H_

#include "../HBase/HBase.h"

H_BNAMSP

class CProtoDisp : public CSingleton<CProtoDisp>
{
public:
    CProtoDisp(void);
    ~CProtoDisp(void);

    void regStrProto(const char *pszProto, const char *pszTask);
    CChan *getStrProto(const char *pszProto);

    void regIProto(int iProto, const char *pszTask);    
    CChan *getIProto(int iProto);

private:
#ifdef H_OS_WIN 
    #define strprotoit std::unordered_map<std::string, CChan*>::iterator
    #define strproto_map std::unordered_map<std::string, CChan*>

    #define iprotoit std::unordered_map<int, CChan*>::iterator
    #define iproto_map std::unordered_map<int, CChan*>
#else
    #define strprotoit std::tr1::unordered_map<std::string, CChan*>::iterator
    #define strproto_map std::tr1::unordered_map<std::string, CChan*>

    #define iprotoit std::tr1::unordered_map<int, CChan*>::iterator
    #define iproto_map std::tr1::unordered_map<int, CChan*>
#endif

    strproto_map m_mapStrProto;
    iproto_map m_mapIProto;
    
    CRWLock m_objStrLock;
    CRWLock m_objILock;
};

H_ENAMSP

#endif//H_PROTODISP_H_
