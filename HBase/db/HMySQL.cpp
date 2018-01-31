
#include "HMySQL.h"
#include "Utils.h"

H_BNAMSP

#ifdef H_MYSQL

struct BindInfo : public CObject
{
    int iIndex;
    std::string strFieldName;
};

struct BindMemInfo : public CObject
{
    size_t iLens;
    char *pMem;
    BindMemInfo(void) : iLens(H_INIT_NUMBER), pMem(NULL)
    {
    };
};

class CMySqlLibInit : public CObject
{
public:
    CMySqlLibInit(void)
    {        
    };
    ~CMySqlLibInit(void)
    {
        mysql_library_end();
    };
};

static CMySqlLibInit objMySqlInit;

CMySQLQuery::CMySQLQuery(MYSQL_RES *mysql_res)
{
    setMysqlRst(mysql_res);
}

CMySQLQuery::~CMySQLQuery(void)
{
    if (m_Mysql_Res != NULL)
    {
        mysql_free_result(m_Mysql_Res);
        m_Mysql_Res = NULL;
    }
}

void CMySQLQuery::Finalize(void)
{
    delete this;
}

void CMySQLQuery::setMysqlRst(MYSQL_RES *mysql_res)
{
    m_Mysql_Res = mysql_res;

    checkVM();
    //定位游标位置到第一个位置
    mysql_data_seek(m_Mysql_Res, 0);
    m_Row =  mysql_fetch_row(m_Mysql_Res);
    m_Row_Count = (unsigned long)mysql_num_rows(m_Mysql_Res); 
    //得到字段数量
    m_Field_Count = mysql_num_fields(m_Mysql_Res);
}

int CMySQLQuery::fieldIndex(const char* pszField)
{
    checkVM();
    unsigned int i(H_INIT_NUMBER);

    mysql_field_seek(m_Mysql_Res, 0);//定位到第0列
    while (i < m_Field_Count)
    {
        m_Field = mysql_fetch_field(m_Mysql_Res);
        if ((m_Field != NULL) 
            && (0 == H_Strcasecmp(m_Field->name, pszField)))//找到
        {
            return i;
        }

        i++;
    }

    H_ASSERT(false, CUtils::formatStr("invalid field name requested %s", pszField).c_str());
}

int CMySQLQuery::getIntField(const int &iField, const int &iNullValue)
{
    const char *pszVal(getStringField(iField, ""));
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return iNullValue;
    }

    return atoi(pszVal);
}

int CMySQLQuery::getIntValue(const char* pszField, const int iNullValue)
{
    int iField(fieldIndex(pszField));
    return getIntField(iField, iNullValue);
}

int64_t CMySQLQuery::getInt64Field(const int &iField, const int64_t &iNullValue)
{
    const char *pszVal(getStringField(iField, ""));
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return iNullValue;
    }
    
    return CUtils::toNumber<int64_t>(pszVal);
}

int64_t CMySQLQuery::getI64Value(const char* pszField, const int64_t iNullValue)
{
    int iField(fieldIndex(pszField));
    return getInt64Field(iField, iNullValue);
}

double CMySQLQuery::getFloatField(const int &iField, const double &fNullValue)
{
    const char *pszVal(getStringField(iField, ""));
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return fNullValue;
    }

    return atof(pszVal);
}

double CMySQLQuery::getFloatValue(const char* pszField, const double fNullValue)
{
    int iField(fieldIndex(pszField));
    return getFloatField(iField, fNullValue);
}

const char* CMySQLQuery::getStringField(const int &iField, const char* pszNullValue)
{
    checkVM();

    if (iField < 0
        || iField  >= (int)m_Field_Count)
    {
        H_ASSERT(false, CUtils::formatStr("invalid field index requested, %d", iField).c_str());
    }

    if (NULL == m_Row)
    {
        return pszNullValue;
    }
    if (NULL == m_Row[iField])
    {
        return pszNullValue;
    }

    return m_Row[iField];
}

const char* CMySQLQuery::getStringValue(const char* pszField, const char* pszNullValue)
{
    int iField(fieldIndex(pszField));
    return getStringField(iField, pszNullValue);
}

