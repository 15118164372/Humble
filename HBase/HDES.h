
#ifndef H_HDES_H_
#define H_HDES_H_

#include "Macros.h"
#include "RSAEuro/rsaeuro.h"
#include "RSAEuro/des.h"

H_BNAMSP

enum 
{
    DESTYPE_DES = 0,
    DESTYPE_D2DES,
    DESTYPE_D3DES,
};

class CDESEncode
{
public:
    CDESEncode(const unsigned short usType);
    ~CDESEncode(void);

private:
    CDESEncode(void);
    H_DISALLOWCOPY(CDESEncode);

private:
    unsigned short m_usType;
};

class CDESDecode
{
public:
    CDESDecode(const unsigned short usType);
    ~CDESDecode(void);

private:
    CDESDecode(void);
    H_DISALLOWCOPY(CDESDecode);

private:
    unsigned short m_usType;
};

H_ENAMSP

#endif//H_HDES_H_
