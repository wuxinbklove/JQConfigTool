// MultiSearchAndEditMText.cpp : 实现文件
//

#include "stdafx.h"
#include "MultiSearchAndEditMText.h"
#include "resource.h"
#include "sdk/GoldTools/GAttrMgr.h"
#include "sdk/GoldTools/GCharSet.h"
#include "sdk/GoldTools/GPath.h"
#include "sdk/GridCtrl/GridCellCombo.h"

#if _MSC_VER>=1600
#include <regex>
#define REGEXSPACE(method) std::##method
#elif _MSC_VER >= 1400
#include "boost/regex.hpp"
#define REGEXSPACE(method) boost::##method
#endif


// MultiSearchAndEditMText 对话框

IMPLEMENT_DYNAMIC(MultiSearchAndEditMText, CDialog)

MultiSearchAndEditMText::MultiSearchAndEditMText(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_MULTIMODIFYMTEXT, pParent)
{
}

MultiSearchAndEditMText::~MultiSearchAndEditMText()
{
}

void MultiSearchAndEditMText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MultiSearchAndEditMText, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &MultiSearchAndEditMText::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &MultiSearchAndEditMText::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_USERMANUAL, &MultiSearchAndEditMText::OnBnClickedButtonUsermanual)
END_MESSAGE_MAP()

// MultiSearchAndEditMText 消息处理程序
bool MultiSearchAndEditMText::LoadLayerData()
{
	m_allLayers.clear();
	AcDbLayerTable *pLayerTbl;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl , AcDb::kForRead);
	if (es != Acad::eOk)
		return false;
	pLayerTbl->close();
	AcDbLayerTableIterator *pLayerTblIter;
	es = pLayerTbl->newIterator(pLayerTblIter);
	if (es != Acad::eOk)
		return false;
	for (pLayerTblIter->start(); !pLayerTblIter->done(); pLayerTblIter->step())
	{
		AcDbLayerTableRecord *pLayerTblRcd;
		es = pLayerTblIter->getRecord(pLayerTblRcd, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			delete pLayerTblIter;
			return false;
		}
		pLayerTblRcd->close();
		ACHAR *layerName;
		pLayerTblRcd->getName(layerName);
		m_allLayers.insert(std::pair<CString , AcDbObjectId>(layerName, pLayerTblRcd->objectId()));
	}
	delete pLayerTblIter;

	return true;
}

bool MultiSearchAndEditMText::LoadTextStyleData()
{
	m_allTextStyles.clear();
	AcDbTextStyleTable *pTextStyleTbl;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForRead);
	if (es != Acad::eOk)
		return false;
	pTextStyleTbl->close();
	AcDbTextStyleTableIterator *pTextStyleTblIter;
	es = pTextStyleTbl->newIterator(pTextStyleTblIter);
	if (es != Acad::eOk)
		return false;
	for (pTextStyleTblIter->start(); !pTextStyleTblIter->done(); pTextStyleTblIter->step())
	{
		AcDbTextStyleTableRecord *pTextStyleTblRcd;
		es = pTextStyleTblIter->getRecord(pTextStyleTblRcd, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			delete pTextStyleTblIter;
			return false;
		}
		pTextStyleTblRcd->close();
		ACHAR *textStyleName;
		pTextStyleTblRcd->getName(textStyleName);
		m_allTextStyles.insert(std::pair<CString, AcDbObjectId>(textStyleName, pTextStyleTblRcd->objectId()));
	}
	delete pTextStyleTblIter;
	return true;
}

