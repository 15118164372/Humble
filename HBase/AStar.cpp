
#include "AStar.h"

H_BNAMSP

CAMap::CAMap(int maxX, int maxY) : m_maxX(maxX), m_maxY(maxY)
{
}

CAMap::~CAMap(void)
{
}

void CAMap::Filled(int iX, int iY, int iWeight)
{
    PointI stPoint;
    stPoint.iX = iX;
    stPoint.iY = iY;

    m_mapFilled[stPoint] = iWeight;
}

int CAMap::filledTiles(PointI &stPoint)
{
    filledit it(m_mapFilled.find(stPoint));
    if (m_mapFilled.end() == it)
    {
        return 0;
    }

    return it->second;
}

bool CAMap::canMove(int iX, int iY)
{
    PointI stPoint;
    stPoint.iX = iX;
    stPoint.iY = iY;

    return canMove(stPoint);
}

bool CAMap::canMove(PointI &stPoint)
{
    filledit it(m_mapFilled.find(stPoint));
    if (m_mapFilled.end() == it)
    {
        return true;
    }

    return it->second >= 0;
}


CAStar::CAStar(void) : m_bSmooth(true), m_iSearchRange(H_INIT_NUMBER)
{
}

CAStar::~CAStar(void)
{
}

void CAStar::Free(path_map &mapOpenList, path_map &mapCloseList)
{
    pathit itPath;
    for (itPath = mapOpenList.begin(); mapOpenList.end() != itPath; ++itPath)
    {
        H_SafeDelete(itPath->second);
    }
    for (itPath = mapCloseList.begin(); mapCloseList.end() != itPath; ++itPath)
    {
        H_SafeDelete(itPath->second);
    }
}

void CAStar::calSurrounding(PointI &stPoint, APath *pCurrent, PointI &stSource,
    path_map &mapOpenList, path_map &mapCloseList, path_queue &quOpen, CAMap *pAMap)
{
    pathit itPath(mapCloseList.find(&stPoint));
    if (mapCloseList.end() != itPath || !pAMap->canMove(stPoint))
    {
        return;
    }

    int iFillWeight(pAMap->filledTiles(stPoint));
    int iWeight(pCurrent->iFillWeight + iFillWeight + pCurrent->iDistTraveled + 1 +
        abs(stPoint.iX - stSource.iX) + abs(stPoint.iY - stSource.iY));

    itPath = mapOpenList.find(&stPoint);
    if (mapOpenList.end() == itPath)
    {
        APath *pPathPoint = new(std::nothrow) APath;
        H_ASSERT(NULL != pPathPoint, "malloc memory error.");

        pPathPoint->stPoint = stPoint;
        pPathPoint->pParent = pCurrent;
        pPathPoint->iFillWeight = pCurrent->iFillWeight + iFillWeight;
        pPathPoint->iDistTraveled = pCurrent->iDistTraveled + 1;
        pPathPoint->iWeight = iWeight;

        mapOpenList[&pPathPoint->stPoint] = pPathPoint;
        quOpen.push(pPathPoint);

        return;
    }

    if (iWeight < itPath->second->iWeight)
    {
        itPath->second->pParent = pCurrent;
    }
}

