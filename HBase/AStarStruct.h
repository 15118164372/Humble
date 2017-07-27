
#ifndef H_ASTARSTRUCT_H_
#define H_ASTARSTRUCT_H_

#include "Macros.h"

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
        return (hashInt(iX) ^ (hashInt(iY) << 1)) >> 1;
    };

private:
    inline std::size_t hashInt(const int &iVal) const
    {
#if defined(_WIN64)
        #define OFFSET_BASIS 14695981039346656037ULL
        #define PRIME 1099511628211ULL
#else
        #define OFFSET_BASIS 2166136261U
        #define PRIME 16777619U
#endif
        size_t iHash(OFFSET_BASIS);
        unsigned char *pAddr((unsigned char *)&iVal);
        for (size_t i = 0; i < sizeof(iVal); ++i)
        {
            iHash ^= (size_t)pAddr[i];
            iHash *= PRIME;
        }

        return iHash;
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
