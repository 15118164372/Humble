
#include "AOI.h"

H_BNAMSP

CAOI::CAOI(int iMaxX, int iMaxY, int iMaxZ) : m_iMaxX(iMaxX), m_iMaxY(iMaxY), m_iMaxZ(iMaxZ)
{
    int i(H_INIT_NUMBER);
    m_iMaxX = (0 == iMaxX ? 1 : iMaxX);
    m_iMaxY = (0 == iMaxY ? 1 : iMaxY);
    m_iMaxZ = (0 == iMaxZ ? 1 : iMaxZ);

    for (i = H_INIT_NUMBER; i < m_iMaxX; ++i)
    {
        position_map mapTmp;
        m_vcXList.push_back(mapTmp);
    }
    for (i = H_INIT_NUMBER; i < m_iMaxY; ++i)
    {
        position_map mapTmp;
        m_vcYList.push_back(mapTmp);
    }
    for (i = H_INIT_NUMBER; i < m_iMaxZ; ++i)
    {
        position_map mapTmp;
        m_vcZList.push_back(mapTmp);
    }
}

CAOI::~CAOI(void)
{
    for (positionit it = m_mapPos.begin(); m_mapPos.end() != it; ++it)
    {
        H_SafeDelete(it->second);
    }
    m_mapPos.clear();
}

bool CAOI::checkPos(const int &iX, const int &iY, const int &iZ)
{
    if (iX < 0 || iX >= m_iMaxX
        || iY < 0 || iY >= m_iMaxY
        || iZ < 0 || iZ >= m_iMaxZ)
    {
        return false;
    }

    return true;
}

void CAOI::calArea(Position *pPos, const int &iXDist, const int &iYDist, const int &iZDist, std::vector<int64_t> &vcArea)
{
    int iSub(pPos->X - iXDist);
    int xStart((iSub >= 0 ? iSub : 0));
    int iAdd(pPos->X + iXDist);
    int xEnd((iAdd >= m_iMaxX ? m_iMaxX - 1 : iAdd));

    iSub = pPos->Y - iYDist;
    int yStart((iSub >= 0 ? iSub : 0));
    iAdd = pPos->Y + iYDist;
    int yEnd((iAdd >= m_iMaxY ? m_iMaxY - 1 : iAdd));

    iSub = pPos->Z - iZDist;
    int zStart((iSub >= 0 ? iSub : 0));
    iAdd = pPos->Z + iZDist;
    int zEnd((iAdd >= m_iMaxZ ? m_iMaxZ - 1 : iAdd));

    positionit itMap;
    position_map *pMap;
    for (int x = xStart; x <= xEnd; ++x)
    {
        pMap = &m_vcXList[x];
        for (itMap = pMap->begin(); pMap->end() != itMap; ++itMap)
        {
            if (itMap->second->Y >= yStart && itMap->second->Y <= yEnd
                && itMap->second->Z >= zStart && itMap->second->Z <= zEnd)
            {
                vcArea.push_back(itMap->first);
            }
        }
    }
}

bool CAOI::Enter(const int64_t iId, const int iX, const int iY, const int iZ)
{
    H_ASSERT(checkPos(iX, iY, iZ), "param error.");

    positionit it(m_mapPos.find(iId));
    if (m_mapPos.end() != it)
    {
        return false;
    }

    Position *pPos = new(std::nothrow) Position;
    H_ASSERT(NULL != pPos, "malloc memory error.");
    pPos->X = iX;
    pPos->Y = iY;
    pPos->Z = iZ;
    m_mapPos[iId] = pPos;

    m_vcXList[iX][iId] = pPos;
    m_vcYList[iY][iId] = pPos;
    m_vcZList[iZ][iId] = pPos;

    return true;
}

void CAOI::Leave(const int64_t iId)
{
    positionit it(m_mapPos.find(iId));
    if (m_mapPos.end() == it)
    {
        return;
    }

    Position *pPos(it->second);
    m_vcXList[pPos->X].erase(m_vcXList[pPos->X].find(iId));
    m_vcYList[pPos->Y].erase(m_vcYList[pPos->Y].find(iId));
    m_vcZList[pPos->Z].erase(m_vcZList[pPos->Z].find(iId));
    m_mapPos.erase(it);
    H_SafeDelete(pPos);
}

