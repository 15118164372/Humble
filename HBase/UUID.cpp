
#include "UUID.h"

H_BNAMSP

#ifdef H_UUID

CUUID::CUUID(void)
{
#ifdef H_OS_WIN
    (void)CoInitialize(NULL);
#endif
}

CUUID::~CUUID(void)
{
#ifdef H_OS_WIN
    CoUninitialize();
#endif
}

int CUUID::createGuid(GUID &stUUID)
{
#ifdef H_OS_WIN
    if (S_OK != CoCreateGuid(&stUUID))
    {
        return H_RTN_FAILE;
    }
#else
    uuid_generate(reinterpret_cast<unsigned char *>(&stUUID));
#endif

    return H_RTN_OK;
}

std::string CUUID::getUUID(void)
{
    GUID stGuid;
    if (H_RTN_OK != createGuid(stGuid))
    {
        return "";
    }
    
    char acUUID[H_UUIDLENS] = {0};
    H_Snprintf(acUUID,
        sizeof(acUUID) - 1,
        "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
        (unsigned int)stGuid.Data1, stGuid.Data2, stGuid.Data3,
        stGuid.Data4[0], stGuid.Data4[1],
        stGuid.Data4[2], stGuid.Data4[3],
        stGuid.Data4[4], stGuid.Data4[5],
        stGuid.Data4[6], stGuid.Data4[7]);

    return std::string(acUUID);
}

#endif//H_UUID

H_ENAMSP
