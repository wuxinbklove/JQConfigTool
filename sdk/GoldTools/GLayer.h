#pragma once
#include "arxHeaders.h"
#include "dbents.h"

#ifndef  GOLD_EXPORT
#define  GOLD_EXPORT
#endif 

namespace GLayer
{

	//检测图层是否存在
	bool GOLD_EXPORT CheckLayer(AcDbDatabase* pDb, CString layerName);

    //获取图层
    AcDbObjectId GOLD_EXPORT GetLayer(AcDbDatabase* pDb, CString& layerName, bool isAutoCreate = false);

	//创建图层
	AcDbObjectId GOLD_EXPORT CreateLayer(AcDbDatabase* pDb, CString& layerName);
	AcDbObjectId GOLD_EXPORT CreateLayer(AcDbDatabase* pDb, CString layerName, int colorIndex, bool isPlot, bool isFrozen = false, bool isOff = false, bool isLock = false, bool isVPDFLT = false);

	//设置图层状态
	bool GOLD_EXPORT SetStatusOfLayer(AcDbDatabase* pDb, CString strLayerName, bool isPlot, bool isOff, bool isFrozen = false, bool isLock = false);


};

namespace GTextStyle
{
	//检测字体样式是否存在
	bool GOLD_EXPORT CheckTextStyle(AcDbDatabase* pDb, CString layerName);

	//获取字体样式
	AcDbObjectId GOLD_EXPORT GetTextStyle(AcDbDatabase* pDb, CString& textStyleName, bool isAutoCreate = false);

	//常见字体样式
	AcDbObjectId GOLD_EXPORT CreateTextStyle(AcDbDatabase* pDb, CString& textStyleName);
}