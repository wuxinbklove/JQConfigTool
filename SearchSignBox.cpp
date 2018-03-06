// SearchSignBox.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchSignBox.h"
#include "sdk/GoldTools/GAttrMgr.h"
#include "sdk/GoldTools/GCharSet.h"
#include "sdk/GoldTools/GPath.h"

#if _MSC_VER>=1600
#include <regex>
#define REGEXSPACE(method) std::##method
#elif _MSC_VER >= 1400
#include "boost/regex.hpp"
#define REGEXSPACE(method) boost::##method
#endif

// SearchSignBox 对话框

IMPLEMENT_DYNAMIC(SearchSignBox, CAcUiDialog)

SearchSignBox::SearchSignBox(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(IDD_DIALOG_SEARCHSIGNBOX, pParent)
{

}

SearchSignBox::~SearchSignBox()
{
}

void SearchSignBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SIGNBOXINFOS, m_listCtrl);
}


BEGIN_MESSAGE_MAP(SearchSignBox, CAcUiDialog)
	ON_BN_CLICKED(IDC_BUTTON_STARTSEARCH, &SearchSignBox::OnBnClickedButtonStartsearch)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_BUTTON_HELP, &SearchSignBox::OnBnClickedButtonHelp)
END_MESSAGE_MAP()

BOOL SearchSignBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(acedGetAcadFrame()->GetIcon(FALSE), FALSE);
	SetIcon(acedGetAcadFrame()->GetIcon(TRUE), TRUE);
	m_listCtrl.InsertColumn(0, _T("签名字段"), LVCFMT_LEFT , 100);
	m_listCtrl.InsertColumn(1, _T("所属块名"), LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(2, _T("左下点位置"), LVCFMT_LEFT, 150);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	return TRUE;
}

void SearchSignBox::RefreshListCtrl()
{
	m_listCtrl.DeleteAllItems();
	list<SignBoxInfo>::iterator iter = m_searchedSignBoxList.begin();
	int row = 0;
	for (; iter != m_searchedSignBoxList.end(); iter++)
	{
		m_listCtrl.InsertItem(row, iter->title);
		m_listCtrl.SetItemText(row, 1, iter->blockName);
		m_listCtrl.SetItemText(row, 2, iter->origin);
		row++;
	}
}


void SearchSignBox::OnBnClickedButtonStartsearch()
{
	m_searchedSignBoxList.clear();
	CString signBoxTitle;
	GetDlgItem(IDC_EDIT_SIGNBOXTITLE)->GetWindowText(signBoxTitle);
	if (signBoxTitle.IsEmpty())
	{
		RefreshListCtrl();
		return;
	}
	

	AcDbBlockTable *pTable;
	Acad::ErrorStatus es;
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, AcDb::kForRead);
	if (es != Acad::eOk)
	{
		RefreshListCtrl();
		return;
	}
	pTable->close();
	AcDbBlockTableIterator *pTableIter;
	pTable->newIterator(pTableIter);
	for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		AcDbBlockTableRecord *pTableRcd;
		es = pTableIter->getRecord(pTableRcd, AcDb::kForRead);
		if (es != Acad::eOk)
			continue;
		pTableRcd->close();
		AcDbBlockTableRecordIterator *pTableRcdIter;
		pTableRcd->newIterator(pTableRcdIter);
		for (pTableRcdIter->start(); !pTableRcdIter->done(); pTableRcdIter->step())
		{
			AcDbEntity *pEnt;
			es = pTableRcdIter->getEntity(pEnt, AcDb::kForRead);
			if (es != Acad::eOk)
				continue;
			pEnt->close();
			CString roleName = GAttrMgr::GetRole(pEnt);
#if _MSC_VER >= 1400
			try
			{
				REGEXSPACE(regex) reg(GCharSet::MFC2STL(signBoxTitle));
				if (!REGEXSPACE(regex_match(GCharSet::MFC2STL(roleName), reg)))
					continue;
			}
			catch (...)
			{
				AfxMessageBox(_T("正则表达式语法错误！"));
				return;
			}
#else

			if (roleName.CompareNoCase(signBoxTitle) != 0)
			{
				continue;
			}
#endif

			SignBoxInfo signBoxInfo;
			ACHAR *blockName;
			pTableRcd->getName(blockName);
			AcDbExtents extents;
			pEnt->getGeomExtents(extents);
			signBoxInfo.title = roleName;
			signBoxInfo.blockName = blockName;
			signBoxInfo.origin.Format(_T("%.2lf , %.2lf"), extents.minPoint().x, extents.minPoint().y);
			m_searchedSignBoxList.push_back(signBoxInfo);
		}
	}
	RefreshListCtrl();
}

LRESULT SearchSignBox::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


void SearchSignBox::OnBnClickedButtonHelp()
{
	GPath::OpenUserManual(_T("SearchRolePageId"));
}