bool MultiSearchAndEditMText::LoadBlockData()
{
	m_allBlocks.clear();
	AcDbBlockTable *pBlockTbl;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTbl, AcDb::kForRead);
	if (es != Acad::eOk)
		return false;
	pBlockTbl->close();
	AcDbBlockTableIterator *pBlockTblIter;
	es = pBlockTbl->newIterator(pBlockTblIter);
	if (es != Acad::eOk)
		return false;
	for (pBlockTblIter->start(); !pBlockTblIter->done(); pBlockTblIter->step())
	{
		AcDbBlockTableRecord *pBlockTblRcd;
		es = pBlockTblIter->getRecord(pBlockTblRcd, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			delete pBlockTblIter;
			return false;
		}
		pBlockTblRcd->close();
		ACHAR *blockName;
		pBlockTblRcd->getName(blockName);
		m_allBlocks.insert(std::pair<CString, AcDbObjectId>(blockName, pBlockTblRcd->objectId()));
	}
	delete pBlockTblIter;
	return true;
}

void MultiSearchAndEditMText::InitCombos()
{
	CComboBox *srcBlockCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SRCBLOCK);
	CComboBox *srcLayerCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SRCLAYER);
	CComboBox *srcTextStyleCombo = (CComboBox *)GetDlgItem(IDC_COMBO_SRCTEXTSTYLE);
	CComboBox *destLayerCombo = (CComboBox *)GetDlgItem(IDC_COMBO_DESTLAYER);
	CComboBox *destTextStyleCombo = (CComboBox *)GetDlgItem(IDC_COMBO_DESTTEXTSTYLE);

	srcBlockCombo->ResetContent();
	srcLayerCombo->ResetContent();
	srcTextStyleCombo->ResetContent();
	destLayerCombo->ResetContent();
	destTextStyleCombo->ResetContent();

	map<CString, AcDbObjectId>::iterator blockIter = m_allBlocks.begin();
	for (; blockIter != m_allBlocks.end(); blockIter++)
	{
		srcBlockCombo->AddString(blockIter->first);
	}
	srcBlockCombo->SetCurSel(0);

	map<CString, AcDbObjectId>::iterator layerIter = m_allLayers.begin();
	for (; layerIter != m_allLayers.end(); layerIter++)
	{
		srcLayerCombo->AddString(layerIter->first);
		destLayerCombo->AddString(layerIter->first);
	}
	resbuf clyr;
	acedGetVar(_T("clayer"), &clyr);
	srcLayerCombo->SelectString(0, clyr.resval.rstring);
	destLayerCombo->SelectString(0, clyr.resval.rstring);

	map<CString, AcDbObjectId>::iterator textStyleIter = m_allTextStyles.begin();
	for (; textStyleIter != m_allTextStyles.end(); textStyleIter++)
	{
		srcTextStyleCombo->AddString(textStyleIter->first);
		destTextStyleCombo->AddString(textStyleIter->first);
	}
	resbuf textStyle;
	acedGetVar(_T("textstyle"), &textStyle);
	srcTextStyleCombo->SelectString(0, textStyle.resval.rstring);
	destTextStyleCombo->SelectString(0, textStyle.resval.rstring);
}

