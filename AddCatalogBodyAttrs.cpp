#include "StdAfx.h"
#include "AddCatalogBodyAttrs.h"
#include "math.h"
#include "sdk/GoldTools/GLayer.h"
#include "sdk/GoldTools/GAttrMgr.h"
#include "sdk/GoldTools/GLockDocument.h"

AcGePoint3d GetPos(int attachment, AcGePoint3d& lt, AcGePoint3d& rb)
{
	AcGePoint3d pos;
	switch (attachment)
	{
	case 4:
		pos.x = lt.x;
		pos.y = (lt.y + rb.y) / 2;
		break;

	case 5:
		pos.x = (lt.x + rb.x) / 2;
		pos.y = (lt.y + rb.y) / 2;
		break;

	default:
		break;
	}
	return pos;
}

AddCatalogBodyAttrs* AddCatalogBodyAttrs::mInstance = NULL;
AddCatalogBodyAttrs* AddCatalogBodyAttrs::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new AddCatalogBodyAttrs();
	}
	mInstance->readConfig();
	return mInstance;
}

AddCatalogBodyAttrs::AddCatalogBodyAttrs()
{
	m_isContentUseNumberSuffix = false;
}


AddCatalogBodyAttrs::~AddCatalogBodyAttrs()
{
}


void AddCatalogBodyAttrs::readConfig()
{
	::GetPrivateProfileString(_T("AddCatalogBodyAttrs"), _T("DefaultLayer"), _T(""), m_layerName.GetBuffer(100), 100, ConfigFile);
	m_layerName.ReleaseBuffer();
	::GetPrivateProfileString(_T("AddCatalogBodyAttrs"), _T("DefaultTextStyle"), _T(""), m_textStyleName.GetBuffer(100), 100, ConfigFile);
	m_textStyleName.ReleaseBuffer();

	CString leftOffsetStr, rightOffsetStr;
	::GetPrivateProfileString(_T("AddCatalogBodyAttrs"), _T("LeftOffset"), _T("0.2"), leftOffsetStr.GetBuffer(100), 100, ConfigFile);
	::GetPrivateProfileString(_T("AddCatalogBodyAttrs"), _T("RightOffset"), _T("0.0"), rightOffsetStr.GetBuffer(100), 100, ConfigFile);
	leftOffsetStr.ReleaseBuffer();
	rightOffsetStr.ReleaseBuffer();
	m_leftOffset = _ttof(leftOffsetStr);
	m_rightOffset = _ttof(rightOffsetStr);
	m_isContentUseNumberSuffix = (bool)::GetPrivateProfileInt(_T("AddCatalogBodyAttrs"), _T("IsContentUseNumberSuffix"), 0, ConfigFile);
}

AcDbObjectId AddCatalogBodyAttrs::getSelectedLayerId()
{
	AcDbObjectId layerRcdId = AcDbObjectId::kNull;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	if (pDb == NULL)
		return AcDbObjectId::kNull;
	if (m_layerName.IsEmpty())
		return acdbHostApplicationServices()->workingDatabase()->clayer();
	if (!GLayer::CheckLayer(pDb, m_layerName))
	{
		layerRcdId = GLayer::CreateLayer(pDb, m_layerName);
	}
	else
	{
		layerRcdId = GLayer::GetLayer(pDb, m_layerName);
	}
	return layerRcdId;
}

AcDbObjectId AddCatalogBodyAttrs::getSelectedTextStyleId()
{
	AcDbObjectId textStyleId = AcDbObjectId::kNull;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	if (pDb == NULL)
		return AcDbObjectId::kNull;
	if (m_textStyleName.IsEmpty())
		return acdbHostApplicationServices()->workingDatabase()->textstyle();
	if (!GTextStyle::CheckTextStyle(pDb, m_textStyleName))
	{
		textStyleId = GTextStyle::CreateTextStyle(pDb, m_textStyleName);
	}
	else
	{
		textStyleId = GTextStyle::GetTextStyle(pDb, m_textStyleName);
	}
	return textStyleId;
}