const unsigned char* CMySQLQuery::getBlobField(const int &iField, int &iLen)
{
    checkVM();

    if (iField < 0
        || iField  >= (int)m_Field_Count)
    {
        return NULL;
    }
    if (NULL == m_Row)
    {
        return NULL;
    }

    unsigned long *pLens(mysql_fetch_lengths(m_Mysql_Res));
    iLen = (int)pLens[iField];

    return (const unsigned char *)m_Row[iField];
}

const unsigned char* CMySQLQuery::getBlobValue(const char* pszField, int &iLen)
{
    int iField(fieldIndex(pszField));
    return getBlobField(iField, iLen);
}

void CMySQLQuery::nextRow(void)
{
    checkVM();
    m_Row = mysql_fetch_row(m_Mysql_Res);
}

bool CMySQLQuery::Eof(void)
{
    return  NULL == m_Row ? true : false;
}

void CMySQLQuery::checkVM(void)
{
    H_ASSERT(NULL != m_Mysql_Res, "got null pointer.")
}


CMySQLStmtQuery::CMySQLStmtQuery(MYSQL_BIND *pRstBinder, struct BindInfo *pStBindInfo,
    MYSQL_STMT *pStmt, unsigned int uiCol)
{
    m_pRstBinder = pRstBinder;
    m_pStBindInfo = pStBindInfo;
    m_pStmt = pStmt;
    m_uiCol = uiCol;
}

CMySQLStmtQuery::~CMySQLStmtQuery(void)
{
    freeRes();
}

void CMySQLStmtQuery::freeRes(void)
{
    char *pTmp;

    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        if (NULL != m_pRstBinder[i].length)
        {
            H_SafeDelete(m_pRstBinder[i].length);
        }
        if (NULL != m_pRstBinder[i].buffer)
        {
            pTmp = (char*)m_pRstBinder[i].buffer;
            H_SafeDelete(pTmp);
            m_pRstBinder[i].buffer = NULL;
        }
    }

    H_SafeDelArray(m_pStBindInfo);
    H_SafeDelArray(m_pRstBinder);

    (void)mysql_stmt_free_result(m_pStmt);
}

void CMySQLStmtQuery::Finalize(void)
{
    delete this;
}

int CMySQLStmtQuery::fieldIndex(const char* pszField)
{
    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        if (0 == H_Strcasecmp(pszField, m_pStBindInfo[i].strFieldName.c_str()))
        {
            return m_pStBindInfo[i].iIndex;
        }
    }

    H_ASSERT(false, CUtils::formatStr("invalid field name requested %s", pszField).c_str());
}

void CMySQLStmtQuery::CheckIndex(int iField)
{
    if (iField < 0
        || iField  >= (int)m_uiCol)
    {
        H_ASSERT(false, CUtils::formatStr("invalid field index requested, %d", iField).c_str());
    }
}

bool CMySQLStmtQuery::Eof(void)
{
    return (H_RTN_OK == mysql_stmt_fetch(m_pStmt)) ? false : true;
}

int64_t CMySQLStmtQuery::getInt64Field(const int &iField, const int64_t &iNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];
    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_LONGLONG:
        {
            return *(long long*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_LONG:
        {
            return *(int*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_SHORT:
        {
            return *(short*)(stData.buffer);
        }
        break;

    default:
        {
            return iNullValue;
        }
        break;
    }
}

int CMySQLStmtQuery::getIntValue(const char* pszField, const int iNullValue)
{
    int iField(fieldIndex(pszField));
    return (int)getInt64Field(iField, iNullValue);
}

int64_t CMySQLStmtQuery::getI64Value(const char* pszField, const int64_t iNullValue)
{
    int iField(fieldIndex(pszField));
    return getInt64Field(iField, iNullValue);
}

double CMySQLStmtQuery::getFloatField(const int &iField, const double &dNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];
    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_DOUBLE:
        {
            return *(double*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_FLOAT:
        {
            return *(float*)(stData.buffer);
        }
        break;

    default:
        {
            return dNullValue;
        }
        break;
    }
}

double CMySQLStmtQuery::getFloatValue(const char* pszField, const double fNullValue)
{
    int iField(fieldIndex(pszField));    
    return getFloatField(iField, fNullValue);
}

const char* CMySQLStmtQuery::getStringField(const int &iField, const char* pszNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];
    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_JSON:
        {
            return (const char*)(stData.buffer);
        }
        break;

    default:
        {
            return pszNullValue;
        }
        break;
    }
}