void MultiSearchAndEditMText::SearchMText()
{
	m_allSearchedMTexts.clear();
	bool isBlockFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCBLOCK))->GetCheck();
	bool isLayerFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCLAYER))->GetCheck();
	bool isTextStyleFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCTEXTSTYLE))->GetCheck();
	bool isXdataFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCXDATA))->GetCheck();
	bool isContentFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCCONTENT))->GetCheck();

	CString blockFilter, layerFilter, textStyleFilter, xdataFilter, contentFilter;
	if (isBlockFilter)
		GetDlgItem(IDC_COMBO_SRCBLOCK)->GetWindowText(blockFilter);
	if (isLayerFilter)
		GetDlgItem(IDC_COMBO_SRCLAYER)->GetWindowText(layerFilter);
	if (isTextStyleFilter)
		GetDlgItem(IDC_COMBO_SRCTEXTSTYLE)->GetWindowText(textStyleFilter);
	if (isXdataFilter)
		GetDlgItem(IDC_EDIT_SRCXDATA)->GetWindowText(xdataFilter);
	if (isContentFilter)
		GetDlgItem(IDC_EDIT_SRCCONTENT)->GetWindowText(contentFilter);

	AcDbBlockTable *pBlockTbl;
	Acad:ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTbl, AcDb::kForRead);
	if (es != Acad::eOk)
		return;
	pBlockTbl->close();
	AcDbBlockTableIterator *pBlockTblIter;
	pBlockTbl->newIterator(pBlockTblIter);
	for (pBlockTblIter->start(); !pBlockTblIter->done(); pBlockTblIter->step())
	{
		AcDbBlockTableRecord *pBlockTblRcd;
		es = pBlockTblIter->getRecord(pBlockTblRcd, AcDb::kForRead);
		if (es != Acad::eOk)
			continue;
		pBlockTblRcd->close();
		ACHAR *blockName;
		pBlockTblRcd->getName(blockName);
		if (isBlockFilter && (blockFilter.CompareNoCase(blockName) != 0))
			continue;

		AcDbBlockTableRecordIterator *pBlockTblRcdIter;
		es = pBlockTblRcd->newIterator(pBlockTblRcdIter);
		if (es != Acad::eOk)
			continue;

		for (pBlockTblRcdIter->start(); !pBlockTblRcdIter->done(); pBlockTblRcdIter->step())
		{
			AcDbEntity *pEnt;
			es = pBlockTblRcdIter->getEntity(pEnt, AcDb::kForRead);
			if (es != Acad::eOk)
				continue;
			pEnt->close();

			if (!pEnt->isKindOf(AcDbMText::desc()))
				continue;

			AcDbMText *pMText = AcDbMText::cast(pEnt);
			if (isLayerFilter && (m_allLayers[layerFilter] != pMText->layerId()))
				continue;

			if (isTextStyleFilter && (m_allTextStyles[textStyleFilter] != pMText->textStyle()))
				continue;
#if _MSC_VER >= 1400
			if (isContentFilter)
			{
				CString contents = pMText->contents();
				CString contentsReg;
				GetDlgItem(IDC_EDIT_SRCCONTENT)->GetWindowText(contentsReg);
				try
				{	
					REGEXSPACE(regex) reg(GCharSet::MFC2STL(contentsReg));
					if (!REGEXSPACE(regex_match(GCharSet::MFC2STL(contents), reg)))
						continue;
				}
				catch (...)
				{
					AfxMessageBox(_T("正则表达式语法错误！"));
					return;
				}

			}

			if (isXdataFilter)
			{
				CString xdata = GAttrMgr::GetAttrKey(pMText);
				CString xdataReg;
				GetDlgItem(IDC_EDIT_SRCXDATA)->GetWindowText(xdataReg);
				try
				{
					REGEXSPACE(regex) reg(GCharSet::MFC2STL(xdataReg));
					if (!REGEXSPACE(regex_match(GCharSet::MFC2STL(xdata), reg)))
						continue;
				}
				catch (...)
				{
					AfxMessageBox(_T("正则表达式语法错误！"));
					return;
				}

			}
#endif
			m_allSearchedMTexts.push_back(pMText);
		}
		delete pBlockTblRcdIter;
	}
	delete pBlockTblIter;
}

list<MultiEditMTextProcess::MTextNewInfo> MultiSearchAndEditMText::CalcNewInfos()
{
	list<MultiEditMTextProcess::MTextNewInfo> infos;
	list<AcDbMText*>::iterator mTextsIter = m_allSearchedMTexts.begin();
	for (; mTextsIter != m_allSearchedMTexts.end(); mTextsIter++)
	{
		MultiEditMTextProcess::MTextNewInfo infoItem;
		infoItem.pMText = *mTextsIter;
		if (((CButton *)GetDlgItem(IDC_CHECK_DESTLAYER))->GetCheck())
			GetDlgItem(IDC_COMBO_DESTLAYER)->GetWindowText(infoItem.newLayerName);
		else
			infoItem.newLayerName.Empty();
		if (((CButton *)GetDlgItem(IDC_CHECK_DESTTEXTSTYLE))->GetCheck())
			GetDlgItem(IDC_COMBO_DESTTEXTSTYLE)->GetWindowText(infoItem.newTextStyleName);
		else
			infoItem.newTextStyleName.Empty();

		CString curContents = (*mTextsIter)->contents();
		if (((CButton *)GetDlgItem(IDC_CHECK_DESTCONTENT))->GetCheck())
			GetDlgItem(IDC_EDIT_DESTCONTENT)->GetWindowText(infoItem.newContents);
		else
			infoItem.newContents = curContents;
		
		if (((CButton *)GetDlgItem(IDC_CHECK_DESTCLEARFORMAT))->GetCheck())
		{
			AcDbMText *mtext_t = new AcDbMText();
			GAttrMgr::SetMText(mtext_t, infoItem.newContents);
			infoItem.newContents = GAttrMgr::GetMText(mtext_t);
			delete mtext_t;
		}

		infoItem.newXdata = GAttrMgr::GetAttrKey(*mTextsIter);
		
		infos.push_back(infoItem);
	}
	return infos;
}


