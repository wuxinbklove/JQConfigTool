#include "StdAfx.h"
#include <set>
#include "MultiRenameSymbolRecord.h"
#include "sdk/GoldTools/GLockDocument.h"
#include "sdk/GoldTools/GPath.h"

MultiRenameSymbolRecord::MultiRenameSymbolRecord(CWnd* pParent) 
	:BaseGridEditor(pParent)
{
}


MultiRenameSymbolRecord::~MultiRenameSymbolRecord()
{
}

bool MultiRenameSymbolRecord::LoadData()
{
	m_forbiddenNames.clear();
	GLockDocument lockDoc;
	AcDbSymbolTable *pSymbolTable;
	Acad::ErrorStatus es;
	switch (m_tableType)
	{
	case MultiRenameSymbolRecord::BlockTable:
		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pSymbolTable, AcDb::kForRead);
		break;
	case MultiRenameSymbolRecord::LayerTable:
		es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pSymbolTable, AcDb::kForRead);
		break;
	case MultiRenameSymbolRecord::TextStyleTable:
		es = acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pSymbolTable, AcDb::kForRead);
		break;
	default:
		return false;
	}
	if (es != Acad::eOk)
		return false;
	pSymbolTable->close();

	AcDbSymbolTableIterator *pSymbolTableIter;
	pSymbolTable->newIterator(pSymbolTableIter);
	for (pSymbolTableIter->start(); !pSymbolTableIter->done(); pSymbolTableIter->step())
	{
		AcDbSymbolTableRecord *pRcd;
		es = pSymbolTableIter->getRecord(pRcd , AcDb::kForRead);
		if (es != Acad::eOk)
			return false;
		
		if (m_tableType == MultiRenameSymbolRecord::BlockTable)
		{
			if (((AcDbBlockTableRecord *)pRcd)->isLayout())		//用于布局的块名不处理
			{
				pRcd->close();
				continue;
			}
		}
		if (m_tableType == MultiRenameSymbolRecord::LayerTable)		//图层可能来自外部参照，先测试是否能进行重名，不能则不处理
		{
			es = pRcd->upgradeOpen();
			if (es != Acad::eOk)
			{
				pRcd->close();
				return false;
			}
			ACHAR *curName;
			pRcd->getName(curName);
			es = pRcd->setName(_T("___tryRenameLayer___"));
			pRcd->setName(curName);
			pRcd->downgradeOpen();
			if (es != Acad::eOk)
			{
				m_forbiddenNames.insert(curName);
				pRcd->close();
				continue;
			}

		}
		m_recordIds.append(pRcd->id());
		pRcd->close();
	}
	delete pSymbolTableIter;
	return true;
}

bool MultiRenameSymbolRecord::LoadData(SymboTableType tableType)
{
	m_tableType = tableType;
	return LoadData();
}


bool MultiRenameSymbolRecord::InitGrid()
{
	SetWindowText(_T("批量重命名(仅显示可编辑项)"));

	mGrid.InsertColumn(_T("旧名称"));
	mGrid.InsertColumn(_T("新名称"));
	mGrid.SetColumnWidthPercent(0, 50);
	mGrid.SetColumnWidthPercent(1, 50);
	mGrid.ExpandColumnsToFit();
	mGrid.ExpandLastColumn();

	mGrid.SetFixedRowCount(1);
	mGrid.SetFixedColumnCount(1);
	mGrid.SetGridBkColor(RGB(255, 255, 255));
	mGrid.EnableSelection(FALSE);
	mGrid.SetTrackFocusCell(FALSE);
	mGrid.SetFrameFocusCell(FALSE);
	mGrid.SetColumnResize(false);
	UpdateGrid();
	return TRUE;
}
void MultiRenameSymbolRecord::UpdateGrid()
{
	mGrid.DeleteNonFixedRows();
	for (int i = 0; i < m_recordIds.logicalLength(); i++)
	{
		AcDbObjectId recordId = m_recordIds[i];;
		AcDbSymbolTableRecord *pSymRcd;
		Acad::ErrorStatus es = acdbOpenObject(pSymRcd, recordId , AcDb::kForRead);
		if (es != Acad::eOk)
		{
			mGrid.InsertRow(_T("unreadable symbol record"));
			continue;
		}
		pSymRcd->close();
		ACHAR *oldName;
		pSymRcd->getName(oldName);
		int row = mGrid.InsertRow(oldName);
		mGrid.SetItemText(row, 1, oldName);
	}
	mGrid.ExpandColumnsToFit();
}

bool MultiRenameSymbolRecord::CheckNewName()
{
	set<CString> strSet;
	for (int i = 1; i < mGrid.GetRowCount(); i++)
	{
		CString str1 = mGrid.GetItemText(i, 0);
		CString str2 = mGrid.GetItemText(i, 1);
		if (str2.IsEmpty())
			return false;
		if (m_forbiddenNames.find(str2) != m_forbiddenNames.end())
			return false;
		if (strSet.find(str2) != strSet.end())
			return false;
		strSet.insert(str1);
		if (str1.Compare(str2) != 0)
			strSet.insert(str2);
	}
	return true;
}

void MultiRenameSymbolRecord::OnOK()
{
	GLockDocument lockDoc;
	
	if (!CheckNewName())
	{
		MessageBox(_T("检测到填写的新名称含有重复值或者非法值，请检查！"));
		return;
	}

	int len = m_recordIds.logicalLength();
	for (int i = 0; i < len; i++)
	{
		AcDbObjectId recordId = m_recordIds[i];
		AcDbSymbolTableRecord *pSymRcd;
		Acad::ErrorStatus es = acdbOpenObject(pSymRcd, recordId, AcDb::kForWrite);
		if (es != Acad::eOk)
			continue;
		CString newName = mGrid.GetItemText(i + 1, 1);
		es = pSymRcd->setName(newName);
		pSymRcd->close();
	}
	CDialog::OnOK();
}

void MultiRenameSymbolRecord::HandleUserManual()
{
	GPath::OpenUserManual(_T("RenamePageId"));
}