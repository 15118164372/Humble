
#include "NetParser.h"

H_BNAMSP

SINGLETON_INIT(CParserMgr)
CParserMgr objParserMgr;

CParserMgr::CParserMgr(void)
{
}

CParserMgr::~CParserMgr(void)
{
}

void CParserMgr::addParser(CParser *pParser)
{
    m_mapName.insert(std::make_pair(std::string(pParser->getName()), pParser));
}

CParser *CParserMgr::getParser(const char *pszParser)
{
    nameit itParser = m_mapName.find(std::string(pszParser));
    if (m_mapName.end() == itParser)
    {
        return NULL;
    }

    return itParser->second;
}

H_ENAMSP
