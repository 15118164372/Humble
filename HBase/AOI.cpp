
#include "AOI.h"

H_BNAMSP

CAOI::CAOI(int iMaxX, int iMaxY, int iMaxZ) : m_iMaxX(iMaxX), m_iMaxY(iMaxY), m_iMaxZ(iMaxZ)
{
    m_iMaxX = (0 == iMaxX ? 1 : iMaxX);
    m_iMaxY = (0 == iMaxY ? 1 : iMaxY);
    m_iMaxZ = (0 == iMaxZ ? 1 : iMaxZ);

    for (int i = 0; i < m_iMaxX; i++)
    {
        position_map mapTmp;
        m_vcXList.push_back(mapTmp);
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

std::vector<int64_t> CAOI::calArea(Position *pPos, const int &iXDist, const int &iYDist, const int &iZDist)
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
    std::vector<int64_t> vcArea;
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

    return vcArea;
}

void CAOI::Enter(const int64_t iId, const int iX, const int iY, const int iZ)
{
    H_ASSERT(checkPos(iX, iY, iZ), "param error.");

    positionit it = m_mapPos.find(iId);
    if (m_mapPos.end() != it)
    {
        return;
    }

    Position *pPos = new(std::nothrow) Position;
    H_ASSERT(NULL != pPos, "malloc memory error.");
    pPos->X = iX;
    pPos->Y = iY;
    pPos->Z = iZ;
    m_mapPos[iId] = pPos;

    m_vcXList[iX][iId] = pPos;
}

void CAOI::Leave(const int64_t iId)
{
    positionit it = m_mapPos.find(iId);
    if (m_mapPos.end() == it)
    {
        return;
    }

    Position *pPos(it->second);
    m_vcXList[pPos->X].erase(m_vcXList[pPos->X].find(iId));
    m_mapPos.erase(it);
    H_SafeDelete(pPos);
}

void CAOI::Move(const int64_t iId, const int iX, const int iY, const int iZ)
{
    H_ASSERT(checkPos(iX, iY, iZ), "param error.");

    positionit it = m_mapPos.find(iId);
    if (m_mapPos.end() == it)
    {
        return;
    }

    Position *pPos(it->second);
    if (pPos->X != iX)
    {
        m_vcXList[pPos->X].erase(m_vcXList[pPos->X].find(iId));
        pPos->X = iX;
        m_vcXList[iX][iId] = pPos;
    }
    pPos->Y = iY;
    pPos->Z = iZ;
}

std::vector<int64_t> CAOI::getArea(const int64_t &iId, const int &iXDist, const int &iYDist, const int &iZDist)
{
    positionit it = m_mapPos.find(iId);
    if (m_mapPos.end() == it)
    {
        std::vector<int64_t> vcArea;
        return vcArea;
    }

    return calArea(it->second, iXDist, iYDist, iZDist);
}

H_ENAMSP
