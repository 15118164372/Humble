
#include "Base64.h"

H_BNAMSP

static const char *g_pCodes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static const unsigned char g_pMap[256] =
{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
    255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
    7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
    19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255
};

std::string H_B64Encode(const char *pszData, const size_t iLens)
{
    size_t i;
    std::string strOut;
    unsigned char *pIn = (unsigned char *)pszData;

    for (i = 0; i < 3 * (iLens / 3); i += 3)
    {
        strOut += g_pCodes[pIn[0] >> 2];
        strOut += g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
        strOut += g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
        strOut += g_pCodes[pIn[2] & 0x3f];
        pIn += 3;
    }

    if (i < iLens)
    {
        unsigned char a = pIn[0];
        unsigned char b = ((i + 1) < iLens) ? pIn[1] : 0;
        unsigned char c = 0;

        strOut += g_pCodes[a >> 2];
        strOut += g_pCodes[((a & 3) << 4) + (b >> 4)];
        strOut += ((i + 1) < iLens) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
        strOut += '=';
    }

    return strOut;
}

std::string H_B64Decode(const char *pszData, const size_t iLens)
{
    size_t t, x, y, z;
    unsigned char c;
    size_t g(3);
    std::string strOut;
    unsigned char *pIn = (unsigned char *)pszData;

    for (x = y = z = t = 0; x < iLens; x++)
    {
        c = g_pMap[pIn[x]];
        if (255 == c)
        {
            continue;
        }
            
        if (254 == c)
        {
            c = 0;
            g--; 
        }

        t = (t << 6) | c;

        if (4 == ++y)
        {
            strOut += (t >> 16) & 255;
            if (g > 1)
            {
                strOut += (t >> 8) & 255; 
            }
            if (g > 2) 
            { 
                strOut += t & 255;
            }

            y = t = 0;
        }
    }

    return strOut;
}

H_ENAMSP