std::vector<PointI> CAStar::Find(PointI &stSource, PointI &stTarget, CAMap *pAMap)
{
    path_map mapOpenList;
    path_map mapCloseList;
    path_queue quOpen;
    std::vector<PointI> vcPath;

    int iTargetWeight(pAMap->filledTiles(stTarget));
    if (-1 == iTargetWeight)
    {
        return vcPath;
    }

    APath *pTargetPoint = new(std::nothrow) APath;
    H_ASSERT(NULL != pTargetPoint, "malloc memory error.");

    pTargetPoint->stPoint = stTarget;
    pTargetPoint->pParent = NULL;
    pTargetPoint->iDistTraveled = H_INIT_NUMBER;
    pTargetPoint->iFillWeight = iTargetWeight;
    pTargetPoint->iWeight = pTargetPoint->iFillWeight + pTargetPoint->iDistTraveled +
        abs(pTargetPoint->stPoint.iX - stSource.iX) + abs(pTargetPoint->stPoint.iY - stSource.iY);

    mapOpenList[&pTargetPoint->stPoint] = pTargetPoint;
    quOpen.push(pTargetPoint);

    APath *pCurrent(NULL);
    PointI stPoint;
    while (true)
    {
        if (quOpen.empty())
        {
            pCurrent = NULL;
            break;
        }

        pCurrent = quOpen.top();
        quOpen.pop();
        if (pCurrent->stPoint == stSource)
        {
            break;
        }

        mapOpenList.erase(mapOpenList.find(&pCurrent->stPoint));
        mapCloseList[&pCurrent->stPoint] = pCurrent;

        if (pCurrent->stPoint.iX > 0)
        {
            stPoint.iX = pCurrent->stPoint.iX - 1;
            stPoint.iY = pCurrent->stPoint.iY;
            calSurrounding(stPoint, pCurrent, stSource, mapOpenList, mapCloseList, quOpen, pAMap);
        }
        if (pCurrent->stPoint.iX < pAMap->m_maxX - 1)
        {
            stPoint.iX = pCurrent->stPoint.iX + 1;
            stPoint.iY = pCurrent->stPoint.iY;
            calSurrounding(stPoint, pCurrent, stSource, mapOpenList, mapCloseList, quOpen, pAMap);
        }
        if (pCurrent->stPoint.iY > 0)
        {
            stPoint.iX = pCurrent->stPoint.iX;
            stPoint.iY = pCurrent->stPoint.iY - 1;
            calSurrounding(stPoint, pCurrent, stSource, mapOpenList, mapCloseList, quOpen, pAMap);
        }
        if (pCurrent->stPoint.iY < pAMap->m_maxY - 1)
        {
            stPoint.iX = pCurrent->stPoint.iX;
            stPoint.iY = pCurrent->stPoint.iY + 1;
            calSurrounding(stPoint, pCurrent, stSource, mapOpenList, mapCloseList, quOpen, pAMap);
        }
    }

    while (NULL != pCurrent)
    {
        vcPath.push_back(pCurrent->stPoint);
        pCurrent = pCurrent->pParent;
    }

    Free(mapOpenList, mapCloseList);

    return vcPath;
}

int CAStar::clampInt(int iValue, int iMin, int iMax)
{
    if (iValue < iMin)
    {
        return iMin;
    }
    if (iValue > iMax)
    {
        return iMax;
    }
    return iValue;
}

void CAStar::checkEdge(const int &iX1, const int &iY1, const bool bY, const int &iRange,
    std::vector<PointI> &vcPoint, CAMap *pAMap)
{
    PointI stTmpPoint;

    if (bY)
    {
        stTmpPoint.iX = iX1;

        int iYMin(iY1 - iRange);
        iYMin = iYMin < 0 ? 0 : iYMin;
        int iYMax(iY1 + iRange);
        iYMax = iYMax >= pAMap->m_maxY ? pAMap->m_maxY - 1 : iYMax;

        for (int iY = iYMin; iY <= iYMax; ++iY)
        {
            stTmpPoint.iY = iY;
            if (pAMap->canMove(stTmpPoint))
            {
                vcPoint.push_back(stTmpPoint);
            }
        }

        return;
    }

    stTmpPoint.iY = iY1;

    int iXMin(iX1 - iRange);
    iXMin = iXMin < 0 ? 0 : iXMin;
    int iXMax(iX1 + iRange);
    iXMax = iXMax >= pAMap->m_maxX ? pAMap->m_maxX - 1 : iXMax;

    for (int iX = iXMin; iX <= iXMax; ++iX)
    {
        stTmpPoint.iX = iX;
        if (pAMap->canMove(stTmpPoint))
        {
            vcPoint.push_back(stTmpPoint);
        }
    }
}

