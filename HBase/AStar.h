
#ifndef H_ASTAR_H_
#define H_ASTAR_H_

#include "AStarStruct.h"

H_BNAMSP

//a*地图数据 A*未考虑半径，在地图填充时将半径加入
class CAMap
{
public:
    CAMap(int maxX, int maxY);
    ~CAMap(void);

    void Filled(int iX, int iY, int iWeight);

    friend class CAStar;

protected:
    int filledTiles(PointI &stPoint);
    bool canMove(int iX, int iY);
    bool canMove(PointI &stPoint);

private:
    CAMap(void);
    H_DISALLOWCOPY(CAMap);

protected:
    int m_maxX;
    int m_maxY;

private:
#ifdef H_OS_WIN
    #define filledit std::unordered_map<PointI, int, PointIHash, PointIEq>::iterator
    #define filled_map std::unordered_map<PointI, int, PointIHash, PointIEq>
#else
    #define filledit std::tr1::unordered_map<PointI, int, PointIHash, PointIEq>::iterator
    #define filled_map std::tr1::unordered_map<PointI, int, PointIHash, PointIEq>
#endif
    filled_map m_mapFilled;
};

//a*
class CAStar
{
public:
    CAStar(void);
    virtual ~CAStar(void);

    void setSmooth(const bool bSmooth) 
    {
        m_bSmooth = bSmooth;
    };
    void setRange(const int iRange)
    {
        m_iSearchRange = iRange;
    };
    std::vector<PointF> findPath(float &fX1, float &fY1, float &fX2, float &fY2, CAMap *pAMap);
    void Print(std::vector<PointF> &vcPoint, CAMap *pAMap);

private:
    H_DISALLOWCOPY(CAStar);

    #define path_queue std::priority_queue<APath*, std::vector<APath*>, APathWeightCMP>
#ifdef H_OS_WIN
    #define pathit std::unordered_map<PointI*, APath*, PointIHash, PointIEq>::iterator
    #define path_map std::unordered_map<PointI*, APath*, PointIHash, PointIEq>
#else
    #define pathit std::tr1::unordered_map<PointI*, APath*, PointIHash, PointIEq>::iterator
    #define path_map std::tr1::unordered_map<PointI*, APath*, PointIHash, PointIEq>
#endif

    void calSurrounding(PointI &stPoint, APath *pCurrent, PointI &stSource,
        path_map &mapOpenList, path_map &mapCloseList, path_queue &quOpen, CAMap *pAMap);
    void Free(path_map &mapOpenList, path_map &mapCloseList);
    std::vector<PointI> Find(PointI &stSource, PointI &stTarget, CAMap *pAMap);
    bool correctPoint(float &fX, float &fY, PointI &stPoint, bool &bAdd, CAMap *pAMap);
    int clampInt(int iValue, int iMin, int iMax);
    void checkEdge(const int &iX1, const int &iY1, const bool bY, const int &iRange, 
        std::vector<PointI> &vcPoint, CAMap *pAMap);
    bool findLatelyPoint(const int iX1, const int iY1, PointI &stPoint, CAMap *pAMap);
    std::vector<PointI> removePoint(std::vector<PointI> &vcPath, CAMap *pAMap);
    void toFPoint(float &fX1, float &fY1, float &fX2, float &fY2,
        bool &bAddStart, bool &bAddEnd, std::vector<PointI> &vcPath, std::vector<PointF> &vcPathF);
    bool checkInLine(PointI &stPoint1, PointI &stPoint2, PointI &stPoint3);
    bool detectMoveCollisionBetween(PointI &stPoint1, PointI &stPoint2, CAMap *pAMap);
    std::vector<PointF> Smooth(std::vector<PointF> &vcPath, CAMap *pAMap);
    std::vector<PointF> removePointsOnSameLine(std::vector<PointF> &vcPath, CAMap *pAMap);
    void addBaginEnd(float &fX1, float &fY1, float &fX2, float &fY2, std::vector<PointF> &vcPath);

private:
    bool m_bSmooth;
    int m_iSearchRange;
};

H_ENAMSP

#endif//H_ASTAR_H_
