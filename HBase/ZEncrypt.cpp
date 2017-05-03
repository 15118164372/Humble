
#include "ZEncrypt.h"
#include "zlib/zutil.h"
#include "zlib/zlib.h"

H_BNAMSP

std::string H_ZEncode(const char *pszVal, const size_t iLens)
{
    int iErr(Z_OK);
    size_t iCount(H_INIT_NUMBER);
    char acBuffer[H_ONEK];
    std::string strRtn;
    z_stream stStream;

    stStream.next_in = (Byte *)pszVal;
    stStream.avail_in = (uInt)iLens;
    stStream.next_out = (Byte *)acBuffer;
    stStream.avail_out = (uInt)sizeof(acBuffer);
    stStream.zalloc = (alloc_func)Z_NULL;
    stStream.zfree = (free_func)Z_NULL;

    iErr = deflateInit2(&stStream, Z_DEFAULT_COMPRESSION,
        Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (Z_OK != iErr)
    {
        H_Printf("deflateInit2 error, code %d.", iErr);
        (void)deflateEnd(&stStream);

        return "";
    }

    do
    {
        iErr = deflate(&stStream, Z_FINISH);
        switch (iErr)
        {
            case(Z_STREAM_END):
            {
                strRtn.append(acBuffer, (stStream.total_out - iCount));
                iCount = stStream.total_out;
            }
            break;
            case(Z_BUF_ERROR):
            {
                if (stStream.avail_out > 0)
                {
                    H_Printf("deflate error, code %d.", iErr);
                    (void)deflateEnd(&stStream);
                    return "";
                }
            }
            case(Z_OK):
            {
                strRtn.append(acBuffer, (stStream.total_out - iCount));
                stStream.next_out = (unsigned char *)acBuffer;
                stStream.avail_out = (uInt)sizeof(acBuffer);
                iCount = stStream.total_out;
            }
            break;
            default:
            {
                H_Printf("deflate error, code %d.", iErr);
                (void)deflateEnd(&stStream);
                return "";
            }
            break;
        }
    } while (Z_STREAM_END != iErr);

    (void)deflateEnd(&stStream);

    return strRtn;
}

std::string H_ZDecode(const char *pszVal, const size_t iLens)
{
    int iErr(Z_OK);
    size_t iCount(H_INIT_NUMBER);
    char acBuffer[H_ONEK];
    std::string strRtn;
    z_stream stStream;

    stStream.next_in = (Byte *)pszVal;
    stStream.avail_in = (uInt)iLens;
    stStream.next_out = (Byte *)acBuffer;
    stStream.avail_out = (uInt)sizeof(acBuffer);
    stStream.zalloc = (alloc_func)Z_NULL;
    stStream.zfree = (free_func)Z_NULL;

    iErr = inflateInit2(&stStream, DEF_WBITS);
    if (Z_OK != iErr)
    {
        H_Printf("inflateInit2 error, code %d.", iErr);
        (void)inflateEnd(&stStream);

        return "";
    }

    do
    {
        iErr = inflate(&stStream, Z_FINISH);
        switch (iErr)
        {
            case(Z_STREAM_END):
            {
                strRtn.append(acBuffer, (stStream.total_out - iCount));
                iCount = stStream.total_out;
            }
            break;
            case(Z_BUF_ERROR):
            {
                if (stStream.avail_out > 0)
                {
                    H_Printf("inflate error, code %d.", iErr);
                    (void)inflateEnd(&stStream);
                    return "";
                }
            }
            case(Z_OK):
            {
                strRtn.append(acBuffer, (stStream.total_out - iCount));
                stStream.next_out = (unsigned char *)acBuffer;
                stStream.avail_out = (uInt)sizeof(acBuffer);
                iCount = stStream.total_out;
            }
            break;
            default:
            {
                H_Printf("inflate error, code %d.", iErr);
                (void)inflateEnd(&stStream);
                return "";
            }
            break;
        }
    } while (Z_STREAM_END != iErr);

    (void)inflateEnd(&stStream);

    return strRtn;
}

H_ENAMSP