BOOL MultiSearchAndEditMText::OnInitDialog()
{
#if _MSC_VER < 1400
	GetDlgItem(IDC_CHECK_SRCCONTENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_SRCXDATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SRCCONTENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SRCXDATA)->EnableWindow(FALSE);
#endif
	SetIcon(acedGetAcadFrame()->GetIcon(FALSE), FALSE);
	SetIcon(acedGetAcadFrame()->GetIcon(TRUE), TRUE);
	InitCombos();
	return TRUE;
}

HBRUSH MultiSearchAndEditMText::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void MultiSearchAndEditMText::OnBnClickedOk()
{
	bool isXdataFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCXDATA))->GetCheck();
	bool isContentFilter = (bool)((CButton *)GetDlgItem(IDC_CHECK_SRCCONTENT))->GetCheck();

	if (isContentFilter)
	{
		CString contentsReg;
		GetDlgItem(IDC_EDIT_SRCCONTENT)->GetWindowText(contentsReg);
		if (contentsReg.IsEmpty())
		{
			AfxMessageBox(_T("正则表达式请勿填空！"));
			return;
		}
	}

	if (isXdataFilter)
	{
		CString xdataReg;
		GetDlgItem(IDC_EDIT_SRCXDATA)->GetWindowText(xdataReg);
		if (xdataReg.IsEmpty())
		{
			AfxMessageBox(_T("正则表达式请勿填空！"));
			return;
		}
	}

	SearchMText();
	MultiEditMTextProcess editPrcess;
	editPrcess.LoadData(m_allBlocks, m_allLayers, m_allTextStyles, CalcNewInfos());
	editPrcess.DoModal();
}

void MultiSearchAndEditMText::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


BEGIN_MESSAGE_MAP(MultiEditMTextProcess, BaseGridEditor)
ON_NOTIFY(NM_CLICK, IDC_GRID_GRIDEDITOR, OnGridClick)
ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_GRIDEDITOR, OnGridEndEdit)
END_MESSAGE_MAP()

MultiEditMTextProcess::MultiEditMTextProcess(CWnd* pParent)
	:BaseGridEditor(pParent)
{
	m_gridActiveCombo = NULL;
}


MultiEditMTextProcess::~MultiEditMTextProcess()
{
}

