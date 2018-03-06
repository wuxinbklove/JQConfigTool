#include "stdafx.h"
#include "GLayer.h"
#include "GLockDocument.h"

namespace GLayer
{
	bool CheckLayer(AcDbDatabase* pDb, CString layerName)
	{
		if (pDb == NULL)
		{
			return false;
		}

		AcDbLayerTable* pTable = NULL;
		Acad::ErrorStatus es = pDb->getLayerTable(pTable, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			return false;
		}
		pTable->close();

		if (pTable->has(layerName))
		{
			return true;
		}
		return false;
	}

	AcDbObjectId GetLayer(AcDbDatabase* pDb, CString& layerName , bool isAutoCreate)
	{
		if (pDb == NULL)
		{
			return AcDbObjectId::kNull;
		}

		AcDbLayerTable* pTable = NULL;
		Acad::ErrorStatus es = pDb->getLayerTable(pTable, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			//log
			return AcDbObjectId::kNull;
		}
		pTable->close();
		AcDbObjectId layerId;
		if (pTable->has(layerName))
		{
			pTable->getAt(layerName, layerId);
			return layerId;
		}
		if (isAutoCreate)
			return CreateLayer(pDb, layerName);
		else
			return AcDbObjectId::kNull;
	}

	AcDbObjectId CreateLayer(AcDbDatabase* pDb, CString& layerName)
	{
		if (pDb == NULL)
		{
			return AcDbObjectId::kNull;
		}

		AcDbLayerTable* pTable = NULL;
		Acad::ErrorStatus es = pDb->getLayerTable(pTable, AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			return AcDbObjectId::kNull;
		}

		AcDbLayerTableRecord* pLayer = new AcDbLayerTableRecord();
		pLayer->setName(layerName);
		AcDbObjectId layerId;
		es = pTable->add(layerId, pLayer);
		pLayer->close();

		pTable->close();
		return layerId;
	}

	AcDbObjectId CreateLayer(AcDbDatabase* pDb, CString strLayerName, int colorIndex, bool isPlot, bool isFrozen, bool isOff, bool isLock, bool isVPDFLT)
	{
		GLockDocument lock;
		AcDbObjectId id;
		if (pDb == NULL) pDb = acdbHostApplicationServices()->workingDatabase();

		AcDbLayerTable *pLayerTbl;
		Acad::ErrorStatus es = pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);
		if (es != Acad::eOk)
			return AcDbObjectId::kNull;

		if (!pLayerTbl->has(strLayerName))
		{
			AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord;
			pLayerTblRcd->setName(strLayerName);

			AcCmColor color;		
			color.setColorIndex(colorIndex);
			pLayerTblRcd->setColor(color);

			pLayerTblRcd->setIsPlottable(isPlot);
			pLayerTblRcd->setIsFrozen(isFrozen);
			pLayerTblRcd->setIsOff(isOff); 
			pLayerTblRcd->setIsLocked(isLock);
			pLayerTblRcd->setVPDFLT(isVPDFLT);

			pLayerTbl->add(id, pLayerTblRcd);

			pLayerTblRcd->close();
			pLayerTbl->close();
		}
		else
		{
			AcDbLayerTableRecord *pLayerTblRcd;
			if (pLayerTbl->getAt(strLayerName, pLayerTblRcd, AcDb::kForWrite) != Acad::eOk)
			{
				pLayerTbl->close();
				return AcDbObjectId::kNull;
			}
			pLayerTbl->close();

			AcCmColor color;	
			color.setColorIndex(colorIndex); 
			pLayerTblRcd->setColor(color);

			pLayerTblRcd->setIsPlottable(isPlot);
			pLayerTblRcd->setIsFrozen(isFrozen);
			pLayerTblRcd->setIsOff(isOff); 
			pLayerTblRcd->setIsLocked(isLock);
			pLayerTblRcd->setVPDFLT(isVPDFLT);

			id = pLayerTblRcd->objectId();
			pLayerTblRcd->close();
		}
		return id;
	}


	bool SetStatusOfLayer(AcDbDatabase* pDb, CString strLayerName, bool isPlot, bool isOff, bool isFrozen,bool isLock)
	{
		GLockDocument lock;
		AcDbObjectId id;
		if (pDb == NULL) pDb = acdbHostApplicationServices()->workingDatabase();

		AcDbLayerTable *pLayerTbl;
		Acad::ErrorStatus es = pDb->getLayerTable(pLayerTbl, AcDb::kForWrite);
		if (es != Acad::eOk)
			return AcDbObjectId::kNull;

		if (!pLayerTbl->has(strLayerName))
		{
			pLayerTbl->close();
			return false;
		}

		AcDbLayerTableRecord *pLayerTblRcd;
		if (pLayerTbl->getAt(strLayerName, pLayerTblRcd, AcDb::kForWrite) != Acad::eOk)
		{
			pLayerTbl->close();
			return AcDbObjectId::kNull;
		}
		pLayerTbl->close();

		pLayerTblRcd->setIsOff(isOff); 
		pLayerTblRcd->setIsPlottable(isPlot);

		pLayerTblRcd->setIsFrozen(isFrozen);
		pLayerTblRcd->setIsLocked(isLock);
		pLayerTblRcd->close();
		return true;
	}

	bool GetTextStyle(const CString csTsName, AcDbObjectId& resultId, AcDbDatabase* pDB)
	{
		if (pDB == NULL)
		{
			return false;
		}
		resultId.setNull();

		AcDbTextStyleTable *pTsTbl;
		Acad::ErrorStatus es = pDB->getSymbolTable(pTsTbl, AcDb::kForWrite);
		if(es != Acad::eOk)
		{
			return false;
		}
		pTsTbl->close();

		es = pTsTbl->getAt(csTsName, resultId);
		if(es != Acad::eOk)
		{
			return false;
		}
		return true;
	}



}

