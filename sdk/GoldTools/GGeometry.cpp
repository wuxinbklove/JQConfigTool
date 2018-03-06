#include "StdAfx.h"
#include "GGeometry.h"
#include <algorithm>
#include "math.h"

namespace GGeometry
{
    //
    AcGeMatrix3d GetBlockMatrix3d(AcDbBlockReference* pRef,
                                  AcDbBlockTableRecord* pRec)
    {
        AcGeMatrix3d matrix = pRef->blockTransform();
#if _MSC_VER < 1400
        matrix *= AcGeMatrix3d::translation(-pRec->origin().asVector());
#endif
        return matrix;
    }

	AcGeMatrix3d GOLD_EXPORT GetBlockMatrix3d(AcDbBlockReference* pRef)
	{
		AcGeMatrix3d matrix = pRef->blockTransform();
#if _MSC_VER < 1400
		AcDbBlockTableRecord *pRec;
		Acad::ErrorStatus es = acdbOpenObject(pRec, pRef->blockTableRecord(), AcDb::kForRead);
		if (es == Acad::eOk)
		{
			pRec->close();
			matrix *= AcGeMatrix3d::translation(-pRec->origin().asVector());
		}
#endif
		return matrix;
	}

    //
    AcGePoint3d Mid(AcGePoint3d& pnt1, AcGePoint3d& pnt2)
    {
        AcGePoint3d mid;
        mid.x = (pnt1.x + pnt2.x) / 2;
        mid.y = (pnt1.y + pnt2.y) / 2;
        mid.z = 0;
        return mid;
    }

