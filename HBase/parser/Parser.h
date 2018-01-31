
#ifndef H_PARSER_H_
#define H_PARSER_H_

#include "Adjure.h"

H_BNAMSP

//���ݰ���������
class CParser : public CObject
{
public:
    CParser(const char *pszName, const bool bHandShake) : m_bHandShake(bHandShake), m_bRPC(false),
        m_uiHSTimeOut(H_INIT_NUMBER), m_strName(pszName)
    {};
    ~CParser(void)
    {};
    const char*getName()
    {
        return m_strName.c_str();
    };
    const bool &needHandShake(void)
    {
        return m_bHandShake;
    };
    //���ֳ�ʱʱ�� (ms)
    void setHSTimeOut(const unsigned int uiTime)
    {
        m_uiHSTimeOut = uiTime;
    };
    const unsigned int &getHSTimeOut(void)
    {
        return m_uiHSTimeOut;
    };
    //�Ƿ�Ϊrpc
    void setRPC(bool bRPC)
    {
        m_bRPC = bRPC;
    };
    const bool &getRPC(void)
    {
        return m_bRPC;
    };

    //pBuf���ݰ� iLens ���ݰ��ܳ��� iParsed ���ν��������ֽ�  psHSStatus ����״̬ bClose �Ƿ�ر�����
    virtual CBuffer *handShake(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
    {
        return NULL;
    };
    virtual CAdjure *Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
    {
        return NULL;
    };
    virtual CAdjure *Parse(const H_SOCK &uiSock, const unsigned short &usType, 
        const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
    {
        return NULL;
    };

private:
    CParser(void);   
    bool m_bHandShake;
    bool m_bRPC;
    unsigned int m_uiHSTimeOut;
    std::string m_strName;
};

//���ݰ���������
class CParserMgr : public CObject
{
public:
    CParserMgr(void) 
    {};
    ~CParserMgr(void) 
    {};

    void addParser(CParser *pParser) 
    {
        m_mapName.insert(std::make_pair(pParser->getName(), pParser));
    };
    CParser *getParser(const char *pszParser) 
    {
        nameit itParser = m_mapName.find(pszParser);
        if (m_mapName.end() == itParser)
        {
            return NULL;
        }

        return itParser->second;
    };

private:
#ifdef H_OS_WIN 
    typedef std::unordered_map<std::string, CParser *>::iterator nameit;
    typedef std::unordered_map<std::string, CParser *> name_map;
#else
    typedef std::tr1::unordered_map<std::string, CParser *>::iterator nameit;
    typedef std::tr1::unordered_map<std::string, CParser *> name_map;
#endif    

    name_map m_mapName;
};

H_ENAMSP

#endif//H_PARSER_H_
