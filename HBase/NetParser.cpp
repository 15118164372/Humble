
#include "NetParser.h"

H_BNAMSP

SINGLETON_INIT(CNetParser)
CNetParser objNetParser;

CNetParser::CNetParser(void)
{
}

CNetParser::~CNetParser(void)
{
}

void CNetParser::addParser(CParser *pParser)
{
    H_ASSERT(H_INIT_NUMBER != strlen(pParser->getName()), "parser is empty.");

    m_objNameLck.wLock();
    m_mapName.insert(std::make_pair(std::string(pParser->getName()), pParser));
    m_objNameLck.unLock();
}

void CNetParser::setParser(const unsigned short usType, const char *pszName)
{
    m_objNameLck.rLock();
    nameit itName = m_mapName.find(std::string(pszName));
    if (m_mapName.end() != itName)
    {
        m_objTypeLck.wLock();
        m_mapType[usType] = itName->second;
        m_objTypeLck.unLock();
    }
    else
    {
        H_Printf("not find parser %s.", pszName);
    }
    m_objNameLck.unLock();
}

CParser *CNetParser::getParser(const unsigned short &usType)
{
    CParser *pParser = NULL;

    m_objTypeLck.rLock();
    typeit itType = m_mapType.find(usType);
    if (m_mapType.end() != itType)
    {
        pParser = itType->second;
    }
    else
    {
        H_Printf("get parser by type %d error.", usType);
    }
    m_objTypeLck.unLock();

    return pParser;
}

const char *CNetParser::getParserNam(const unsigned short usType)
{
    m_objTypeLck.rLock();
    typeit itType = m_mapType.find(usType);
    if (m_mapType.end() != itType)
    {
        m_objTypeLck.unLock();
        return itType->second->getName();
    }
    m_objTypeLck.unLock();

    return "";
}

H_ENAMSP
