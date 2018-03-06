#pragma once

#include "arxHeaders.h"
#include <list>
using std::list;

#ifndef  GOLD_EXPORT
#define  GOLD_EXPORT
#endif 


/*!
 @brief 几何相关的方法
 */
namespace GGeometry
{
    /*!
     @brief 获取块参照的变换矩阵
     @param
     @return
     */
    AcGeMatrix3d GOLD_EXPORT GetBlockMatrix3d(AcDbBlockReference* pRef,
                                  AcDbBlockTableRecord* pRec);
	AcGeMatrix3d GOLD_EXPORT GetBlockMatrix3d(AcDbBlockReference* pRef);

    /*!
     @brief
     @param
     @return
     */
    AcGePoint3d GOLD_EXPORT Mid(AcGePoint3d& pnt1, AcGePoint3d& pnt2);

    /*!
     @brief 判断是否相交
     @param
     @return
     */
    bool GOLD_EXPORT IsIntersect(AcDbExtents& ext1, AcDbExtents& ext2);

	/*!
	 @brief 判断ext1是否包含ext2（不包括边界）
	 */
	bool GOLD_EXPORT IsIn(AcDbExtents* ext1,AcDbExtents* ext2);
	bool GOLD_EXPORT IsIn(AcDbExtents ext1, AcDbExtents ext2);
	/*!
	@brief 判断ext1是否包含ext2（包括边界）
	*/
	bool GOLD_EXPORT IsInclude(AcDbExtents ext1, AcDbExtents ext2);

	//判断ext1 是否和 ext2相等
	bool GOLD_EXPORT isSameExtents(AcDbExtents* ext1, AcDbExtents* ext2);
	bool GOLD_EXPORT isSameExtents(AcDbExtents ext1, AcDbExtents ext2);

	//是否紧靠
	bool GOLD_EXPORT isAdjoin(AcDbExtents* ext1, AcDbExtents* ext2);
	bool GOLD_EXPORT isAdjoin(AcDbExtents ext1, AcDbExtents ext2);

	//compareExtent是否在ext 左、右、上、下紧靠
	bool GOLD_EXPORT isLeftAdjoin(AcDbExtents ext, AcDbExtents compareExtent);
	bool GOLD_EXPORT isRightAdjoin(AcDbExtents ext, AcDbExtents compareExtent);
	bool GOLD_EXPORT isTopAdjoin(AcDbExtents ext, AcDbExtents compareExtent);
	bool GOLD_EXPORT isBottomAdjoin(AcDbExtents ext, AcDbExtents compareExtent);

	/*!
	@brief 判断point点是否在ext的范围内
	*/
	bool GOLD_EXPORT IsPointInExtents(const AcGePoint3d& point , const AcDbExtents& ext);

	enum IntersectionType {NoIntersection = 0x0001 ,			//无相交点
											LeftIntersection = 0x0002 ,			//相交于左边界
											RightIntersection = 0x0004,		//相交于右边界
											TopIntersection = 0x0008,			//相交于上边界
											BottomIntersection = 0x0010,	//相交于下边界
											HorInContainment = 0x0020,		//水平方向上坐标有内包含关系
											HorOutContainment = 0x0040,	//水平方向上坐标有外包含关系
											VerInContainment = 0x0080,		//垂直方向上坐标有内包含关系
											VerOutContainment = 0x0100};	//垂直方向上坐标有外包含关系
	typedef unsigned long IntersectionTypes;

	/*!
	@brief 判断ext1相对于ext2的所在的位置，返回值包含IntersectionType的一个或多个 , offsetX、offsetY为允许误差值 , 为正值时，适当扩大ext2的范围，否则减小。
	*/
	IntersectionTypes GOLD_EXPORT TwoExtentsIntersection(const AcDbExtents& ext1, const AcDbExtents& ext2 , double offsetX = 0.0 , double offsetY = 0.0);


	//计算逆矩阵
	void GOLD_EXPORT InverseMatrix(AcGeMatrix3d &matrix);

	//根据相对位置排序重新排序实体列表(从左往右，从上往下)
	std::list<AcDbEntity *> GOLD_EXPORT SortEntitiesByOrientation(const std::list<AcDbEntity *> &allEntities);
}