const char* CMySQLStmtQuery::getStringValue(const char* pszField, const char* pszNullValue)
{
    int iField(fieldIndex(pszField));
    return getStringField(iField, pszNullValue);
}

const unsigned char* CMySQLStmtQuery::getBlobField(const int &iField, int &iLen)
{
    CheckIndex(iField);

    iLen = 0;
    MYSQL_BIND &stData = m_pRstBinder[iField];    
    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_BLOB:
        {
            iLen =  (int)(*(stData.length));
            return (const unsigned char*)(stData.buffer);
        }
        break;

    default:
        {
            return NULL;
        }
        break;
    }

    return  (const unsigned char*)m_pRstBinder[iField].buffer;
}

const unsigned char* CMySQLStmtQuery::getBlobValue(const char* pszField, int &iLen)
{
    int iField(fieldIndex(pszField));
    return getBlobField(iField, iLen);
}


CMySQLStatement::CMySQLStatement(MYSQL *pDB, MYSQL_STMT *pStmt)
{
    m_pStmt = NULL;
    m_pBinder_Param = NULL;
    m_pMysql_Res = NULL;
    m_pField = NULL;
    m_pBindMemInfo = NULL;
    m_uiCol = H_INIT_NUMBER;
    m_pStmt = pStmt;

    //参数个数
    m_uiParamNum = (unsigned int)mysql_stmt_param_count(m_pStmt);
    if (m_uiParamNum <= 0)
    {
        return;
    }

    m_pBinder_Param = new(std::nothrow) MYSQL_BIND[m_uiParamNum];
    H_ASSERT(NULL != m_pBinder_Param, "malloc memory error.");
    H_Zero(m_pBinder_Param, m_uiParamNum * sizeof(MYSQL_BIND));

    m_pBindMemInfo = new(std::nothrow) BindMemInfo[m_uiParamNum];
    H_ASSERT(NULL != m_pBindMemInfo, "malloc memory error.");    
}

void CMySQLStatement::getFiledInfo(void)
{
    unsigned int uiFieldCond(H_INIT_NUMBER);
    m_pField = H_INIT_NUMBER;
    m_uiCol = H_INIT_NUMBER;
    m_pMysql_Res = NULL;

    //字段个数
    uiFieldCond = mysql_stmt_field_count(m_pStmt);
    if (uiFieldCond <= 0)
    {
        return;
    }

    m_pMysql_Res = mysql_stmt_result_metadata(m_pStmt);
    if (NULL != m_pMysql_Res)
    {
        m_pField = mysql_fetch_fields(m_pMysql_Res);
        m_uiCol = mysql_num_fields(m_pMysql_Res);

        mysql_free_result(m_pMysql_Res);
    }
}

CMySQLStatement::~CMySQLStatement(void)
{
    try
    {
        freeVM();
    }
    catch (...)
    {
    }
}

void CMySQLStatement::Finalize(void)
{
    delete this;
}

void CMySQLStatement::freeVM(void)
{
    if (NULL != m_pStmt)
    {
        (void)mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
    }

    H_SafeDelArray(m_pBinder_Param);
    clearParam();
    H_SafeDelArray(m_pBindMemInfo);
}

void CMySQLStatement::clearParam(void)
{
    if (NULL == m_pBindMemInfo)
    {
        return;
    }

    for (unsigned int i = 0; i < m_uiParamNum; i++)
    {
        H_SafeDelete(m_pBindMemInfo[i].pMem);
    }
}

void CMySQLStatement::checkVM(void)
{
    H_ASSERT(NULL != m_pStmt, "got null pointer.")
}

void CMySQLStatement::checkBinder(void)
{
    H_ASSERT(NULL != m_pBinder_Param, "got null pointer.")
}