namespace GTextStyle
{
	bool CheckTextStyle(AcDbDatabase* pDb, CString layerName)
	{
		if (pDb == NULL)
			return false;

		AcDbTextStyleTable *pTextStyleTbl;
		Acad::ErrorStatus es = pDb->getTextStyleTable(pTextStyleTbl, AcDb::kForRead);
		if (es != Acad::eOk)
			return false;
		pTextStyleTbl->close();
		return pTextStyleTbl->has(layerName);
	}

	AcDbObjectId GetTextStyle(AcDbDatabase* pDb, CString& textStyleName, bool isAutoCreate)
	{
		if (pDb == NULL)
			return AcDbObjectId::kNull;

		AcDbTextStyleTable *pTextStyleTbl;
		Acad::ErrorStatus es = pDb->getTextStyleTable(pTextStyleTbl, AcDb::kForRead);
		if (es != Acad::eOk)
			return AcDbObjectId::kNull;
		pTextStyleTbl->close();

		AcDbObjectId objectId = AcDbObjectId::kNull;
		if (pTextStyleTbl->has(textStyleName))
		{
			pTextStyleTbl->getAt(textStyleName, objectId);
			return objectId;
		}
		
		if (isAutoCreate)
			return CreateTextStyle(pDb, textStyleName);
		return AcDbObjectId::kNull;
	}

	AcDbObjectId CreateTextStyle(AcDbDatabase* pDb, CString& textStyleName)
	{
		if (pDb == NULL)
			return AcDbObjectId::kNull;

		AcDbObjectId curTextStyle = pDb->textstyle();
		if (curTextStyle == AcDbObjectId::kNull)
			return AcDbObjectId::kNull;

		AcDbTextStyleTableRecord *pCurTSRcd;
		Acad::ErrorStatus es = acdbOpenObject(pCurTSRcd, curTextStyle, AcDb::kForRead);
		if (es = Acad::eOk)
			return AcDbObjectId::kNull;
		pCurTSRcd->close();

		AcDbTextStyleTable *pTextStyleTbl;
		es = pDb->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);
		if (es != Acad::eOk)
			return AcDbObjectId::kNull;

		AcDbObjectId newTSId = AcDbObjectId::kNull;
		if (pTextStyleTbl->has(textStyleName))
		{
			es = pTextStyleTbl->getAt(textStyleName, newTSId);
			pTextStyleTbl->close();
			if (es != Acad::eOk)
				return AcDbObjectId::kNull;
			return newTSId;
		}

		AcDbTextStyleTableRecord *pNewTSRcd = new AcDbTextStyleTableRecord();
		pNewTSRcd->setName(textStyleName);
		es = pTextStyleTbl->add(newTSId, pNewTSRcd);
		pNewTSRcd->close();
		pTextStyleTbl->close();
		if (es != Acad::eOk)
			return AcDbObjectId::kNull;
		return newTSId;
	}
}

