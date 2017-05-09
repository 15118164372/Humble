
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
    CChan *pChan(CWorkerDisp::getSingletonPtr()->getChan(pszTask));
    H_ASSERT(NULL != pChan, H_FormatStr("get chan by task %s error.", pszTask).c_str());

    std::string strProto(pszProto);

    m_objStrLock.wLock();
    strprotoit itProto = m_mapStrProto.find(strProto);
    if (m_mapStrProto.end() != itProto)
    {
        H_Printf("proto %s duplicate registration", pszProto);
    }
    m_mapStrProto[strProto] = pChan;
    m_objStrLock.unLock();
}

CChan *CProtoDisp::getStrProto(const char *pszProto)
{
    CChan *pChan(NULL);

    m_objStrLock.rLock();
    strprotoit itProto = m_mapStrProto.find(std::string(pszProto));
    if (m_mapStrProto.end() != itProto)
    {
        pChan = itProto->second;
    }
    m_objStrLock.unLock();

    return pChan;
}

void CProtoDisp::regIProto(int iProto, const char *pszTask)
{
    CChan *pChan(CWorkerDisp::getSingletonPtr()->getChan(pszTask));
    H_ASSERT(NULL != pChan, H_FormatStr("get chan by task %s error.", pszTask).c_str());

    m_objILock.wLock();
    iprotoit itProto = m_mapIProto.find(iProto);
    if (m_mapIProto.end() != itProto)
    {
        H_Printf("proto %d duplicate registration", iProto);
    }
    m_mapIProto[iProto] = pChan;
    m_objILock.unLock();
}

CChan *CProtoDisp::getIProto(int iProto)
{
    CChan *pChan(NULL);

    m_objILock.rLock();
    iprotoit itProto = m_mapIProto.find(iProto);
    if (m_mapIProto.end() != itProto)
    {
        pChan = itProto->second;
    }
    m_objILock.unLock();

    return pChan;
}

H_ENAMSP
