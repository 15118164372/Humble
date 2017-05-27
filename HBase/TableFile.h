
#ifndef H_TABLEFILE_H_
#define H_TABLEFILE_H_

#include "Macros.h"

H_BNAMSP
/*
读取格式为：
姓名 年龄 住址
小明 20   龙泉
....
的文本文件
注释为#或者//
*/
class CTableFile
{
public:
    CTableFile(void);
    ~CTableFile(void){};

    //文件名
    void setFile(const char *pszFile);
    const char *getFile(void) const;
    //拆分标志
    void setSplitFlag(const char *pszSplitFlag);

    //解析
    int Parse(void);
    //是否还有未读行
    bool eof(void) const;
    //下一行
    void nextRow(void);
    //返回到第一行
    void reSet(void);

    /*读取值*/
    const char *getStringValue(const char *pszName, const char *pszDefault = "");
    int getIntValue(const char *pszName, const int iDefault = 0);
    double getFloatValue(const char *pszName, const double dDefault = 0.0);

    /*返回头*/
    const std::map<int, std::string> *getHead(void) const
    {
        return &m_mapTableHead;
    };

private:
    H_DISALLOWCOPY(CTableFile);
    void getValue(const std::string &strValue, std::map<int, std::string> &mapTableHead);
    bool getHead(const std::string &strHead, std::map<int, std::string> &mapTableHead);
    bool checkHead(std::list<std::string> &lstTableHead) const;
    bool parseHead(std::list<std::string> &lstTableHead, std::map<int, std::string> &mapTableHead) const;
    void checkEof(void);
    bool checkNote(const std::string &strMsg) const;
    void Clear(void);

private:
    bool m_bEof;
    int m_iRemoveCount;
    std::string m_strFile;
    std::string m_strSplitFlag;
    std::map<int, std::string> m_mapTableHead;
    std::list<std::map<std::string, std::string> >::iterator m_itNowRow;
    std::list<std::map<std::string, std::string> > m_lstAllValue;
};

H_ENAMSP

#endif//H_TABLEFILE_H_
