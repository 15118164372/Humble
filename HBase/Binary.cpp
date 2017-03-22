
#include "Binary.h"
#include "Funcs.h"

H_BNAMSP

#define H_LINEEFLAG "\r\n"

CBinary::CBinary(void) : m_pParseBuffer(NULL), m_pTmpBuf(NULL),
    m_iParseBufLens(H_INIT_NUMBER), m_iTmpBufLens(H_INIT_NUMBER), m_iCurParseLens(H_INIT_NUMBER),
    m_iTmpParseLens(H_INIT_NUMBER)
{
    H_Zero(m_acZero, sizeof(m_acZero));
    m_iLEFLens = strlen(H_LINEEFLAG);
}

CBinary::~CBinary(void)
{
    m_pParseBuffer = NULL;
}

void CBinary::setReadBuffer(const char *pszBuf, const size_t iLens)
{
    m_pParseBuffer = (char*)pszBuf;
    m_iParseBufLens = iLens;
    m_iCurParseLens = H_INIT_NUMBER;
}

void CBinary::reSetWrite(void)
{
    m_strWritBuffer.clear();
}

void CBinary::skipRead(const size_t iLens)
{
    m_iCurParseLens += iLens;
}

void CBinary::skipWrite(const size_t iLens)
{
    if (iLens > sizeof(m_acZero))
    {
        H_Printf("skip write lens %d must less than %d.", (int)iLens, (int)sizeof(m_acZero));
        return;
    }

    setVal(m_acZero, iLens);
}

size_t CBinary::getSurpLens(void)
{
    return m_iParseBufLens - m_iCurParseLens;
}

void CBinary::setVal(const void *pszBuf, const size_t iLens)
{
    if (H_INIT_NUMBER == iLens
        || NULL == pszBuf)
    {
        return;
    }

    m_strWritBuffer.append((const char*)pszBuf, iLens);   
}

void CBinary::setSint8(char cVal)
{
    setVal(&cVal, sizeof(cVal));
}
char CBinary::getSint8(void)
{
    return readNumber<char>();
}
void CBinary::setUint8(unsigned char ucVal)
{
    setVal(&ucVal, sizeof(ucVal));
}
unsigned char CBinary::getUint8(void)
{
    return readNumber<unsigned char>();
}

void CBinary::setBool(const bool bVal)
{
    unsigned char ucVal((bVal ? 1 : 0));

    setVal(&ucVal, sizeof(ucVal));
}
bool CBinary::getBool(void)
{
    return ((H_INIT_NUMBER == readNumber<unsigned char>()) ? false : true);
}

void CBinary::setSint16(short sVal)
{
    sVal = ntohs(sVal);

    setVal(&sVal, sizeof(sVal));
}
short CBinary::getSint16(void)
{
    return ntohs(readNumber<short>());
}
void CBinary::setUint16(unsigned short usVal)
{
    usVal = ntohs(usVal);

    setVal(&usVal, sizeof(usVal));
}
unsigned short CBinary::getUint16(void)
{
    return ntohs(readNumber<unsigned short>());
}

void CBinary::setSint32(int iVal)
{
    iVal = ntohl(iVal);

    setVal(&iVal, sizeof(iVal));
}
int CBinary::getSint32(void)
{
    return ntohl(readNumber<int>());
}
void CBinary::setUint32(unsigned int uiVal)
{
    uiVal = ntohl(uiVal);

    setVal(&uiVal, sizeof(uiVal));
}
unsigned int CBinary::getUint32(void)
{
    return ntohl(readNumber<unsigned int>());
}

void CBinary::setSint64(int64_t iVal)
{
    iVal = ntohl64(iVal);

    setVal(&iVal, sizeof(iVal));
}
int64_t CBinary::getSint64(void)
{
    return ntohl64(readNumber<int64_t>());
}
void CBinary::setUint64(uint64_t uiVal)
{
    uiVal = ntohl64(uiVal);

    setVal(&uiVal, sizeof(uiVal));
}
uint64_t CBinary::getUint64(void)
{
    return ntohl64(readNumber<uint64_t>());
}

void CBinary::setDouble(const double dVal)
{
    setVal(&dVal, sizeof(dVal));
}
double CBinary::getDouble(void)
{
    return readNumber<double>();
}

void CBinary::setFloat(const float fVal)
{
    setVal(&fVal, sizeof(fVal));
}
float CBinary::getFloat(void)
{
    return readNumber<float>();
}

void CBinary::setString(const char *pszVal)
{
    setVal(pszVal, strlen(pszVal));
    skipWrite(1);
}
luabridge::H_LBinary& CBinary::getString(void)
{
    m_stVal.iLens = H_INIT_NUMBER;
    m_stVal.pBufer = NULL;

    //³¬³¤
    if (m_iCurParseLens >= m_iParseBufLens)
    {
        return m_stVal;
    }

    char *pBuf = m_pParseBuffer + m_iCurParseLens;
    size_t iLens(strlen(pBuf));

    m_stVal.pBufer = pBuf;
    m_stVal.iLens = iLens;

    //³¬³¤
    if (m_iCurParseLens + iLens + 1 > m_iParseBufLens)
    {
        m_iCurParseLens = m_iParseBufLens;
        m_stVal.iLens = getSurpLens();

        return m_stVal;
    }

    m_iCurParseLens += (iLens + 1);

    return m_stVal;
}

void CBinary::setByte(const char *pszVal, const unsigned int iLens)
{
    setVal(pszVal, iLens);
}

const char *CBinary::getByte(const unsigned int &iLens)
{
    if ((m_iCurParseLens + iLens) > m_iParseBufLens)
    {
        return NULL;
    }

    if (H_INIT_NUMBER == iLens)
    {
        return NULL;
    }

    char *pBuf = m_pParseBuffer + m_iCurParseLens;

    m_iCurParseLens += iLens;

    return pBuf;
}

luabridge::H_LBinary& CBinary::getLByte(const unsigned int iLens)
{
    m_stVal.iLens = H_INIT_NUMBER;
    m_stVal.pBufer = NULL;

    const char *pBuf = getByte(iLens);
    if (NULL != pBuf)
    {
        m_stVal.pBufer = (char*)pBuf;
        m_stVal.iLens = iLens;
    }

    return m_stVal;
}

luabridge::H_LBinary& CBinary::readLine(void)
{
    m_stVal.iLens = H_INIT_NUMBER;
    m_stVal.pBufer = NULL;

    if (m_iCurParseLens >= m_iParseBufLens)
    {
        return m_stVal;
    }

    char *pBuf = m_pParseBuffer + m_iCurParseLens;
    char *pPos = strstr(pBuf, H_LINEEFLAG);
    if (NULL == pPos)
    {
        m_stVal.pBufer = pBuf;
        m_stVal.iLens = getSurpLens();
        m_iCurParseLens = m_iParseBufLens;

        return m_stVal;
    }

    m_stVal.pBufer = pBuf;
    m_stVal.iLens = pPos - pBuf;

    m_iCurParseLens += (pPos - pBuf + m_iLEFLens);

    return m_stVal;
}

int CBinary::Find(const char *pFlag)
{
    const char *pBuf = m_pParseBuffer + m_iCurParseLens;
    const char *pPos = H_StrStr(pBuf, pFlag);
    if (NULL == pPos)
    {
        return -1;
    }

    return (int)(pPos - pBuf);
}

H_ENAMSP