    bool IsIntersect(AcDbExtents& ext1, AcDbExtents& ext2)
    {
		if(isSameExtents(ext1, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(ext2, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

        AcGePoint3d minPt1 = ext1.minPoint();
        AcGePoint3d maxPt1 = ext1.maxPoint();
        AcGePoint3d minPt2 = ext2.minPoint();
        AcGePoint3d maxPt2 = ext2.maxPoint();
        if (abs((minPt1.x + maxPt1.x - minPt2.x - maxPt2.x) / 2) < ((maxPt1.x + maxPt2.x - minPt1.x - minPt2.x) / 2)
            && (abs((minPt1.y + maxPt1.y - minPt2.y - maxPt2.y) / 2)) < ((maxPt1.y + maxPt2.y - minPt1.y - minPt2.y) / 2))
        {
            return true;
        }
        return false;
    }

	bool IsIn(AcDbExtents* ext1, AcDbExtents* ext2)
	{
		if(isSameExtents(*ext1, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(*ext2, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d minPt1 = ext1->minPoint();
		AcGePoint3d maxPt1 = ext1->maxPoint();
		AcGePoint3d minPt2 = ext2->minPoint();
		AcGePoint3d maxPt2 = ext2->maxPoint();
		if (minPt2.x > minPt1.x && minPt2.y > minPt1.y && maxPt2.x < maxPt1.x && maxPt2.y < maxPt1.y)
		{
			return true;
		}
		return false;
	}

	bool IsIn(AcDbExtents ext1, AcDbExtents ext2)
	{
		if(isSameExtents(ext1, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(ext2, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d minPt1 = ext1.minPoint();
		AcGePoint3d maxPt1 = ext1.maxPoint();
		AcGePoint3d minPt2 = ext2.minPoint();
		AcGePoint3d maxPt2 = ext2.maxPoint();
		if (minPt2.x > minPt1.x && minPt2.y > minPt1.y && maxPt2.x < maxPt1.x && maxPt2.y < maxPt1.y)
		{
			return true;
		}
		return false;
	}

	bool isAdjoin(AcDbExtents* ext1, AcDbExtents* ext2)
	{
		AcGePoint3d minPt1 = ext1->minPoint();
		AcGePoint3d maxPt1 = ext1->maxPoint();
		AcGePoint3d minPt2 = ext2->minPoint();
		AcGePoint3d maxPt2 = ext2->maxPoint();
		if(fabs(maxPt1.y - maxPt2.y) < 0.01 || fabs(maxPt1.x - maxPt2.x) < 0.01
			|| fabs(minPt1.y - minPt2.y) < 0.01 || fabs(minPt1.x - minPt2.x) < 0.01)
			return true;
		return false;
	}

	bool isAdjoin(AcDbExtents ext1, AcDbExtents ext2)
	{
		AcGePoint3d minPt1 = ext1.minPoint();
		AcGePoint3d maxPt1 = ext1.maxPoint();
		AcGePoint3d minPt2 = ext2.minPoint();
		AcGePoint3d maxPt2 = ext2.maxPoint();
		if(fabs(maxPt1.y - maxPt2.y) < 0.01 || fabs(maxPt1.x - maxPt2.x) < 0.01
			|| fabs(minPt1.y - minPt2.y) < 0.01 || fabs(minPt1.x - minPt2.x) < 0.01)
			return true;
		return false;
	}

	bool isLeftAdjoin(AcDbExtents ext, AcDbExtents compareExtent)
	{
		if(isSameExtents(ext, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(compareExtent, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d rightMinPt = ext.minPoint();
		AcGePoint3d rightMaxPt = ext.maxPoint();
		AcGePoint3d leftMinPt = compareExtent.minPoint();
		AcGePoint3d leftMaxPt = compareExtent.maxPoint();
		if(fabs(leftMaxPt.x - rightMinPt.x) < 0.01 
			&& (leftMinPt.y > rightMinPt.y || fabs(leftMinPt.y - rightMinPt.y) < 0.01) 
			&& (leftMaxPt.y < rightMaxPt.y || fabs(rightMaxPt.y - leftMaxPt.y) < 0.01))
			return true;
		return false;
	}
	bool isRightAdjoin(AcDbExtents ext, AcDbExtents compareExtent)
	{
		if(isSameExtents(ext, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(compareExtent, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d rightMinPt = compareExtent.minPoint();
		AcGePoint3d rightMaxPt = compareExtent.maxPoint();
		AcGePoint3d leftMinPt = ext.minPoint();
		AcGePoint3d leftMaxPt = ext.maxPoint();
		if(fabs(leftMaxPt.x - rightMinPt.x) < 0.01 
			&& (rightMinPt.y > leftMinPt.y || fabs(rightMinPt.y - leftMinPt.y) < 0.01) 
			&& (rightMaxPt.y < leftMaxPt.y || fabs(leftMaxPt.y - rightMaxPt.y) < 0.01))
			return true;
		return false;
	}
	bool isTopAdjoin(AcDbExtents ext, AcDbExtents compareExtent)
	{
		if(isSameExtents(ext, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(compareExtent, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d bottomMinPt = ext.minPoint();
		AcGePoint3d bottomMaxPt = ext.maxPoint();
		AcGePoint3d topMinPt = compareExtent.minPoint();
		AcGePoint3d topMaxPt = compareExtent.maxPoint();
		if(fabs(topMinPt.y - bottomMaxPt.y) < 0.01
			&& (topMinPt.x > bottomMinPt.x|| fabs(topMinPt.x - bottomMinPt.x) < 0.01) 
			&& (topMaxPt.x < bottomMaxPt.x || fabs(bottomMaxPt.x - topMaxPt.x) < 0.01))
			return true;
		return false;
	}
	bool isBottomAdjoin(AcDbExtents ext, AcDbExtents compareExtent)
	{
		if(isSameExtents(ext, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(compareExtent, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d topMinPt = ext.minPoint();
		AcGePoint3d topMaxPt = ext.maxPoint();
		AcGePoint3d bottomMinPt = compareExtent.minPoint();
		AcGePoint3d bottomMaxPt = compareExtent.maxPoint();
		if(fabs(bottomMaxPt.y - topMinPt.y) < 0.01
			&& (bottomMinPt.x > topMinPt.x|| fabs(bottomMinPt.x - topMinPt.x) < 0.01) 
			&& (bottomMaxPt.x < topMaxPt.x || fabs(topMaxPt.x - bottomMaxPt.x) < 0.01))
			return true;
		return false;
	}

	bool isSameExtents(AcDbExtents* ext1, AcDbExtents* ext2)
	{
		AcGePoint3d minPt1 = ext1->minPoint();
		AcGePoint3d maxPt1 = ext1->maxPoint();
		AcGePoint3d minPt2 = ext2->minPoint();
		AcGePoint3d maxPt2 = ext2->maxPoint();
		if (fabs(minPt2.x - minPt1.x) <= 0.01 && fabs(minPt2.y - minPt1.y) < 0.01 
			&& fabs(maxPt2.x - maxPt1.x) < 0.01 && fabs(maxPt2.y - maxPt1.y) < 0.01)
		{
			return true;
		}
		return false;
	}

	bool isSameExtents(AcDbExtents ext1, AcDbExtents ext2)
	{
		AcGePoint3d minPt1 = ext1.minPoint();
		AcGePoint3d maxPt1 = ext1.maxPoint();
		AcGePoint3d minPt2 = ext2.minPoint();
		AcGePoint3d maxPt2 = ext2.maxPoint();
		if (fabs(minPt2.x - minPt1.x) <= 0.01 && fabs(minPt2.y - minPt1.y) < 0.01 
			&& fabs(maxPt2.x - maxPt1.x) < 0.01 && fabs(maxPt2.y - maxPt1.y) < 0.01)
		{
			return true;
		}
		return false;
	}

	bool IsInclude(AcDbExtents ext1, AcDbExtents ext2)
	{
		if(isSameExtents(ext1, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)) || isSameExtents(ext2, AcDbExtents(AcGePoint3d::kOrigin, AcGePoint3d::kOrigin)))
			return false;

		AcGePoint3d minPt1 = ext1.minPoint();
		AcGePoint3d maxPt1 = ext1.maxPoint();
		AcGePoint3d minPt2 = ext2.minPoint();
		AcGePoint3d maxPt2 = ext2.maxPoint();
		if (minPt2.x >= minPt1.x && minPt2.y >= minPt1.y && maxPt2.x <= maxPt1.x && maxPt2.y <= maxPt1.y)
		{
			return true;
		}
		return false;
	}

	bool IsPointInExtents(const AcGePoint3d& point, const AcDbExtents& ext)
	{
		if (point.x > ext.minPoint().x &&
			point.y > ext.minPoint().y &&
			point.x < ext.maxPoint().x &&
			point.y < ext.maxPoint().y)
		{
			return true;
		}
		return false;
	}

	IntersectionTypes TwoExtentsIntersection(const AcDbExtents& ext1, const AcDbExtents& ext2 , double offsetX , double offsetY)
	{	
		double minX1 = ext1.minPoint().x;
		double minY1 = ext1.minPoint().y;
		double maxX1 = ext1.maxPoint().x;
		double maxY1 = ext1.maxPoint().y;

		double minX2 = ext2.minPoint().x - offsetX;
		double minY2 = ext2.minPoint().y - offsetY;
		double maxX2 = ext2.maxPoint().x + offsetX;
		double maxY2 = ext2.maxPoint().y + offsetY;
		
		if (maxX1 < minX2 || minX1 > maxX2 || minY1 > maxY2 || maxY1 < minY2)
			return NoIntersection;

		IntersectionTypes intersection = 0x00;
		if (minX1 <= minX2 && maxX1 >= minX2)
			intersection |= LeftIntersection;
		if (minX1 <= maxX2 && maxX1 >= maxX2)
			intersection |= RightIntersection;
		if (minX1 > minX2 && maxX1 < maxX2)
			intersection |= HorInContainment;
		if (minX1 < minX2 && maxX1 > maxX2)
			intersection |= HorOutContainment;

		if (minY1 <= minY2 && maxY1 >= minY2)
			intersection |= BottomIntersection;
		if (minY1 <= maxY2 && maxY1 >= maxY2)
			intersection |= TopIntersection;
		if (minY1 > minY2 && maxY1 < maxY2)
			intersection |= VerInContainment;
		if (minY1 < minY2 && maxY1 > maxY2)
			intersection |= VerOutContainment;

		return intersection;
	}

	void InverseMatrix(AcGeMatrix3d &matrix)
	{
		double offsetX = matrix.entry[0][3];
		double offsetY = matrix.entry[1][3];
		double offsetZ = matrix.entry[2][3];

		double inversedOffsetX = 0.0 - (offsetX * matrix.entry[0][0] + offsetY * matrix.entry[0][1] + offsetZ * matrix.entry[0][2]);
		double inversedOffsetY = 0.0 - (offsetX * matrix.entry[1][0] + offsetY * matrix.entry[1][1] + offsetZ * matrix.entry[1][2]);
		double inversedOffsetZ = 0.0 - (offsetX * matrix.entry[2][0] + offsetY * matrix.entry[2][1] + offsetZ * matrix.entry[2][2]);

		matrix.transposeIt();
		matrix.entry[0][3] = inversedOffsetX;
		matrix.entry[1][3] = inversedOffsetY;
		matrix.entry[2][3] = inversedOffsetZ;
		matrix.entry[3][0] = 0;
		matrix.entry[3][1] = 0;
		matrix.entry[3][2] = 0;
	}


	//实体位置信息相关
	typedef struct _EntityGeometryInfo
	{
		AcDbEntity *pEnt;
		double minX;
		double maxX;
		double minY;
		double maxY;
		int statisticalNumber;
	} _EntityGeometryInfo;

	_EntityGeometryInfo _ReadEntityGeometryInfo(AcDbEntity *pEnt)
	{
		_EntityGeometryInfo entityInfo;
		AcDbExtents extents;
		pEnt->getGeomExtents(extents);
		entityInfo.minX = extents.minPoint().x;
		entityInfo.maxX = extents.maxPoint().x;
		entityInfo.minY = extents.minPoint().y;
		entityInfo.maxY = extents.maxPoint().y;
		entityInfo.statisticalNumber = 0;
		entityInfo.pEnt = pEnt;
		return entityInfo;
	}

	//判断实体Y向是否存在相交
	bool _IsTwoEntityYRangeCanInteract(const _EntityGeometryInfo& info_1, const _EntityGeometryInfo& info_2)
	{
		if (info_1.minY >= info_2.maxY || info_1.maxY <= info_2.minY)
			return false;
		else
			return true;
	}

	bool _SortEntityInfoByX(const _EntityGeometryInfo& info_1 , const _EntityGeometryInfo& info_2)
	{
		if (fabs(info_1.minX - info_2.minX) > 1)
			return info_1.minX < info_2.minX;
		else
			return info_1.maxX < info_2.maxX;
	}

	bool _SortEntityInfoByY(const _EntityGeometryInfo& info_1, const _EntityGeometryInfo& info_2)
	{
		if (fabs(info_1.minY - info_2.minY) > 1)
			return info_1.minY < info_2.minY;
		else
			return info_1.maxY < info_2.maxY;
	}

	bool _SortEntityHor(const list<_EntityGeometryInfo>& hor_1, const list<_EntityGeometryInfo>& hor_2)
	{
		if (hor_1.begin()->minY >= hor_2.begin()->minY)
			return true;
		else
			return false;
	}

	std::list<AcDbEntity *> SortEntitiesByOrientation(const std::list<AcDbEntity *> &allEntities)
	{
		if (allEntities.size() < 2)
			return allEntities;

		//收集所有实体的大小信息到 std::list<_EntityGeometryInfo>
		std::list<_EntityGeometryInfo> allInfos;
		std::list<AcDbEntity *>::const_iterator allEntitiesIter = allEntities.begin();
		for (; allEntitiesIter != allEntities.end(); allEntitiesIter++)
		{
			allInfos.push_back(_ReadEntityGeometryInfo(*allEntitiesIter));
		}
		
		//按照实体起始点的x值排列
		allInfos.sort(_SortEntityInfoByX);
		
		//将所有实体放入一个二维list中
		list<list<_EntityGeometryInfo> >twoDimensionalList;		//二维list： 所有行记录组成的list ， 且每个行里存储有可看成同行的实体
		std::list<_EntityGeometryInfo>::iterator allInfosIter = allInfos.begin();
		for (; allInfosIter != allInfos.end(); allInfosIter++)
		{
			if (allInfosIter->statisticalNumber > 0)
				continue;

			//若二维list为空， 则创建新行记录
			if (twoDimensionalList.size() == 0)
			{
				list<_EntityGeometryInfo> newHor;
				newHor.push_back(*allInfosIter);
				allInfosIter->statisticalNumber++;
				twoDimensionalList.push_back(newHor);
				continue;
			}

			//查找并判断该实体是否应在二维list中已存储的某一行
			list<list<_EntityGeometryInfo> > ::iterator twoDimensionalListIter = twoDimensionalList.begin();
			for (; twoDimensionalListIter != twoDimensionalList.end(); twoDimensionalListIter++)
			{
				//根据Y向的相交性判断 ， 如果存在相交， 则认为在同一行 ， 将实体添加到该行中
				if (_IsTwoEntityYRangeCanInteract(*allInfosIter, *(twoDimensionalListIter->begin())))
				{
					twoDimensionalListIter->push_back(*allInfosIter);
					break;
				}
			}
			if (twoDimensionalListIter == twoDimensionalList.end())		//未找到，创建新行
			{
				list<_EntityGeometryInfo> newHor;
				newHor.push_back(*allInfosIter);
				twoDimensionalList.push_back(newHor);
			}
			allInfosIter->statisticalNumber++;
		}
		
		//二维list的行根据y值进行排序
		twoDimensionalList.sort(_SortEntityHor);

		//遍历二维list， 创建返回值
		std::list<AcDbEntity *> returnEntitiesList;
		list<list<_EntityGeometryInfo> > ::iterator twoDimensionalListIter = twoDimensionalList.begin();
		for (; twoDimensionalListIter != twoDimensionalList.end(); twoDimensionalListIter++)
		{
			list<_EntityGeometryInfo>::iterator entityHorIter = twoDimensionalListIter->begin();
			for (; entityHorIter != twoDimensionalListIter->end(); entityHorIter++)
			{
				returnEntitiesList.push_back(entityHorIter->pEnt);
			}
		}
		return returnEntitiesList;
	}
}
