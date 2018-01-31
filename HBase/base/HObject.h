
#ifndef H_OBJECT_H_
#define H_OBJECT_H_

#include "Macros.h"

H_BNAMSP

//»ùÀà

class CObject
{
public:
    CObject(void) {};
    virtual ~CObject(void) {};

    virtual std::string toString(void)
    {
        return "";
    };

private:
    H_DISALLOWCOPY(CObject);
};

H_ENAMSP

#endif//H_OBJECT_H_
