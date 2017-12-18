
#ifndef H_MYSQL_H_
#define H_MYSQL_H_

#include "Macros.h"

H_BNAMSP

#ifdef H_MYSQL
#include "mysql.h"

class CDBQuery
{
public:
    CDBQuery(void) {};
    virtual ~CDBQuery(void) {};

    virtual int getIntValue(const char* pszField, const int iNullValue = 0)
    {
        return iNullValue;
    };
    virtual int64_t getI64Value(const char* pszField, const int64_t iNullValue = 0)
    {
        return iNullValue;
    };
    virtual double getFloatValue(const char* pszField, const double fNullValue = 0.0)
    {
        return fNullValue;
    };
    virtual const char* getStringValue(const char* pszField, const char* pszNullValue = "")
    {
        return pszNullValue;
    };
    virtual const unsigned char* getBlobValue(const char* pszField, int& iLen)
    {
        return NULL;
    };

    virtual bool Eof(void)
    {
        return true;
    };

    virtual void nextRow(void) {};
    virtual void Finalize(void) {};
};


class CMySQLQuery : public CDBQuery
{
public:    
    CMySQLQuery(MYSQL_RES *mysql_res);
    ~CMySQLQuery(void);
    
    int getIntValue(const char* pszField, const int iNullValue = 0);
    int64_t getI64Value(const char* pszField, const int64_t iNullValue = 0);
    double getFloatValue(const char* pszField, const double fNullValue = 0.0);
    const char* getStringValue(const char* pszField, const char* pszNullValue = "");
    const unsigned char* getBlobValue(const char* pszField, int& iLen);

    bool Eof(void);
    void nextRow(void);
    //释放资源
    void Finalize(void);

private:
    CMySQLQuery(void);
    H_DISALLOWCOPY(CMySQLQuery);
    void checkVM(void);
    int fieldIndex(const char* pszField);
    void setMysqlRst(MYSQL_RES *mysql_res);
    int getIntField(const int &iField, const int &iNullValue);
    int64_t getInt64Field(const int &iField, const int64_t &iNullValue);
    double getFloatField(const int &iField, const double &fNullValue);
    const char* getStringField(const int &iField, const char* pszNullValue);
    const unsigned char* getBlobField(const int &iField, int &iLen);

private:
    MYSQL_RES*		m_Mysql_Res;
    MYSQL_FIELD*	m_Field;
    MYSQL_ROW		m_Row;
    unsigned long	m_Row_Count;
    unsigned int	m_Field_Count;
};


class CMySQLStmtQuery : public CDBQuery
{
public:
    CMySQLStmtQuery(MYSQL_BIND *pRstBinder, struct BindInfo *pStBindInfo,
        MYSQL_STMT *pStmt, unsigned int uiCol);
    ~CMySQLStmtQuery(void);
        
    int getIntValue(const char* pszField, const int iNullValue = 0);
    int64_t getI64Value(const char* pszField, const int64_t iNullValue = 0);
    double getFloatValue(const char* pszField, const double fNullValue = 0.0);
    const char* getStringValue(const char* pszField, const char* pszNullValue = "");
    const unsigned char* getBlobValue(const char* pszField, int &iLen);

    bool Eof(void);
    void nextRow(void){};

    //释放资源
    void Finalize(void);

private:
    CMySQLStmtQuery(void);
    H_DISALLOWCOPY(CMySQLStmtQuery);
    void CheckIndex(int iField);
    void freeRes(void);
    int fieldIndex(const char* pszField);
    int64_t getInt64Field(const int &iField, const int64_t &iNullValue);
    double getFloatField(const int &iField, const double &dNullValue);
    const char* getStringField(const int &iField, const char* pszNullValue);
    const unsigned char* getBlobField(const int &iField, int &iLen);

private:
    MYSQL_BIND *m_pRstBinder;
    struct BindInfo *m_pStBindInfo;
    MYSQL_STMT *m_pStmt;
    unsigned int m_uiCol;
};


class CMySQLStatement
{
public:
    CMySQLStatement(MYSQL *pDB, MYSQL_STMT *pStmt);
    ~CMySQLStatement(void);

    int execDML(void);
    CDBQuery *execQuery(void);

    void bindString(const int iField, const char* pszValue);
    void bindInt(const int iField, const int iValue);
    void bindInt64(const int iField, const int64_t iValue);
    void bindFloat(const int iField, const double dValue);
    void bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen);
    void bindNull(const int iField);
    
    void clearParam(void);
    //释放资源
    void Finalize(void);

private:
    CMySQLStatement(void);
    H_DISALLOWCOPY(CMySQLStatement);
    void clearExecQuery(MYSQL_BIND *pBinder, struct BindInfo *pStBindInfo);
    void reSet(void);
    void freeVM(void);
    void checkVM(void);
    void checkBinder(void);
    void CheckParam(int iParam);
    void BindParam(void);
    void setBindInfo(size_t iLens, struct BindMemInfo &stInfo);
    void getRstBinderInfo(MYSQL_BIND *pBinder, struct BindInfo *pStBindInfo);
    void getFiledInfo(void);

private:
    MYSQL_STMT *m_pStmt;
    MYSQL_BIND *m_pBinder_Param;
    MYSQL_RES  *m_pMysql_Res;
    MYSQL_FIELD *m_pField;
    struct BindMemInfo *m_pBindMemInfo;
    unsigned int m_uiParamNum;
    unsigned int m_uiCol; 
};


class CMySQLLink 
{
public:
    CMySQLLink(void);
    ~CMySQLLink(void);

    int Open(const char *pszHost, unsigned short usPort, const char *pszUser, const char *pszPSW, const char *pszDB);
    void Close(void);

    void Trans(void);
    void Commit(void);
    void rollBack(void);

    int execDML(const char* szSQL);
    CDBQuery* execQuery(const char* szSQL);
    CMySQLStatement *compileStatement(const char* pszSQL);

    void setBusyTimeout(int nMillisecs);

    int reLink(void);

private:
    H_DISALLOWCOPY(CMySQLLink);
    void checkDB(void);

private:
    bool m_bTrans;
    MYSQL* m_pDb_Ptr;    
};

#endif

H_ENAMSP

#endif//H_MYSQL_H_
