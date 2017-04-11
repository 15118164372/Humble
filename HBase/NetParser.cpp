
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

    m_mapName.insert(std::make_pair(std::string(pParser->getName()), pParser));
}

void CNetParser::setParser(const unsigned short usType, const char *pszName)
{
    nameit itName = m_mapName.find(std::string(pszName));
    if (m_mapName.end() != itName)
    {
        m_mapType[usType] = itName->second;
    }
    else
    {
        H_Printf("not find parser %s.", pszName);
    }
}

CParser *CNetParser::getParser(const unsigned short &usType)
{
    CParser *pParser = NULL;

    typeit itType = m_mapType.find(usType);
    if (m_mapType.end() != itType)
    {
        pParser = itType->second;
    }
    else
    {
        H_Printf("get parser by type %d error.", usType);
    }

    return pParser;
}

const char *CNetParser::getParserNam(const unsigned short usType)
{
    typeit itType = m_mapType.find(usType);
    if (m_mapType.end() != itType)
    {
        return itType->second->getName();
    }

    return "";
}

H_ENAMSP
