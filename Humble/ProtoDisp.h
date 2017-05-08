
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
    void regIProto(int iProto, const char *pszTask);
    const char *getStrProto(const char *pszProto);
    const char *getIProto(int iProto);
private:
#ifdef H_OS_WIN 
    #define strprotoit std::unordered_map<std::string, std::string>::iterator
    #define strproto_map std::unordered_map<std::string, std::string>

    #define iprotoit std::unordered_map<int, std::string>::iterator
    #define iproto_map std::unordered_map<int, std::string>
#else
    #define strprotoit std::tr1::unordered_map<std::string, std::string>::iterator
    #define strproto_map std::tr1::unordered_map<std::string, std::string>

    #define iprotoit std::tr1::unordered_map<int, std::string>::iterator
    #define iproto_map std::tr1::unordered_map<int, std::string>
#endif

    strproto_map m_mapStrProto;
    iproto_map m_mapIProto;
    CAtomic m_objStrLock;
    CAtomic m_objILock;
};

H_ENAMSP

#endif//H_PROTODISP_H_