bool AddCatalogBodyAttrs::start()
{
	GLockDocument lockDoc;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

	AcDbObjectId layerId = getSelectedLayerId();
	AcDbObjectId textStyleId = getSelectedTextStyleId();

	ads_point p1, p2, p3;
	if (acedGetPoint(NULL, _T("\n请选择第一行属性文字的左上点: "), p1) != RTNORM)
	{
		return false;
	}
	if (acedGetPoint(p1, _T("\n请选择第一行属性文字的右下点: "), p3) != RTNORM)
	{
		return false;
	}
	if (acedGetPoint(p1, _T("\n请选择最后一行属性文字的右下点: "), p2) != RTNORM)
	{
		return false;
	}

	TCHAR lpResult[128] = { 0 };
	if (acedGetString(NULL, _T("\n请输入扩展数据(属性):"), lpResult) != RTNORM)
	{
		return false;
	}
	ads_real height;
	if (acedGetReal(_T("请输入文本高度:"), &height) != RTNORM)
	{
		return false;
	}
	TCHAR lpContent[128] = { 0 };
	if (acedGetString(NULL, _T("请输入默认文本内容："), lpContent) != RTNORM)
	{
		return false;
	}
	int attachment;
	if (acedGetInt(_T("请输入多行文字对齐方式(4:左中点;5:中心点):"), &attachment) != RTNORM)
	{
		return false;
	}

	ads_point pt1_w, pt2_w, pt3_w;
	struct resbuf fromrb, torb;
	fromrb.restype = RTSHORT;
	fromrb.resval.rint = 1; // UCS  
	torb.restype = RTSHORT;
	torb.resval.rint = 0; // WCS  
	ads_trans(p1, &fromrb, &torb, FALSE, pt1_w);
	ads_trans(p2, &fromrb, &torb, FALSE, pt2_w);
	ads_trans(p3, &fromrb, &torb, FALSE, pt3_w);

	double cellWidth = fabs(p3[X] - p1[X]);
	double cellHeight = fabs(p1[Y] - p3[Y]);
	double totalHeight = fabs(p1[Y] - p2[Y]);

	if (cellWidth <= 0 || cellHeight <= 0 || totalHeight <= 0)
		return false;
	
	AcDbBlockTableRecordPointer modelRcd(ACDB_MODEL_SPACE, acdbHostApplicationServices()->workingDatabase(), AcDb::kForWrite);
	if (modelRcd.openStatus() != Acad::eOk)
		return false;

	for (int i = 0; fabs(cellHeight*(i + 1)) < fabs(totalHeight + cellHeight / 2); ++i)
	{
		AcGePoint3d ptLT(p1[X] + m_leftOffset, p1[Y] - i * cellHeight, 0.0);
		AcGePoint3d ptRB(p1[X] + cellWidth - m_rightOffset, p1[Y] - (i + 1) * cellHeight, 0.0);

		CString contents;
		if (m_isContentUseNumberSuffix)
			contents.Format(_T("%s%d"), lpContent, i + 1);
		else
			contents.Format(_T("%s"), lpContent);

		AcDbMText* pMText = new AcDbMText();
		pMText->setAttachment((AcDbMText::AttachmentPoint)attachment);
		pMText->setContents(contents);
		pMText->setLocation(GetPos(attachment, ptLT, ptRB));

		pMText->setLayer(layerId);
		pMText->setTextStyle(textStyleId);

		pMText->setTextHeight(height);
#if _MSC_VER>=1400
		pMText->setHeight(cellHeight);
#endif
		pMText->setWidth(cellWidth - m_leftOffset - m_rightOffset);

		AcDbObjectId objectId = AcDbObjectId::kNull;
		Acad::ErrorStatus es = (modelRcd.object())->appendAcDbEntity(objectId, pMText);
		if (es != Acad::eOk)
		{
			(modelRcd.object())->close();
			pMText->close();
			return false;
		}

		CString strKey;
		strKey.Format(_T("%s%d"), lpResult, i + 1);
		GAttrMgr::SetAttrKey(pMText, strKey);
		pMText->close();
	}
	(modelRcd.object())->close();
	return true;
}