bool CAStar::findLatelyPoint(const int iX1, const int iY1, PointI &stPoint, CAMap *pAMap)
{
    int iMinDis(0);
    int iDis;
    int iX, iY;
    std::vector<PointI> vcPoint;
    std::vector<PointI>::iterator itPoint;

    for (int i = 1; i <= m_iSearchRange; ++i)
    {
        //左
        iX = iX1 - i;
        if (iX >= 0)
        {
            checkEdge(iX, iY1, true, i, vcPoint, pAMap);
        }
        //右
        iX = iX1 + i;
        if (iX < pAMap->m_maxX)
        {
            checkEdge(iX, iY1, true, i, vcPoint, pAMap);
        }
        //上
        iY = iY1 + i;
        if (iY < pAMap->m_maxY)
        {
            checkEdge(iX1, iY, false, i, vcPoint, pAMap);
        }
        //下
        iY = iY1 - i;
        if (iY >= 0)
        {
            checkEdge(iX1, iY, false, i, vcPoint, pAMap);
        }

        if (!vcPoint.empty())
        {
            break;
        }
    }

    if (vcPoint.empty())
    {
        return false;
    }

    for (itPoint = vcPoint.begin(); vcPoint.end() != itPoint; ++itPoint)
    {
        iDis = abs(iX1 - itPoint->iX) + abs(iY1 - itPoint->iY);
        if (itPoint == vcPoint.begin())
        {
            iMinDis = iDis;
            stPoint = *itPoint;
            continue;
        }
        
        if (iDis < iMinDis)
        {
            iMinDis = iDis;
            stPoint = *itPoint;
        }
    }

    return true;
}

bool CAStar::correctPoint(float &fX, float &fY, PointI &stPoint, bool &bAdd, CAMap *pAMap)
{
    stPoint.iX = clampInt(int(round(fX)), 0, pAMap->m_maxX - 1);
    stPoint.iY = clampInt(int(round(fY)), 0, pAMap->m_maxY - 1);
    if (!pAMap->canMove(stPoint))
    {
        bAdd = true;
        if (!findLatelyPoint(stPoint.iX, stPoint.iY, stPoint, pAMap))
        {
            return false;
        }

        return true;
    }

    bAdd = false;

    return true;
}

bool CAStar::checkInLine(PointI &stPoint1, PointI &stPoint2, PointI &stPoint3)
{
    if (stPoint1.iX == stPoint2.iX && stPoint1.iX == stPoint3.iX)
    {
        return true;
    }
    if (stPoint1.iY == stPoint2.iY && stPoint1.iY == stPoint3.iY)
    {
        return true;
    }

    return false;
}

