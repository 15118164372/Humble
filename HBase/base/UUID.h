
#ifndef H_UUID_H_
#define H_UUID_H_

#include "HObject.h"

H_BNAMSP

#ifdef H_UUID

/*UUID
yum install libuuid-devel
*/
class CUUID : public CObject
{
public:
    CUUID(void);
    ~CUUID(void);

    /*ªÒ»°“ªUUID*/
    std::string getUUID(void);

private:
    int createGuid(struct GUID *stUUID);
};

#endif//H_UUID

H_ENAMSP

#endif//H_UUID_H_
