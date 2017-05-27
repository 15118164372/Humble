
#ifndef H_NETPARSER_H_
#define H_NETPARSER_H_

#include "Singleton.h"
#include "HStruct.h"

H_BNAMSP

//数据包解析基类
class CParser
{
public:
    CParser(void) 
    {};
    virtual ~CParser(void) {};
    
    const char *getName()
    {
        return m_strName.c_str();
    };

    virtual H_Binary parsePack(char *pAllBuf, const size_t &iLens, size_t &iParsed)
    {
        H_Binary stBinary;

        iParsed = iLens;
        stBinary.iLens = iLens;
        stBinary.pBufer = pAllBuf;

        return stBinary;
    };

    virtual void Response(H_SOCK &sock, H_PROTOTYPE &iProto, const char *pszMsg, const size_t &iLens) {};

protected:
    void setName(const char *pszNam)
    {
        m_strName = pszNam;
    };    

private:
    H_DISALLOWCOPY(CParser);

private:    
    std::string m_strName;
};

//数据包解析管理
class CParserMgr : public CSingleton<CParserMgr>
{
public:
    CParserMgr(void);
    ~CParserMgr(void);

    void addParser(CParser *pParser);
    CParser *getParser(const char *pszParser);

private:
    H_DISALLOWCOPY(CParserMgr);
#ifdef H_OS_WIN 
    #define nameit std::unordered_map<std::string, CParser *>::iterator
    #define name_map std::unordered_map<std::string, CParser *>
#else
    #define nameit std::tr1::unordered_map<std::string, CParser *>::iterator
    #define name_map std::tr1::unordered_map<std::string, CParser *>
#endif

    name_map m_mapName;
};

H_ENAMSP

#endif//H_NETPARSER_H_