void CMySQLStatement::CheckParam(int iParam)
{
    if (iParam < 0
        || iParam >= (int)m_uiParamNum)
    {
        H_ASSERT(false, CUtils::formatStr("param error. %d", iParam).c_str())
    }
}

void CMySQLStatement::BindParam(void)
{
    checkBinder();

    if (H_RTN_OK != mysql_stmt_bind_param(m_pStmt, m_pBinder_Param))
    {
        H_ASSERT(false, mysql_stmt_error(m_pStmt));
    }
}

int CMySQLStatement::execDML(void)
{
    checkVM();

    reSet();
    BindParam();

    if (H_RTN_OK != mysql_stmt_execute(m_pStmt))
    {
        H_Printf("%s", mysql_stmt_error(m_pStmt));
        return H_RTN_FAILE;
    }

    int iChangeVal((int)mysql_stmt_affected_rows(m_pStmt));
    (void)mysql_stmt_store_result(m_pStmt);
    (void)mysql_stmt_free_result(m_pStmt);

    return iChangeVal;
}

CDBQuery *CMySQLStatement::execQuery(void)
{
    checkVM();

    MYSQL_BIND *pRstBinder(NULL);
    BindInfo *pStBindInfo(NULL);

    reSet();
    BindParam();

    if (H_RTN_OK != mysql_stmt_execute(m_pStmt))
    {
        H_Printf("%s", mysql_stmt_error(m_pStmt));
        return NULL;
    }

    getFiledInfo();
    if (0 >= m_uiCol)
    {
        H_Printf("%s", "no field");
        return NULL;
    }

    pStBindInfo = new(std::nothrow) BindInfo[m_uiCol];
    H_ASSERT(NULL != pStBindInfo, "malloc memory error.");
    pRstBinder = new(std::nothrow) MYSQL_BIND[m_uiCol];
    H_ASSERT(NULL != pRstBinder, "malloc memory error.");
    H_Zero(pRstBinder, m_uiCol * sizeof(MYSQL_BIND));

    getRstBinderInfo(pRstBinder, pStBindInfo);

    if (H_RTN_OK != mysql_stmt_bind_result(m_pStmt, pRstBinder))
    {
        clearExecQuery(pRstBinder, pStBindInfo);
        H_Printf("%s", mysql_stmt_error(m_pStmt));
        return NULL;
    }
    
    if (H_RTN_OK != mysql_stmt_store_result(m_pStmt))
    {
        clearExecQuery(pRstBinder, pStBindInfo);
        H_Printf("%s", mysql_stmt_error(m_pStmt));
        return NULL;
    }    

    CDBQuery * pQuery = new(std::nothrow) CMySQLStmtQuery(pRstBinder, pStBindInfo, m_pStmt, m_uiCol);
    H_ASSERT(NULL != pQuery, "malloc memory error.");

    return pQuery;
}

void CMySQLStatement::clearExecQuery(MYSQL_BIND *pBinder, struct BindInfo *pStBindInfo)
{
    if (NULL != pBinder)
    {
        char *pTmp;

        for (unsigned int i = 0; i < m_uiCol; i++)
        {
            if (NULL != pBinder[i].length)
            {
                H_SafeDelete(pBinder[i].length);
            }
            if (NULL != pBinder[i].buffer)
            {
                pTmp = (char*)pBinder[i].buffer;
                H_SafeDelete(pTmp);
                pBinder[i].buffer = NULL;
            }
        }
    }

    H_SafeDelArray(pBinder);
    H_SafeDelArray(pStBindInfo);

    (void)mysql_stmt_free_result(m_pStmt);
}