bool MultiEditMTextProcess::InitGrid()
{
	HDC hdc = ::GetDC(m_hWnd);
	double xScale = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0;
	double yScale = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0;

	SetWindowText(_T("批量修改多行文字"));
	SetWindowPos(0, 0, 0, 1000 * xScale, 550 * yScale, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowMinSize(CSize(1000 * xScale, 400 * yScale));
	CenterWindow();
	GetDlgItem(IDC_BUTTON_USERMANUAL)->ShowWindow(SW_HIDE);

	CGridCell* pCell = new CGridCell();
	mGrid.InsertColumn(_T("旧文本内容"));
	mGrid.InsertColumn(_T("旧xdata"));
	mGrid.InsertColumn(_T("旧块记录"));
	mGrid.InsertColumn(_T("旧图层"));
	mGrid.InsertColumn(_T("旧文字样式"));

	mGrid.InsertColumn(_T("新文本内容"));
	mGrid.InsertColumn(_T("新xdata"));
	mGrid.InsertColumn(_T("新图层"));
	mGrid.InsertColumn(_T("新文字样式"));

	mGrid.SetColumnWidthPercent(OldContents, 10);
	mGrid.SetColumnWidthPercent(OldXdata, 6);
	mGrid.SetColumnWidthPercent(OldBlockName, 6);
	mGrid.SetColumnWidthPercent(OldLayerName, 6);
	mGrid.SetColumnWidthPercent(OldTextStyleName, 6);
	mGrid.SetColumnWidthPercent(NewContents, 10);
	mGrid.SetColumnWidthPercent(NewXdata, 8);
	mGrid.SetColumnWidthPercent(NewLayerName, 8);
	mGrid.SetColumnWidthPercent(NewTextStyleName, 8);
	mGrid.ExpandColumnsToFit();

	mGrid.SetFixedRowCount(1);
	//mGrid.SetFixedColumnCount(1);
	mGrid.SetGridBkColor(RGB(255, 255, 255));
	mGrid.EnableSelection(FALSE);
	mGrid.SetTrackFocusCell(FALSE);
	mGrid.SetFrameFocusCell(FALSE);
	UpdateGrid();	
	return true;
}


void MultiEditMTextProcess::UpdateGrid()
{
	mGrid.DeleteNonFixedRows();
	list<MTextNewInfo>::iterator infosIter = m_allMTextsInfos.begin();
	for (; infosIter != m_allMTextsInfos.end(); infosIter++)
	{
		int row = mGrid.InsertRow(infosIter->pMText->contents());

		for (size_t col = OldContents; col < NewContents; col++)
		{
			CGridCellBase *pCell;
			pCell = mGrid.GetCell(row, col);
			pCell->SetFormat(DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
			pCell->SetState(GVIS_READONLY);
			pCell->SetTextClr(RGB(80, 80, 80));
			pCell->SetBackClr(RGB(245, 245, 245));
		}

		mGrid.SetItemText(row, OldXdata, GAttrMgr::GetAttrKey(infosIter->pMText));
		
		//获取当前所在块名
		AcDbBlockTableRecord *pBlockRcd;
		Acad::ErrorStatus es = acdbOpenObject(pBlockRcd, infosIter->pMText->blockId(), AcDb::kForRead);
		if (es == Acad::eOk)
		{
			pBlockRcd->close();
			ACHAR *blockName;
			pBlockRcd->getName(blockName);
			mGrid.SetItemText(row, OldBlockName, blockName);
		}
		else
		{
			mGrid.SetItemText(row, OldBlockName, _T("块名未知"));
		}

		//获取当前所在图层
		mGrid.SetItemText(row, OldLayerName, infosIter->pMText->layer());
		
		//获取当前文字样式
		AcDbTextStyleTableRecord *pTextStyleRcd;
		es = acdbOpenObject(pTextStyleRcd, infosIter->pMText->textStyle(), AcDb::kForRead);
		if (es == Acad::eOk)
		{
			pTextStyleRcd->close();
			ACHAR *textStyleName;
			pTextStyleRcd->getName(textStyleName);
			mGrid.SetItemText(row, OldTextStyleName, textStyleName);
		}
		else
		{
			mGrid.SetItemText(row, OldTextStyleName, _T("未知文字样式"));
		}

		mGrid.SetItemText(row, NewContents, infosIter->newContents);
		mGrid.SetItemFormat(row, NewContents, DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
		
		mGrid.SetItemText(row, NewXdata, infosIter->newXdata);
		mGrid.SetItemFormat(row, NewXdata, DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);

		mGrid.SetCellType(row, NewLayerName, RUNTIME_CLASS(CGridCellCombo));
		((CGridCellCombo *)mGrid.GetCell(row, NewLayerName))->SetOptions(m_allLayersArray);
		if (infosIter->newLayerName.IsEmpty())
			((CGridCellCombo *)mGrid.GetCell(row, NewLayerName))->SetText(mGrid.GetItemText(row, OldLayerName));
		else
			((CGridCellCombo *)mGrid.GetCell(row, NewLayerName))->SetText(infosIter->newLayerName);

		mGrid.SetCellType(row, NewTextStyleName, RUNTIME_CLASS(CGridCellCombo));
		((CGridCellCombo *)mGrid.GetCell(row, NewTextStyleName))->SetOptions(m_allTextStylesArray);
		if (infosIter->newTextStyleName.IsEmpty())
			((CGridCellCombo *)mGrid.GetCell(row, NewTextStyleName))->SetText(mGrid.GetItemText(row, OldTextStyleName));
		else
			((CGridCellCombo *)mGrid.GetCell(row, NewTextStyleName))->SetText(infosIter->newTextStyleName);
	}
	mGrid.ExpandColumnsToFit();
}

void MultiEditMTextProcess::OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (m_gridActiveCombo != NULL)
	{
		::SetFocus(this->m_hWnd);
		SetCapture();
		ReleaseCapture();
		m_gridActiveCombo = NULL;
		mGrid.Refresh();
		return;
	}

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	int row = pItem->iRow;
	int column = pItem->iColumn; //得到当前行、列
	if (row > 0 && column >= NewLayerName)  //不是固定行且是打印尺寸列和打印样式表列
	{
		m_gridActiveCombo = (CGridCellCombo*)mGrid.GetCell(row, column);
		CCellID cellId(row, column);
		m_gridActiveCombo->Click(cellId);
	}
}

void MultiEditMTextProcess::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	m_gridActiveCombo = NULL;
	mGrid.Refresh();
}

void MultiEditMTextProcess::OnOK()
{
	CDialog::OnOK();
	int index = 1;
	list<MTextNewInfo>::iterator infosIter = m_allMTextsInfos.begin();
	for (; infosIter != m_allMTextsInfos.end(); infosIter++)
	{
		AcDbMText *pMtext ;
		Acad::ErrorStatus es = acdbOpenObject(pMtext, infosIter->pMText->id(), AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			index++;
			continue;
		}
		pMtext->setContents(mGrid.GetItemText(index, NewContents));
		GAttrMgr::SetAttrKey(pMtext, mGrid.GetItemText(index, NewXdata));
		pMtext->setLayer(mGrid.GetItemText(index, NewLayerName));
		pMtext->setTextStyle(m_allTextStyles[mGrid.GetItemText(index, NewTextStyleName)]);
		pMtext->close();
		index++;
	}

	acedCommand(RTSTR, _T("REGEN"), RTNONE);
}


bool MultiEditMTextProcess::LoadData()
{
	return true;
}

bool MultiEditMTextProcess::LoadData(const map<CString, AcDbObjectId>& allBlocks, const map<CString, AcDbObjectId>& allLayers,
	const map<CString, AcDbObjectId>& allTextStyles, const list<MTextNewInfo>& allMTextInfos)
{
	m_allBlocks = allBlocks;
	m_allLayers = allLayers;
	m_allTextStyles = allTextStyles;
	m_allMTextsInfos = allMTextInfos;

	m_allBlocksArray.RemoveAll();
	m_allLayersArray.RemoveAll();
	m_allTextStylesArray.RemoveAll();

	map<CString, AcDbObjectId>::const_iterator iter = allBlocks.begin();
	for (; iter != allBlocks.end(); ++iter)
	{
		m_allBlocksArray.Add(iter->first);
	}
	iter = allLayers.begin();
	for (; iter != allLayers.end(); ++iter)
	{
		m_allLayersArray.Add(iter->first);
	}
	iter = allTextStyles.begin();
	for (; iter != allTextStyles.end(); ++iter)
	{
		m_allTextStylesArray.Add(iter->first);
	}

	return LoadData();
}



void MultiSearchAndEditMText::OnBnClickedButtonUsermanual()
{
	GPath::OpenUserManual(_T("MultiSearchAndEditMTextPageId"));
}
