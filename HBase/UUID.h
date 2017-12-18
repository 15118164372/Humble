
#ifndef H_UUID_H_
#define H_UUID_H_

#include "Macros.h"

H_BNAMSP

#ifdef H_UUID

#ifndef H_OS_WIN
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID, UUID;
#endif

/*UUID
yum install libuuid-devel
*/
class CUUID
{
public:
    CUUID(void);
    ~CUUID(void);

    /*��ȡһUUID*/
    std::string getUUID(void);

private:
    H_DISALLOWCOPY(CUUID);
    int createGuid(GUID &stUUID);
};

#endif//H_UUID

H_ENAMSP

#endif//H_UUID_H_