void CMySQLStatement::getRstBinderInfo(MYSQL_BIND *pBinder, struct BindInfo *pStBindInfo)
{ 
    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        MYSQL_FIELD &stField = m_pField[i];
        MYSQL_BIND &stBinder = pBinder[i];
        BindInfo &stBindInfo = pStBindInfo[i];

        stBindInfo.iIndex = i;
        stBindInfo.strFieldName = stField.name;

        stBinder.buffer_type = stField.type;
        stBinder.buffer_length = stField.length;
        
        if (MYSQL_TYPE_TINY_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_MEDIUM_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_LONG_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_BLOB == stBinder.buffer_type)
        {
            stBinder.length = new(std::nothrow) unsigned long;
            H_ASSERT(NULL != stBinder.length, "malloc memory error.");
            *(stBinder.length) = 0;
        }

        stBinder.buffer = new(std::nothrow) char[stBinder.buffer_length];
        H_ASSERT(NULL != stBinder.buffer, "malloc memory error."); 
        H_Zero(stBinder.buffer, stBinder.buffer_length);
    }
}

void CMySQLStatement::setBindInfo(size_t iLens, struct BindMemInfo &stInfo)
{
    if (NULL == stInfo.pMem
        || iLens > stInfo.iLens)
    {
        H_SafeDelete(stInfo.pMem);
        stInfo.pMem = new(std::nothrow) char[iLens];
        H_ASSERT(NULL != stInfo.pMem, "malloc memory error.");
        stInfo.iLens = iLens;
    }
}

void CMySQLStatement::bindString(const int iField, const char* pszValue)
{
    checkVM();
    CheckParam(iField);

    size_t iLens(strlen(pszValue));
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens + 1, stBindInfo);
    memcpy(stBindInfo.pMem, pszValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_STRING;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLens;
}

void CMySQLStatement::bindJson(const int iField, const char* pszValue)
{
    checkVM();
    CheckParam(iField);

    size_t iLens(strlen(pszValue));
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens + 1, stBindInfo);
    memcpy(stBindInfo.pMem, pszValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_JSON;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLens;
}

void CMySQLStatement::bindInt(const int iField, const int iValue)
{
    checkVM();
    CheckParam(iField);

    size_t iLens(sizeof(iValue));
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &iValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_LONG;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLens;
}

void CMySQLStatement::bindInt64(const int iField, const int64_t iValue)
{
    checkVM();
    CheckParam(iField);

    size_t iLens(sizeof(iValue));
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &iValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_LONGLONG;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLens;
}

void CMySQLStatement::bindFloat(const int iField, const double dValue)
{
    checkVM();
    CheckParam(iField);

    size_t iLens(sizeof(dValue));
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &dValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_DOUBLE;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLens;
}

void CMySQLStatement::bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen)
{
    checkVM();
    CheckParam(iField);

    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLen + 1, stBindInfo);
    memcpy(stBindInfo.pMem, blobValue, iLen);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_BLOB;
    m_pBinder_Param[iField].buffer_length = (unsigned long)iLen;
}

void CMySQLStatement::bindNull(const int iField)
{
    checkVM();
    CheckParam(iField);

    m_pBinder_Param[iField].buffer = NULL;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_NULL;
    m_pBinder_Param[iField].buffer_length = 0;
}

void CMySQLStatement::reSet(void)
{
    if (NULL != m_pStmt)
    {
        if (H_RTN_OK != mysql_stmt_reset(m_pStmt))
        {
            H_Printf("%s", mysql_stmt_error(m_pStmt));
        }
    }
}


CMySQLLink::CMySQLLink(void) : m_bTrans(false), m_pDb_Ptr(NULL)
{
}

CMySQLLink::~CMySQLLink(void)
{
    Close();
}

void CMySQLLink::Close(void)
{
    if (m_pDb_Ptr != NULL)
    {
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;
    }
}

