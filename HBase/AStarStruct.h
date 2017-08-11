
#ifndef H_ASTARSTRUCT_H_
#define H_ASTARSTRUCT_H_

#include "Funcs.h"

H_BNAMSP

//µã
struct PointF
{
    float fX;
    float fY;
};

struct PointI
{
    int iX;
    int iY;
    bool operator == (const PointI &stPoint) const
    {
        return  iX == stPoint.iX && iY == stPoint.iY;
    }
    std::size_t Hash() const
    {
        return (H_HashNumber(iX) ^ (H_HashNumber(iY) << 1)) >> 1;
    };
};
struct PointIEq
{
    bool operator () (const PointI &stSrc, const PointI &stTag) const
    {
        return stSrc.iX == stTag.iX && stSrc.iY == stTag.iY;
    }
    bool operator () (const PointI *pSrc, const PointI *pTag) const
    {
        return pSrc->iX == pTag->iX && pSrc->iY == pTag->iY;
    }
};
struct PointIHash
{
    std::size_t operator () (const PointI &stPoint) const
    {
        return stPoint.Hash();
    }
    std::size_t operator () (const PointI *pPoint) const
    {
        return pPoint->Hash();
    }
};

//Â·¾¶
struct APath
{
    PointI stPoint;
    int iWeight;
    int iFillWeight;
    int iDistTraveled;

    APath *pParent;
};
struct APathWeightCMP
{
    bool operator () (const APath *pSrc, const APath *pTag)
    {
        return pSrc->iWeight > pTag->iWeight;
    }
    bool operator () (const APath &stSrc, const APath &stTag)
    {
        return stSrc.iWeight > stTag.iWeight;
    }
};

H_ENAMSP

#endif//H_ASTARSTRUCT_H_
