#include "StdAfx.h"
#include "AddRoleSignBox.h"
#include "sdk/GoldTools/GLayer.h"
#include "sdk/GoldTools/GLockDocument.h"

AddRoleSignBox* AddRoleSignBox::mInstance = NULL;

AddRoleSignBox* AddRoleSignBox::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new AddRoleSignBox();
	}
	mInstance->readConfig();
	return mInstance;
}

AddRoleSignBox::AddRoleSignBox()
{
	m_isUseSameHyperLinkAndName = true;
}

AddRoleSignBox::~AddRoleSignBox()
{

}

void AddRoleSignBox::readConfig()
{
	m_isUseSameHyperLinkAndName = (bool)::GetPrivateProfileInt(_T("AddRoleSignBox"), _T("IsUseSameHyperLinkAndName"), 1, ConfigFile);
	m_isAutoContinue = (bool)::GetPrivateProfileInt(_T("AddRoleSignBox"), _T("IsAutoContinue"), 0, ConfigFile);
	::GetPrivateProfileString(_T("AddRoleSignBox"), _T("DefaultLayer"), _T(""), m_layerName.GetBuffer(100), 100, ConfigFile);
	m_layerName.ReleaseBuffer();
}

AcDbObjectId AddRoleSignBox::getSelectedLayerId()
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

void AddRoleSignBox::start()
{
	GLockDocument lockDoc;

	Acad::ErrorStatus es;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *pBlockTable;
	es = pDb->getBlockTable(pBlockTable, AcDb::kForRead);
	if (es != Acad::eOk)
		return;
	pBlockTable->close();
	AcDbBlockTableRecord *pBlockTableRecord;
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
		AcDb::kForWrite);
	if (es != Acad::eOk)
		return;

	AcDbObjectId layerId = getSelectedLayerId();
	if (layerId == AcDbObjectId::kNull)
	{
		pBlockTableRecord->close();
		return;
	}

	while (true)
	{
		ads_point pt1, pt2;
		if (acedGetPoint(NULL, _T("\n请选择矩形区域对角线起始位置(左上角): "), pt1) != RTNORM)
		{
			acutPrintf(_T("\n错误：出现未知错误！"));
			pBlockTableRecord->close();
			return;
		}
		if (acedGetCorner(pt1, _T("\n请选择矩形区域对角线终止位置(右下角): "), pt2) != RTNORM)
		{
			acutPrintf(_T("\n错误：出现未知错误！"));
			pBlockTableRecord->close();
			return;
		}
		ads_point pt1_w, pt2_w;
		struct resbuf fromrb, torb;
		fromrb.restype = RTSHORT;
		fromrb.resval.rint = 1; // UCS  
		torb.restype = RTSHORT;
		torb.resval.rint = 0; // WCS  
		ads_trans(pt1, &fromrb, &torb, FALSE, pt1_w);
		ads_trans(pt2, &fromrb, &torb, FALSE, pt2_w);
		if (pt1_w[0] > pt2_w[0])
		{
			double tmp = pt1_w[0];
			pt1_w[0] = pt2_w[0];
			pt2_w[0] = tmp;
		}
		if (pt1_w[1] > pt2_w[1])
		{
			double tmp = pt1_w[1];
			pt1_w[1] = pt2_w[1];
			pt2_w[1] = tmp;
		}
		AcGePoint3d minPt(pt1_w[0], pt1_w[1], 0);
		AcGePoint3d maxPt(pt2_w[0], pt2_w[1], 0);

		AcGePoint2d ptLeftBottom(pt1_w[0], pt1_w[1]);
		AcGePoint2d ptRightBottom(pt2_w[0], pt1_w[1]);
		AcGePoint2d ptRightTop(pt2_w[0], pt2_w[1]);
		AcGePoint2d ptLeftTop(pt1_w[0], pt2_w[1]);

		TCHAR hyperLinkTitle[128] = { 0 };
		TCHAR hyperLink[128] = { 0 };
		if (acedGetString(NULL, _T("\n请输入超链接第一个字段:"), hyperLink) != RTNORM)
		{
			acutPrintf(_T("\n已取消本次添加！"));
			pBlockTableRecord->close();
			return;
		}

		if (m_isUseSameHyperLinkAndName)
		{
			lstrcpy(hyperLinkTitle, hyperLink);
		}
		else if (acedGetString(NULL, _T("\n请输入超链接第二个字段:"), hyperLinkTitle) != RTNORM)
		{
			acutPrintf(_T("\n已取消本次添加！"));
			pBlockTableRecord->close();
			return;
		}

		AcDbPolyline *pPoly = new AcDbPolyline(4);
		pPoly->addVertexAt(0, ptLeftBottom, 0);
		pPoly->addVertexAt(1, ptRightBottom, 0);
		pPoly->addVertexAt(2, ptRightTop, 0);
		pPoly->addVertexAt(3, ptLeftTop, 0);
		pPoly->setClosed(Adesk::kTrue);
		if (pPoly->setLayer(layerId) != Acad::eOk)
		{
			acutPrintf(_T("\n警告：添加到图层GoldSignBox失败！"));
			pBlockTableRecord->close();
			delete pPoly;
			return;
		}

		AcDbObjectId rectId;
		pBlockTableRecord->appendAcDbEntity(rectId, pPoly);

		acdbRegApp(_T("PE_URL"));
		struct resbuf* pRb = acutBuildList(AcDb::kDxfRegAppName, _T("PE_URL"),
			AcDb::kDxfXdAsciiString, hyperLink,
			AcDb::kDxfXdControlString, _T("{"),
			AcDb::kDxfXdAsciiString, hyperLinkTitle,
			AcDb::kDxfXdControlString, _T("{"),
			AcDb::kDxfXdInteger32, 1,
			AcDb::kDxfXdControlString, _T("}"),
			AcDb::kDxfXdControlString, _T("}"),
			RTNONE);
		pPoly->setXData(pRb);
		pPoly->close();
		acutRelRb(pRb);

		if (!m_isAutoContinue)
			break;

		acutPrintf(_T("\n自动进入下一个添加，按ESC取消\n"));

		/*TCHAR isContineChar[10] = {0};
		bool isContinue = false;
		if (acedGetString(NULL, _T("\n是否继续选取[是(Y)/否(N)] (Y)"), isContineChar) != RTNORM)
		{
			acutPrintf(_T("\n\n错误：选项出现未知错误，程序自动中断拾取！"));
			break;
		}

		CString isContinueStr(isContineChar);
		if ((isContinueStr.CompareNoCase(_T("Y")) == 0) || isContinueStr.IsEmpty())
		{
			isContinue = true;
		}
		else  if(isContinueStr.CompareNoCase(_T("N")) == 0)
		{
			isContinue = false;
		}
		else
		{
			isContinue = false;
			acutPrintf(_T("\n\n警告：输入了未知选项，默认已取消！"));
		}
		if (!isContinue)
			break;*/
	}

	pBlockTableRecord->close();
}