void CAOI::moveData(const int64_t &iId, Position *pPos, const int &iX, const int &iY, const int &iZ)
{
    if (pPos->X != iX)
    {
        m_vcXList[pPos->X].erase(m_vcXList[pPos->X].find(iId));
        pPos->X = iX;
        m_vcXList[iX][iId] = pPos;
    }
    if (pPos->Y != iY)
    {
        m_vcYList[pPos->Y].erase(m_vcYList[pPos->Y].find(iId));
        pPos->Y = iY;
        m_vcYList[iY][iId] = pPos;
    }
    if (pPos->Z != iZ)
    {
        m_vcZList[pPos->Z].erase(m_vcZList[pPos->Z].find(iId));
        pPos->Z = iZ;
        m_vcZList[iZ][iId] = pPos;
    }
}

bool CAOI::onlyMove(const int64_t iId, const int iX, const int iY, const int iZ)
{
    H_ASSERT(checkPos(iX, iY, iZ), "param error.");
    positionit it(m_mapPos.find(iId));
    if (m_mapPos.end() == it)
    {
        return false;
    }

    moveData(iId, it->second, iX, iY, iZ);

    return true;
}

bool CAOI::Move(const int64_t &iId, const int &iX, const int &iY, const int &iZ,
    const int &iXDist, const int &iYDist, const int &iZDist,
    std::vector<int64_t> &outArea, std::vector<int64_t> &newArea)
{
    H_ASSERT(checkPos(iX, iY, iZ), "param error.");
    positionit it(m_mapPos.find(iId));
    if (m_mapPos.end() == it)
    {
        return false;
    }

    Position *pPos(it->second);
    //处理移动距离大于视野距离的
    if (abs(pPos->X - iX) > iXDist * 2 
        || abs(pPos->Y - iY) > iYDist * 2
        || abs(pPos->Z - iZ) > iZDist * 2)
    {
        calArea(pPos, iXDist, iYDist, iZDist, outArea);
        moveData(iId, pPos, iX, iY, iZ);
        calArea(pPos, iXDist, iYDist, iZDist, newArea);

        return true;
    }

    //移动前区域
    Position stOldStart;
    Position stOldEnd;
    calPosArea(stOldStart, stOldEnd, pPos, iXDist, iYDist, iZDist);

    moveData(iId, pPos, iX, iY, iZ);

    //移动后区域
    Position stStart;
    Position stEnd;
    calPosArea(stStart, stEnd, pPos, iXDist, iYDist, iZDist);
    
    calOutInArea(stOldStart, stOldEnd, stStart, stEnd, iId, outArea, newArea);

    return true;
}

void CAOI::calPosArea(Position &stStart, Position &stEnd, Position *pPos, 
    const int &iXDist, const int &iYDist, const int &iZDist)
{
    int iSub(pPos->X - iXDist);
    stStart.X = iSub >= 0 ? iSub : 0;
    int iAdd(pPos->X + iXDist);
    stEnd.X = iAdd >= m_iMaxX ? m_iMaxX - 1 : iAdd;
    iSub = pPos->Y - iYDist;
    stStart.Y = iSub >= 0 ? iSub : 0;
    iAdd = pPos->Y + iYDist;
    stEnd.Y = iAdd >= m_iMaxY ? m_iMaxY - 1 : iAdd;
    iSub = pPos->Z - iZDist;
    stStart.Z = iSub >= 0 ? iSub : 0;
    iAdd = pPos->Z + iZDist;
    stEnd.Z = iAdd >= m_iMaxZ ? m_iMaxZ - 1 : iAdd;
}

void CAOI::addVector(std::vector<int64_t> &vcArea, const int64_t &iId)
{
    if (vcArea.end() == std::find(vcArea.begin(), vcArea.end(), iId))
    {
        vcArea.push_back(iId);
    }
}

void CAOI::addXArea(Position &stStart, Position &stEnd,
    position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea)
{
    for (positionit itPos = mapPos.begin(); mapPos.end() != itPos; ++itPos)
    {
        if (itPos->first == iId)
        {
            continue;
        }
        if (itPos->second->Y >= stStart.Y && itPos->second->Y <= stEnd.Y
            && itPos->second->Z >= stStart.Z && itPos->second->Z <= stEnd.Z)
        {
            addVector(vcArea, itPos->first);
        }
    }
}

