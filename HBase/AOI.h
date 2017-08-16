
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

    bool Enter(const int64_t iId, const int iX, const int iY, const int iZ);
    void Leave(const int64_t iId);
    bool Move(const int64_t &iId, const int &iX, const int &iY, const int &iZ,
        const int &iXDist, const int &iYDist, const int &iZDist,
        std::vector<int64_t> &outArea, std::vector<int64_t> &newArea);
    bool onlyMove(const int64_t iId, const int iX, const int iY, const int iZ);
    bool getArea(const int64_t &iId, const int &iXDist, const int &iYDist, const int &iZDist, std::vector<int64_t> &vcArea);

private:
    CAOI(void);
    H_DISALLOWCOPY(CAOI);
#ifdef H_OS_WIN
    #define positionit std::unordered_map<int64_t , Position *>::iterator
    #define position_map std::unordered_map<int64_t , Position *>
#else
    #define positionit std::tr1::unordered_map<int64_t , Position *>::iterator
    #define position_map std::tr1::unordered_map<int64_t , Position *>
#endif
    struct Position
    {
        int X;
        int Y;
        int Z;
        Position(void) : X(H_INIT_NUMBER), Y(H_INIT_NUMBER), Z(H_INIT_NUMBER)
        {};
    };

    void calArea(Position *pPos, const int &iXDist, const int &iYDist, const int &iZDist, std::vector<int64_t> &vcArea);
    bool checkPos(const int &iX, const int &iY, const int &iZ);
    void moveData(const int64_t &iId, Position *pPos, const int &iX, const int &iY, const int &iZ);
    void calOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd, 
        const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea);
    void addVector(std::vector<int64_t> &vcArea, const int64_t &iId);
    void calPosArea(Position &stStart, Position &stEnd, Position *pPos, 
        const int &iXDist, const int &iYDist, const int &iZDist);

    void calXOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
        const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea);
    void calXOutArea(Position &stOldStart, Position &stOldEnd, 
        position_map &mapPos, const int64_t &iId,std::vector<int64_t> &vcArea);
    void calXInArea(Position &stStart, Position &stEnd, 
        position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea);

    void calYOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
        const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea);
    void calYOutArea(Position &stOldStart, Position &stOldEnd, 
        position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea);
    void calYInArea(Position &stStart, Position &stEnd, 
        position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea);

    void calZOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
        const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea);
    void calZOutArea(Position &stOldStart, Position &stOldEnd, 
        position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea);
    void calZInArea(Position &stStart, Position &stEnd, 
        position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea);

private:
    int m_iMaxX;
    int m_iMaxY;
    int m_iMaxZ;
    std::vector<position_map> m_vcXList;
    std::vector<position_map> m_vcYList;
    std::vector<position_map> m_vcZList;
    position_map m_mapPos;
};

H_ENAMSP

#endif//H_AOI_H_
