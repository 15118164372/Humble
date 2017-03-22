
#include "ProtoDisp.h"

H_BNAMSP

SINGLETON_INIT(CProtoDisp)
CProtoDisp objProtoDisp;

CProtoDisp::CProtoDisp(void)
{
}

CProtoDisp::~CProtoDisp(void)
{
    std::vector<std::string *>::iterator itTask;
    for (itTask = m_vcTask.begin(); m_vcTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(*itTask);
    }

    m_vcTask.clear();
}

std::string *CProtoDisp::getTaskPoint(const char *pszTask)
{
    std::vector<std::string *>::iterator itTask;
    for (itTask = m_vcTask.begin(); m_vcTask.end() != itTask; ++itTask)
    {
        if (**itTask == std::string(pszTask))
        {
            return *itTask;
        }
    }

    std::string *pStr = new(std::nothrow) std::string;
    H_ASSERT(NULL != pStr, "malloc memory error");

    *pStr = pszTask;
    m_vcTask.push_back(pStr);

    return pStr;
}

void CProtoDisp::regStrProto(const char *pszProto, const char *pszTask)
{
    std::string *pTask = getTaskPoint(pszTask);
    m_mapStrProto[std::string(pszProto)] = pTask;
}

void CProtoDisp::regIProto(int iProto, const char *pszTask)
{
    std::string *pTask = getTaskPoint(pszTask);
    m_mapIProto[iProto] = pTask;
}

const char *CProtoDisp::getStrProto(const char *pszProto)
{
    strprotoit itProto = m_mapStrProto.find(std::string(pszProto));
    if (m_mapStrProto.end() != itProto)
    {
        return itProto->second->c_str();
    }

    return NULL;
}

const char *CProtoDisp::getIProto(int iProto)
{
    iprotoit itProto = m_mapIProto.find(iProto);
    if (m_mapIProto.end() != itProto)
    {
        return itProto->second->c_str();
    }

    return NULL;
}

H_ENAMSP
