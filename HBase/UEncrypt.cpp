
#include "UEncrypt.h"

H_BNAMSP

static unsigned char toHex(unsigned char x)
{
    return  ((x > 9) ? (x + 55) : (x + 48));
}

static unsigned char fromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') 
    {
        y = x - 'A' + 10;  
    }
    else if (x >= 'a' && x <= 'z') 
    {
        y = x - 'a' + 10;  
    }
    else if (x >= '0' && x <= '9') 
    {
        y = x - '0';  
    }
    else 
    {
        H_ASSERT(false, "");  
    }

    return y;  
}

std::string H_UEncode(const char *pszVal, const size_t iLens)
{
    std::string strTemp;

    for (size_t i = 0; i < iLens; i++)  
    {  
        if (isalnum((unsigned char)pszVal[i]) 
            || ('-' == pszVal[i]) 
            || ('_' == pszVal[i]) 
            || ('.' == pszVal[i]) 
            || ('~' == pszVal[i]))
        {
            strTemp += pszVal[i]; 
        }
        else if (' ' == pszVal[i])
        {
            strTemp += "+";  
        }
        else  
        {  
            strTemp += '%';
            strTemp += toHex((unsigned char)pszVal[i] >> 4);  
            strTemp += toHex((unsigned char)pszVal[i] % 16);  
        }  
    }

    return strTemp; 
}

std::string H_UDecode(const char *pszVal, const size_t iLens)
{
    std::string strTemp;

    for (size_t i = 0; i < iLens; i++)  
    {  
        if ('+' == pszVal[i]) 
        {
            strTemp += ' ';  
        }
        else if ('%' == pszVal[i])  
        {
            assert(i + 2 < iLens);

            unsigned char cHigh = fromHex((unsigned char)pszVal[++i]);  
            unsigned char cLow = fromHex((unsigned char)pszVal[++i]); 

            strTemp += cHigh*16 + cLow;
        }  
        else 
        {
            strTemp += pszVal[i];  
        }
    }

    return strTemp; 
}

H_ENAMSP
