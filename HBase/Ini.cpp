
#include "Ini.h"
#include "Funcs.h"

H_BNAMSP

CIniFile::CIniFile(void)
{
}

CIniFile::CIniFile(const char *pszFile)
{
    H_ASSERT(NULL != pszFile, "pointer is null.");
    m_strFile = pszFile;
    readInfo();
}

CIniFile::~CIniFile(void)
{

}

void CIniFile::setFile(const char *pszFile)
{
    H_ASSERT(NULL != pszFile, "pointer is null.");
    m_strFile = pszFile;
    readInfo();
}

bool CIniFile::isNote(const std::string &str) const
{
    if ('#' == str[0])
    {
        return true;
    }

    if (str.size() >= 2)
    {
        if ('/' == str[0] && '/' == str[1])
        {
            return true;
        }
    }    

    return false;
}

void CIniFile::removeNote(std::string &str) const
{
    str = H_GetFrontOfFlag(str, "//");
    str = H_GetFrontOfFlag(str, "#");
}

bool CIniFile::isNode(const std::string &str) const
{
    if (('[' == str[0])
        && (']' == str[str.size() - 1]))
    {
        return true;
    }

    return false;
}

std::string CIniFile::getNode(const std::string &str) const
{
    std::string strTmp(str.substr(1, str.size() - 2));

    return H_Trim(strTmp);
}

bool CIniFile::isKey(const std::string &str) const
{
    if (std::string::npos == str.find("="))
    {
        return false;
    }

    return true;
}

std::string CIniFile::getKey(const std::string &str) const
{   
    std::string strTmp(H_GetFrontOfFlag(str, "="));

    return H_Trim(strTmp);
}

std::string CIniFile::getVal(const std::string &str) const
{
    std::string::size_type iPos(str.find_first_of("="));
    if (std::string::npos == iPos)
    {
        return "";
    }

    std::string strTmp(str.substr(iPos + 1, str.size() - 1));

    return H_Trim(strTmp);
}

void CIniFile::readInfo(void)
{
    m_lstNodeInfo.clear();

    std::fstream inStream(m_strFile.c_str(), std::ios::in);
    if (!inStream.good())
    {
        H_Printf("open file %s error.", m_strFile.c_str());
        inStream.close();

        return;
    }

    char pBuffer[H_ONEK];
    std::string strTmp;
    std::string strNode;

    while(inStream.good())
    {
        H_Zero(pBuffer, sizeof(pBuffer));

        inStream.getline(pBuffer, (std::streamsize)(sizeof(pBuffer) - 1));

        strTmp = std::string(pBuffer);
        strTmp = H_Trim(strTmp);
        if (strTmp.empty()
            || isNote(strTmp))
        {
            continue;
        }

        //去掉注释
        removeNote(strTmp);
        strTmp = H_Trim(strTmp);
        if (strTmp.empty())
        {
            continue;
        }

        if (isNode(strTmp))
        {
            strNode = getNode(strTmp);

            continue;
        }

        if (isKey(strTmp))
        {
            setStringValue(strNode.c_str(), getKey(strTmp).c_str(), getVal(strTmp).c_str());
        }
    }

    inStream.close();

    return;
}

const char *CIniFile::getStringValue(const char *pszNode, const char *pszKey, const char *pszDefault)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    return itKeyInfo->strValue.c_str();
                }
            }
        }
    }

    H_Printf("not find node %s key %s", pszNode, pszKey);

    return pszDefault;
}

int CIniFile::getIntValue(const char *pszNode, const char *pszKey, int iDefault)
{
    const char *pszVal(getStringValue(pszNode, pszKey, NULL));
    if (NULL == pszVal)
    {
        return iDefault;
    }

    if (0 == strlen(pszVal))
    {
        return iDefault;
    }

    return atoi(pszVal);
}

double CIniFile::getFloatValue(const char *pszNode, const char *pszKey, double dDefault)
{
    const char *pszVal(getStringValue(pszNode, pszKey, NULL));
    if (NULL == pszVal)
    {
        return dDefault;
    }

    if (0 == strlen(pszVal))
    {
        return dDefault;
    }

    return atof(pszVal);
}

void CIniFile::setStringValue(const char *pszNode, const char *pszKey, 
    const char *pszValue)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);
    assert(NULL != pszValue);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    itKeyInfo->strValue = pszValue;

                    return;
                }
            }

            //没有key则加入
            stIniKey stKey;
            stKey.strKey = pszKey;
            stKey.strValue = pszValue;
            itNodeInfo->lstKey.push_back(stKey);

            return;
        }
    }

    //没有Node则加入
    stIniNode stNode;
    stIniKey stKey;
    stKey.strKey = pszKey;
    stKey.strValue = pszValue;

    stNode.strNode = pszNode;
    stNode.lstKey.push_back(stKey);

    m_lstNodeInfo.push_back(stNode);

    return;
}

void CIniFile::setIntValue(const char *pszNode, const char *pszKey, 
    int iValue)
{
    setStringValue(pszNode, pszKey, H_ToString(iValue).c_str());
}

void CIniFile::setFloatValue(const char *pszNode, const char *pszKey, 
    double dValue)
{
    setStringValue(pszNode, pszKey, H_ToString(dValue).c_str());
}

void CIniFile::delKey(const char *pszNode, const char *pszKey)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    itNodeInfo->lstKey.erase(itKeyInfo);

                    return;
                }
            }
        }
    }
}

void CIniFile::delNode(const char *pszNode)
{
    assert(NULL != pszNode);

    std::list<stIniNode>::iterator itNodeInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            m_lstNodeInfo.erase(itNodeInfo);

            return;
        }
    }
}

bool CIniFile::haveNode(const char *pszNode)
{
    assert(NULL != pszNode);

    std::list<stIniNode>::iterator itNodeInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            return true;
        }
    }

    return false;
}

void CIniFile::saveTo(const char *pszFile)
{
    assert(NULL != pszFile);

    FILE *pFile = NULL;
    std::string strTmp;
    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;    

    pFile = fopen(pszFile, "w");
    if (NULL == pFile)
    {
        return;
    }

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        strTmp = std::string("[") + itNodeInfo->strNode + std::string("]\n");
        fwrite(strTmp.c_str(), 1, strTmp.size(), pFile);

        for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
            itKeyInfo++)
        {
            strTmp = itKeyInfo->strKey + std::string(" = ") + itKeyInfo->strValue + std::string("\n");
            fwrite(strTmp.c_str(), 1, strTmp.size(), pFile);
        }
    }

    fclose(pFile);

    return;
}

void CIniFile::Save(void)
{
    saveTo(m_strFile.c_str());
}

H_ENAMSP