int CMySQLLink::Open(const char *pszHost, unsigned short usPort, const char *pszUser, const char *pszPSW, const char *pszDB)
{    
    m_pDb_Ptr = mysql_init(NULL);
    if (NULL == m_pDb_Ptr) 
    {
        return H_RTN_FAILE;
    }

    //set re-conn to true. could use ping to reconn
    char cReConnect(1);
    if (H_RTN_OK != mysql_options(m_pDb_Ptr, MYSQL_OPT_RECONNECT, &cReConnect))
    {
        H_Printf("%s", mysql_error(m_pDb_Ptr));
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;

        return H_RTN_FAILE;
    }

    //如果连接失败，返回NULL。对于成功的连接，返回值与第1个参数的值相同。
    if (NULL == mysql_real_connect(m_pDb_Ptr, 
        pszHost, pszUser,
        pszPSW, pszDB,
        usPort, NULL, CLIENT_MULTI_STATEMENTS ))
    {
        H_Printf("%s", mysql_error(m_pDb_Ptr));
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;

        return H_RTN_FAILE;
    }

    //选择制定的数据库失败
    //0表示成功，非0值表示出现错误。
    if (H_RTN_OK != mysql_select_db( m_pDb_Ptr, pszDB))
    {
        H_Printf("%s", mysql_error(m_pDb_Ptr));
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;

        return H_RTN_FAILE;
    }
    
    return H_RTN_OK;
}

CDBQuery* CMySQLLink::execQuery(const char* szSQL)
{
    checkDB();

    if (H_RTN_OK == mysql_real_query(m_pDb_Ptr, szSQL, (unsigned long)strlen(szSQL)))
    {
        MYSQL_RES *Mysql_Res(mysql_store_result(m_pDb_Ptr));
        if (NULL == Mysql_Res)
        {
            H_Printf("%s", mysql_error(m_pDb_Ptr));
            return NULL;
        }

        CDBQuery *pQuery = new(std::nothrow) CMySQLQuery(Mysql_Res);
        H_ASSERT(NULL != pQuery, "malloc memory error.");

        return pQuery;
    }

    H_Printf("%s", mysql_error(m_pDb_Ptr));

    return NULL;
}

CMySQLStatement *CMySQLLink::compileStatement(const char* pszSQL)
{
    checkDB();

    MYSQL_STMT *pStmt(mysql_stmt_init(m_pDb_Ptr));
    if (NULL == pStmt)
    {
        H_Printf("%s", mysql_error(m_pDb_Ptr));
        return NULL;
    }
    
    if (H_RTN_OK == mysql_stmt_prepare(pStmt, pszSQL, (unsigned long)strlen(pszSQL)))
    {
        CMySQLStatement *pStatement = new(std::nothrow) CMySQLStatement(m_pDb_Ptr, pStmt);
        H_ASSERT(NULL != pStatement, "malloc memory error.");

        return pStatement;
    }

    H_Printf("%s", mysql_stmt_error(pStmt));
    mysql_stmt_close(pStmt);

    return NULL;
}

int CMySQLLink::execDML(const char* szSQL)
{
    checkDB();

    int iRtn(mysql_real_query(m_pDb_Ptr, szSQL, (unsigned long)strlen(szSQL)));    
    if (H_RTN_OK == iRtn)
    {
        //得到受影响的行数
        iRtn = (int)mysql_affected_rows(m_pDb_Ptr);
        MYSQL_RES *Mysql_Res(mysql_store_result(m_pDb_Ptr));
        if(NULL != Mysql_Res)
        {
            mysql_free_result(Mysql_Res);
        }

        return iRtn;
    }

    //执行查询失败
    H_Printf("%s", mysql_error(m_pDb_Ptr));

    return H_RTN_FAILE;
}

int CMySQLLink::reLink(void)
{
    if (NULL == m_pDb_Ptr)
    {
        return H_RTN_FAILE;
    }

    return mysql_ping(m_pDb_Ptr);
}

void CMySQLLink::Trans(void)
{
    m_bTrans = true;
    (void)execDML("START TRANSACTION");
}

void CMySQLLink::Commit(void)
{
    (void)execDML("COMMIT");
    m_bTrans = false;
}

void CMySQLLink::rollBack(void)
{
    (void)execDML("ROLLBACK");
    m_bTrans = false;
}

void CMySQLLink::setBusyTimeout(int nMillisecs)
{
    checkDB();
    m_pDb_Ptr->options.read_timeout = nMillisecs;
    m_pDb_Ptr->options.write_timeout = nMillisecs;
    m_pDb_Ptr->options.connect_timeout = nMillisecs;
}

void CMySQLLink::checkDB(void)
{
    H_ASSERT(NULL != m_pDb_Ptr, "got null pointer.");
}

#endif

H_ENAMSP
