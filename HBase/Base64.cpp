
#include "Base64.h"

H_BNAMSP

static const  char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const int decoding[] = { 62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,
    -1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
    -1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51 };

std::string H_B64Encode(const char *pszData, const size_t iLens)
{
    int i(0);
    uint32_t v(0);
    std::string ret;
    unsigned char *pText = (unsigned char *)pszData;

    for (i = 0; i < (int)iLens - 2; i += 3) 
    {
        v = pText[i] << 16 | pText[i + 1] << 8 | pText[i + 2];
        ret += encoding[v >> 18];
        ret += encoding[(v >> 12) & 0x3f];
        ret += encoding[(v >> 6) & 0x3f];
        ret += encoding[(v) & 0x3f];
    }

    int padding((int)iLens - i);
    switch (padding)
    {
        case 1:
            v = pText[i];
            ret += encoding[v >> 2];
            ret += encoding[(v & 3) << 4];
            ret += '=';
            ret += '=';
            break;

        case 2:
            v = pText[i] << 8 | pText[i + 1];
            ret += encoding[v >> 10];
            ret += encoding[(v >> 4) & 0x3f];
            ret += encoding[(v & 0xf) << 2];
            ret += '=';
            break;
    }

    return ret;
}

static inline int b64index(uint8_t c)
{
    int decoding_size = sizeof(decoding) / sizeof(decoding[0]);
    if (c < 43)
    {
        return -1;
    }

    c -= 43;
    if (c >= decoding_size)
    {
        return -1;
    }

    return decoding[c];
}

std::string H_B64Decode(const char *pszData, const size_t iLens)
{
    int c[4];
    int j(0), padding(0);
    std::string ret;
    uint32_t v(0);
    unsigned char *pText = (unsigned char *)pszData;

    for (int i = 0; i < (int)iLens;) 
    {
        padding = 0;
        for (j = 0; j < 4;) 
        {
            if (i >= (int)iLens)
            {
                return "";
            }

            c[j] = b64index(pText[i]);
            if (c[j] == -1) 
            {
                ++i;
                continue;
            }

            if (c[j] == -2) 
            {
                ++padding;
            }
            ++i;
            ++j;
        }
        
        switch (padding)
        {
            case 0:
                v = (unsigned)c[0] << 18 | c[1] << 12 | c[2] << 6 | c[3];
                ret += v >> 16;
                ret += (v >> 8) & 0xff;
                ret += v & 0xff;
                break;

            case 1:
                if (c[3] != -2 || (c[2] & 3) != 0)
                {
                    return "";
                }

                v = (unsigned)c[0] << 10 | c[1] << 4 | c[2] >> 2;
                ret += v >> 8;
                ret += v & 0xff;
                break;

            case 2:
                if (c[3] != -2 || c[2] != -2 || (c[1] & 0xf) != 0) 
                {
                    return "";
                }

                v = (unsigned)c[0] << 2 | c[1] >> 4;
                ret += v;
                break;

            default:
                return "";
        }
    }

    return ret;
}

H_ENAMSP