bool CAStar::detectMoveCollisionBetween(PointI &stPoint1, PointI &stPoint2, CAMap *pAMap)
{
    float fX0(float(stPoint1.iX));
    float fY0(float(stPoint1.iY));
    float fX1(float(stPoint2.iX));
    float fY1(float(stPoint2.iY));

    bool bSteep = fabs(fY1 - fY0) > fabs(fX1 - fX0);
    if (bSteep)
    {
        fX0 = float(stPoint1.iY);
        fY0 = float(stPoint1.iX);
        fX1 = float(stPoint2.iY);
        fY1 = float(stPoint2.iX);
    }

    if (fX0 > fX1)
    {
        float fX(fX0);
        float fY(fY0);
        fX0 = fX1;
        fX1 = fX;
        fY0 = fY1;
        fY1 = fY;
    }

    float fRatio(fabs((fY1 - fY0) / (fX1 - fX0)));
    int iMirror(-1);
    if (fY1 > fY0)
    {
        iMirror = 1;
    }

    bool bSkip(false);
    float fCurY, fTmp;
    int iCrossY;
    for (int iCol = int(floor(fX0)); iCol < int(ceil(fX1)); ++iCol)
    {
        fCurY = fY0 + iMirror*fRatio*(float(iCol) - fX0);
        //第一格不进行延边计算
        bSkip = false;
        if (iCol == int(floor(fX0)))
        {
            bSkip = int(fCurY) != int(fY0);
        }

        if (!bSkip)
        {
            if (!bSteep)
            {
                if (!pAMap->canMove(iCol, int(H_Max(0, floor(fCurY)))))
                {
                    return true;
                }
            }
            else
            {
                if (!pAMap->canMove(int(H_Max(0, floor(fCurY))), iCol))
                {
                    return true;
                }
            }
        }

        if (iMirror > 0)
        {
            fTmp = ceil(fCurY) - fCurY;
        }
        else
        {
            fTmp = fCurY - floor(fCurY);
        }

        //根据斜率计算是否有跨格
        if (fTmp < fRatio)
        {
            iCrossY = int(floor(fCurY)) + iMirror;
            //判断是否超出范围
            if (iCrossY > int(H_Max(fY0, fY1)) ||
                iCrossY < int(H_Min(fY0, fY1)))
            {
                return false;
            }

            //跨线格子
            if (!bSteep)
            {
                if (!pAMap->canMove(iCol, iCrossY))
                {
                    return true;
                }
            }
            else
            {
                if (!pAMap->canMove(iCrossY, iCol))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

std::vector<PointI> CAStar::removePoint(std::vector<PointI> &vcPath, CAMap *pAMap)
{
    if (vcPath.size() < 3)
    {
        return vcPath;
    }

    std::vector<PointI> vcTmpLine;
    vcTmpLine.push_back(vcPath[0]);
    //移除直线上的点,简单处理
    for (size_t i = 1; i < vcPath.size() - 1; ++i)
    {
        if (!checkInLine(vcTmpLine[vcTmpLine.size() - 1], vcPath[i], vcPath[i + 1]))
        {
            vcTmpLine.push_back(vcPath[i]);
        }
    }
    vcTmpLine.push_back(vcPath[vcPath.size() - 1]);
    if (vcTmpLine.size() < 3)
    {
        return vcTmpLine;
    }

    //拐点移除
    vcPath.clear();
    vcPath.push_back(vcTmpLine[0]);
    for (size_t i = 1; i < vcTmpLine.size() - 1; ++i)
    {
        if (detectMoveCollisionBetween(vcPath[vcPath.size() - 1], vcTmpLine[i + 1], pAMap))
        {
            vcPath.push_back(vcTmpLine[i]);
        }
    }
    vcPath.push_back(vcTmpLine[vcTmpLine.size() - 1]);

    return vcPath;
}

void CAStar::toFPoint(float &fX1, float &fY1, float &fX2, float &fY2,
    bool &bAddStart, bool &bAddEnd, std::vector<PointI> &vcPath, std::vector<PointF> &vcPathF)
{
    if (bAddStart)
    {
        PointF stF;
        stF.fX = fX1;
        stF.fY = fY1;
        vcPathF.push_back(stF);
    }
    for (std::vector<PointI>::iterator it = vcPath.begin(); vcPath.end() != it; ++it)
    {
        PointF stF;
        stF.fX = float(it->iX);
        stF.fY = float(it->iY);
        vcPathF.push_back(stF);
    }
    if (!bAddStart)
    {
        vcPathF[0].fX = fX1;
        vcPathF[0].fY = fY1;
    }
    if (!bAddEnd)
    {
        vcPathF[vcPathF.size() - 1].fX = fX2;
        vcPathF[vcPathF.size() - 1].fY = fY2;
    }
}

std::vector<PointF> CAStar::removePointsOnSameLine(std::vector<PointF> &vcPath, CAMap *pAMap)
{
    size_t iPathLen(vcPath.size());
    std::vector<PointF> vcNewPath(vcPath);
    std::vector<PointF>::iterator itPath;
    float fSubX, fSubY, fA1, fB1, fC1, fAngle;
    float fA1X, fA1Y, fA2X, fA2Y;
    float fDiffX, fDiffY;
    PointI stP1, stP2;
    for (size_t i = 0; i < iPathLen - 2; )
    {
        for (size_t j = i + 1; j < iPathLen - 1; )
        {
            fSubX = vcPath[i + 1].fX - vcPath[i].fX;
            fSubY = vcPath[i + 1].fY - vcPath[i].fY;
            fA1 = fSubX*fSubX + fSubY*fSubY;
            fB1 = sqrt(fA1);
            fC1 = 1 / fB1;
            fA1X = fC1 * fSubX;
            fA1Y = fC1 * fSubY;

            fSubX = vcPath[j + 1].fX - vcPath[i].fX;
            fSubY = vcPath[j + 1].fY - vcPath[i].fY;
            fA1 = fSubX*fSubX + fSubY*fSubY;
            fB1 = sqrt(fA1);
            fC1 = 1 / fB1;
            fA2X = fC1 * fSubX;
            fA2Y = fC1 * fSubY;

            fAngle = (float)(acos(fA1X*fA2X + fA1Y*fA2Y) / (sqrt(fA1X*fA1X + fA1Y*fA1Y) * sqrt(fA2X*fA2X + fA2Y*fA2Y)) * 180.0 / 3.1415926);

            if (fAngle <= 10)
            {
                //检测是否有移动阻挡
                stP1.iX = (int)vcPath[i].fX;
                stP1.iY = (int)vcPath[i].fY;
                stP2.iX = (int)vcPath[j].fX;
                stP2.iY = (int)vcPath[j].fY;
                if (!detectMoveCollisionBetween(stP1, stP2, pAMap))
                {
                    for (itPath = vcNewPath.begin(); vcNewPath.end() != itPath; ++itPath)
                    {
                        fDiffX = itPath->fX - vcPath[j].fX;
                        fDiffY = itPath->fY - vcPath[j].fY;
                        if (fDiffX > -0.0001 && fDiffX < 0.0001 && fDiffY > -0.0001 && fDiffY < 0.0001)
                        {
                            vcNewPath.erase(itPath);
                            break;
                        }
                    }
                }
                else
                {
                    i = j + 1;
                    break;
                }
            }
            else
            {
                i = j + 1;
                break;
            }
            j++;
            if (j == iPathLen - 1)
            {
                i = j;
            }
        }
    }

    return vcNewPath;
}

std::vector<PointF> CAStar::Smooth(std::vector<PointF> &vcPath, CAMap *pAMap)
{
    float fMaxSegmentLength(2);
    float fPathLength(0.0);
    float fXDist, fYDist;
    for (size_t i = 0; i < vcPath.size() - 1; ++i)
    {
        fXDist = vcPath[i].fX - vcPath[i + 1].fX;
        fYDist = vcPath[i].fY - vcPath[i + 1].fY;
        fPathLength += sqrt(fXDist*fXDist + fYDist*fYDist);
    }

    std::vector<PointF> vcSubdivided;
    float fDistanceAlong(0.0);
    float fStartX, fStartY, fEndX, fEndY;
    float fLength, fD;
    PointF stPoint;
    for (size_t i = 0; i < vcPath.size() - 1; ++i)
    {
        fStartX = vcPath[i].fX;
        fStartY = vcPath[i].fY;
        fEndX = vcPath[i + 1].fX;
        fEndY = vcPath[i + 1].fY;

        fXDist = fStartX - fEndX;
        fYDist = fStartY - fEndY;
        fLength = sqrt(fXDist*fXDist + fYDist*fYDist);

        while (fDistanceAlong < fLength)
        {
            fD = fDistanceAlong / fLength;
            stPoint.fX = vcPath[i].fX + (vcPath[i + 1].fX - vcPath[i].fX)*fD;
            stPoint.fY = vcPath[i].fY + (vcPath[i + 1].fY - vcPath[i].fY)*fD;
            vcSubdivided.push_back(stPoint);
            fDistanceAlong += fMaxSegmentLength;
        }

        fDistanceAlong -= fLength;
    }

    vcSubdivided.push_back(vcPath[vcPath.size() - 1]);
    int iIterations(2);
    float fStrength(0.5);
    float fTmp2X, fTmp2Y;
    PointF *pPrev;
    for (int iIt = 0; iIt < iIterations; ++iIt)
    {
        pPrev = &vcSubdivided[0];
        for (size_t i = 1; i < vcSubdivided.size() - 1; ++i)
        {
            fTmp2X = (pPrev->fX + vcSubdivided[i + 1].fX) / 2;
            fTmp2Y = (pPrev->fY + vcSubdivided[i + 1].fY) / 2;
            vcSubdivided[i].fX = vcSubdivided[i].fX + (fTmp2X - vcSubdivided[i].fX)*fStrength;
            vcSubdivided[i].fY = vcSubdivided[i].fY + (fTmp2Y - vcSubdivided[i].fY)*fStrength;
            pPrev = &vcSubdivided[i];
        }
    }

    if (vcSubdivided.size() <= 3)
    {
        return vcSubdivided;
    }

    return removePointsOnSameLine(vcSubdivided, pAMap);
}

void CAStar::addBaginEnd(float &fX1, float &fY1, float &fX2, float &fY2, std::vector<PointF> &vcPath)
{
    PointF stFSource;
    stFSource.fX = fX1;
    stFSource.fY = fY1;
    vcPath.push_back(stFSource);

    PointF stFTarget;
    stFTarget.fX = fX2;
    stFTarget.fY = fY2;
    vcPath.push_back(stFTarget);
}

std::vector<PointF> CAStar::findPath(float &fX1, float &fY1, float &fX2, float &fY2, CAMap *pAMap)
{
    bool bAddStart, bAddEnd;
    PointI stSource, stTarget;
    std::vector<PointF> vcPathF;
    //检查起点是否可以寻，不能则找最近的点
    if (!correctPoint(fX1, fY1, stSource, bAddStart, pAMap))
    {
        H_Printf("source point can't move. %f,%f", fX1, fY1);
        return vcPathF;
    }
    //检查终点是否可以寻，不能则找最近的点
    if (!correctPoint(fX2, fY2, stTarget, bAddEnd, pAMap))
    {
        H_Printf("target point can't move. %f,%f", fX2, fY2);
        return vcPathF;
    }

    //起点终点重合
    if (stTarget.iX == stSource.iX && stTarget.iY == stSource.iY)
    {
        addBaginEnd(fX1, fY1, fX2, fY2, vcPathF);
        return vcPathF;
    }

    //检查是否有阻挡
    if (!detectMoveCollisionBetween(stSource, stTarget, pAMap))
    {
        addBaginEnd(fX1, fY1, fX2, fY2, vcPathF);
        return vcPathF;
    }

    std::vector<PointI> vcFindedPath(Find(stSource, stTarget, pAMap));
    if (vcFindedPath.empty())
    {
        H_Printf("%s", "path find error.");
        return vcPathF;
    }

    vcFindedPath = removePoint(vcFindedPath, pAMap);
    toFPoint(fX1, fY1, fX2, fY2, bAddStart, bAddEnd, vcFindedPath, vcPathF);
    if (m_bSmooth && vcPathF.size() > 2)
    {
        vcPathF = Smooth(vcPathF, pAMap);
    }

    return vcPathF;
}

void CAStar::Print(std::vector<PointF> &vcPoint, CAMap *pAMap)
{
    PointI stCur;
    std::string strMsg;
    std::vector<PointF>::iterator itP;

    printf("\n");
    for (int y = pAMap->m_maxY - 1; y >= 0; --y)
    {
        strMsg.clear();
        for (int x = 0; x < pAMap->m_maxX; ++x)
        {
            stCur.iX = x;
            stCur.iY = y;
            for (itP = vcPoint.begin(); vcPoint.end() != itP; ++itP)
            {
                if (int(itP->fX) == x && int(itP->fY) == y)
                {
                    if (!pAMap->canMove(stCur))
                    {
                        strMsg += "&";
                    }
                    else
                    {
                        strMsg += "*";
                    }
                    break;
                }
            }

            if (itP == vcPoint.end())
            {
                if (!pAMap->canMove(stCur))
                {
                    strMsg += "#";
                }
                else
                {
                    strMsg += " ";
                }
            }
        }
        strMsg += "\n";
        printf(strMsg.c_str());
    }
    printf("\n");
}

H_ENAMSP
