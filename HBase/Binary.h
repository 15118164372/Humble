
#ifndef H_LUABINARY_H_
#define H_LUABINARY_H_

#include "HStruct.h"

H_BNAMSP

//二进制解析
class CBinary
{
public:
    CBinary(void);
    ~CBinary(void);

    //设置要解析的数据
    void setReadBuffer(const char *pszBuf, const size_t iLens);
    size_t getRBufLens(void)
    {
        return m_iParseBufLens;
    };
    size_t getReadedLens(void)
    {
        return m_iCurParseLens;
    };

    //重置写
    void reSetWrite(void);
    //读  跳过指定字节
    void skipRead(const size_t iLens);
    //写  跳过指定字节跳过字节以0填充
    void skipWrite(const size_t iLens);

    //获取尚未解析的字节数
    size_t getSurpLens(void);

    //char
    void setSint8(char cVal);
    char getSint8(void);
    void setUint8(unsigned char ucVal);
    unsigned char getUint8(void);

    //bool
    void setBool(const bool bVal);
    bool getBool(void);

    //short
    void setSint16(short sVal);
    short getSint16(void);
    void setUint16(unsigned short usVal);
    unsigned short getUint16(void);

    //int
    void setSint32(int iVal);
    int getSint32(void);
    void setUint32(unsigned int uiVal);
    unsigned int getUint32(void);

    //int64_t
    void setSint64(int64_t iVal);
    int64_t getSint64(void);
    void setUint64(uint64_t uiVal);
    uint64_t getUint64(void);

    //double
    void setDouble(const double dVal);
    double getDouble(void);

    //float
    void setFloat(const float fVal);
    float getFloat(void);

    //string 保证以/0结束
    void setString(const char *pszVal);
    H_Binary& getString(void);

    //byte
    void setByte(const char *pszVal, const unsigned int iLens);
    H_Binary& getLByte(const unsigned int iLens);
    const char *getByte(const unsigned int &iLens);

    H_Binary& readLine(void);

    int Find(const char *pFlag);

    std::string& getWritedBuf(void)
    {
        return m_strWritBuffer;
    };
    void setW2R(void)
    {
        m_pTmpBuf = m_pParseBuffer;
        m_iTmpBufLens = m_iParseBufLens;
        m_iTmpParseLens = m_iCurParseLens;

        setReadBuffer(m_strWritBuffer.c_str(), m_strWritBuffer.size());
    };
    void resetW2R(void)
    {
        setReadBuffer(m_pTmpBuf, m_iTmpBufLens);
        m_iCurParseLens = m_iTmpParseLens;
    };
    void Append(CBinary *pBinary)
    {
        m_strWritBuffer.append(pBinary->getWritedBuf().c_str(), pBinary->getWritedBuf().size());
    };

private:
    H_DISALLOWCOPY(CBinary);
    void setVal(const void *pszBuf, const size_t iLens);
    template<typename T>
    T readNumber(void)
    {
        if ((m_iCurParseLens + sizeof(T)) > m_iParseBufLens)
        {
            H_Printf("%s", "read number error.");
            return H_INIT_NUMBER;
        }

        T varVal(*((T*)(m_pParseBuffer + m_iCurParseLens)));
        m_iCurParseLens += sizeof(T);

        return varVal;
    };

private:
    char *m_pParseBuffer;//要解析的buffer
    char *m_pTmpBuf;
    char m_acZero[H_ONEK / 8];
    size_t m_iParseBufLens;//要解析的buffer长度
    size_t m_iTmpBufLens;
    size_t m_iCurParseLens;//已经解析的长度
    size_t m_iTmpParseLens;
    size_t m_iLEFLens;
    std::string m_strWritBuffer;//写buffer
    H_Binary m_stVal;
};

H_ENAMSP

#endif//H_LUABINARY_H_