void CAOI::calXOutInArea(Position &stOldStart, Position &stOldEnd, 
    Position &stStart, Position &stEnd,
    const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea)
{
    int x;
    if (stStart.X > stOldStart.X)
    {
        //离开区域
        for (x = stOldStart.X; x < stStart.X; ++x)
        {
            addXArea(stOldStart, stOldEnd, m_vcXList[x], iId, outArea);
        }
        //进入区域
        for (x = stOldEnd.X + 1; x <= stEnd.X; ++x)
        {
            addXArea(stStart, stEnd, m_vcXList[x], iId, newArea);
        }
    }

    if (stStart.X < stOldStart.X)
    {
        //离开区域
        for (x = stEnd.X + 1; x <= stOldEnd.X; ++x)
        {
            addXArea(stOldStart, stOldEnd, m_vcXList[x], iId, outArea);
        }
        //进入区域
        for (x = stStart.X; x < stOldStart.X; ++x)
        {
            addXArea(stStart, stEnd, m_vcXList[x], iId, newArea);
        }
    }
}

void CAOI::addYArea(Position &stStart, Position &stEnd,
    position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea)
{
    for (positionit itPos = mapPos.begin(); mapPos.end() != itPos; ++itPos)
    {
        if (itPos->first == iId)
        {
            continue;
        }
        if (itPos->second->X >= stStart.X && itPos->second->X <= stEnd.X
            && itPos->second->Z >= stStart.Z && itPos->second->Z <= stEnd.Z)
        {
            addVector(vcArea, itPos->first);
        }
    }
}

void CAOI::calYOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
    const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea)
{
    int y;
    if (stStart.Y > stOldStart.Y)
    {
        //离开区域
        for (y = stOldStart.Y; y < stStart.Y; ++y)
        {
            addYArea(stOldStart, stOldEnd, m_vcYList[y], iId, outArea);
        }
        //进入区域
        for (y = stOldEnd.Y + 1; y <= stEnd.Y; ++y)
        {
            addYArea(stStart, stEnd, m_vcYList[y], iId, newArea);
        }
    }

    if (stStart.Y < stOldStart.Y)
    {
        //离开区域
        for (y = stEnd.Y + 1; y <= stOldEnd.Y; ++y)
        {
            addYArea(stOldStart, stOldEnd, m_vcYList[y], iId, outArea);
        }
        //进入区域
        for (y = stStart.Y; y < stOldStart.Y; ++y)
        {
            addYArea(stStart, stEnd, m_vcYList[y], iId, newArea);
        }
    }
}

void CAOI::addZArea(Position &stStart, Position &stEnd,
    position_map &mapPos, const int64_t &iId, std::vector<int64_t> &vcArea)
{
    for (positionit itPos = mapPos.begin(); mapPos.end() != itPos; ++itPos)
    {
        if (itPos->first == iId)
        {
            continue;
        }
        if (itPos->second->X >= stStart.X && itPos->second->X <= stEnd.X
            && itPos->second->Y >= stStart.Y && itPos->second->Y <= stEnd.Y)
        {
            addVector(vcArea, itPos->first);
        }
    }
}

void CAOI::calZOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
    const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea)
{
    int z;
    if (stStart.Z > stOldStart.Z)
    {
        //离开区域
        for (z = stOldStart.Z; z < stStart.Z; ++z)
        {
            addZArea(stOldStart, stOldEnd, m_vcZList[z], iId, outArea);
        }
        //进入区域
        for (z = stOldEnd.Z + 1; z <= stEnd.Z; ++z)
        {
            addZArea(stStart, stEnd, m_vcZList[z], iId, newArea);
        }
    }
    if (stStart.Z < stOldStart.Z)
    {
        //离开区域
        for (z = stEnd.Z + 1; z <= stOldEnd.Z; ++z)
        {
            addZArea(stOldStart, stOldEnd, m_vcZList[z], iId, outArea);
        }
        //进入区域
        for (z = stStart.Z; z < stOldStart.Z; ++z)
        {
            addZArea(stStart, stEnd, m_vcZList[z], iId, newArea);
        }
    }
}

void CAOI::calOutInArea(Position &stOldStart, Position &stOldEnd, Position &stStart, Position &stEnd,
    const int64_t &iId, std::vector<int64_t> &outArea, std::vector<int64_t> &newArea)
{
    calXOutInArea(stOldStart, stOldEnd, stStart, stEnd, iId, outArea, newArea);
    calYOutInArea(stOldStart, stOldEnd, stStart, stEnd, iId, outArea, newArea);
    calZOutInArea(stOldStart, stOldEnd, stStart, stEnd, iId, outArea, newArea);
}

bool CAOI::getArea(const int64_t &iId, const int &iXDist, const int &iYDist, const int &iZDist, std::vector<int64_t> &vcArea)
{
    positionit it(m_mapPos.find(iId));
    if (m_mapPos.end() == it)
    {
        return false;
    }

    calArea(it->second, iXDist, iYDist, iZDist, vcArea);

    return true;
}

H_ENAMSP
