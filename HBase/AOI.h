
#ifndef H_AOI_H_
#define H_AOI_H_

#include "Macros.h"

H_BNAMSP

// ”“∞«¯”Ú
class CAOI
{
public:
    CAOI(int iMaxX, int iMaxY, int iMaxZ);
    virtual ~CAOI(void);

    void Enter(const int64_t iId, const int iX, const int iY, const int iZ);
    void Leave(const int64_t iId);
    void Move(const int64_t iId, const int iX, const int iY, const int iZ);
    std::vector<int64_t> getArea(const int64_t &iId, const int &iXDist, const int &iYDist, const int &iZDist);

private:
    CAOI(void);
    H_DISALLOWCOPY(CAOI);

    struct Position
    {
        int X;
        int Y;
        int Z;
        Position(void) : X(H_INIT_NUMBER), Y(H_INIT_NUMBER), Z(H_INIT_NUMBER)
        {};
    };

    std::vector<int64_t> calArea(Position *pPos, const int &iXDist, const int &iYDist, const int &iZDist);
    bool checkPos(const int &iX, const int &iY, const int &iZ);

private:
    int m_iMaxX;
    int m_iMaxY;
    int m_iMaxZ;
#ifdef H_OS_WIN
    #define positionit std::unordered_map<int64_t , Position *>::iterator
    #define position_map std::unordered_map<int64_t , Position *>
#else
    #define positionit std::tr1::unordered_map<int64_t , Position *>::iterator
    #define position_map std::tr1::unordered_map<int64_t , Position *>
#endif
    std::vector<position_map> m_vcXList;
    position_map m_mapPos;
};

H_ENAMSP

#endif//H_AOI_H_
