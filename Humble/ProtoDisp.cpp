
#include "ProtoDisp.h"

H_BNAMSP

SINGLETON_INIT(CProtoDisp)
CProtoDisp objProtoDisp;

CProtoDisp::CProtoDisp(void)
{
}

CProtoDisp::~CProtoDisp(void)
{
}

void CProtoDisp::regStrProto(const char *pszProto, const char *pszTask)
{
    std::string strProto(pszProto);

    m_objStrLock.Lock();
    strprotoit itProto = m_mapStrProto.find(strProto);
    if (m_mapStrProto.end() != itProto)
    {
        H_Printf("proto %s duplicate registration", pszProto);
    }
    m_mapStrProto[strProto] = std::string(pszTask);
    m_objStrLock.unLock();
}

void CProtoDisp::regIProto(int iProto, const char *pszTask)
{
    m_objILock.Lock();
    iprotoit itProto = m_mapIProto.find(iProto);
    if (m_mapIProto.end() != itProto)
    {
        H_Printf("proto %d duplicate registration", iProto);
    }
    m_mapIProto[iProto] = std::string(pszTask);
    m_objILock.unLock();
}

const char *CProtoDisp::getStrProto(const char *pszProto)
{
    m_objStrLock.Lock();
    strprotoit itProto = m_mapStrProto.find(std::string(pszProto));
    if (m_mapStrProto.end() != itProto)
    {
        m_objStrLock.unLock();
        return itProto->second.c_str();
    }
    m_objStrLock.unLock();

    return NULL;
}

const char *CProtoDisp::getIProto(int iProto)
{
    m_objILock.Lock();
    iprotoit itProto = m_mapIProto.find(iProto);
    if (m_mapIProto.end() != itProto)
    {
        m_objILock.unLock();
        return itProto->second.c_str();
    }
    m_objILock.unLock();

    return NULL;
}

H_ENAMSP
