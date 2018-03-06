#include "StdAfx.h"
#include "EditConfigToolSettings.h"
#include "sdk/GoldTools/GPath.h"

EditConfigToolSettings::EditConfigToolSettings(CWnd* pParent)
	:BaseGridEditor(pParent)
{
	LoadData();
}


EditConfigToolSettings::~EditConfigToolSettings()
{
}

bool EditConfigToolSettings::InitGrid()
{
	SetWindowText(_T("选项"));

	SetWindowPos(0, 0, 0, 620, 450, SWP_NOMOVE | SWP_NOZORDER);

	mGrid.InsertColumn(_T("项目"));
	mGrid.InsertColumn(_T("值"));
	mGrid.SetColumnWidthPercent(0, 50);
	mGrid.SetColumnWidthPercent(1, 50);
	mGrid.ExpandLastColumn();

	mGrid.SetRowHeight(0, 25);
	mGrid.SetGridBkColor(RGB(255, 255, 255));
	mGrid.EnableSelection(FALSE);
	mGrid.SetTrackFocusCell(FALSE);
	mGrid.SetFrameFocusCell(FALSE);
	mGrid.SetColumnResize(false);
	UpdateGrid();
	return TRUE;
}

void EditConfigToolSettings::UpdateGrid()
{
	mGrid.DeleteNonFixedRows();
	for (int i = 0; i < m_allRowInfos.size(); i++)
	{
		int row = mGrid.InsertRow(m_allRowInfos[i].displayName);
		mGrid.GetCell(row, 0)->SetState(GVIS_READONLY);
		if (m_allRowInfos[i].iniKey.IsEmpty())
		{
			mGrid.GetCell(row, 0)->SetBackClr(RGB(200, 200, 200));
			mGrid.GetCell(row, 1)->SetBackClr(RGB(200, 200, 200));
			mGrid.GetCell(row, 1)->SetState(GVIS_READONLY);
		}
		else
		{
			mGrid.SetItemText(row, 1, m_allRowInfos[i].curIniValue);
		}
	}
	mGrid.ExpandColumnsToFit();
}

bool EditConfigToolSettings::LoadData()
{
	m_allRowInfos.clear();
	AddData(_T(""), _T(""), _T("目录表格属性"));
	AddData(_T("AddCatalogBodyAttrs"), _T("LeftOffset"), _T("左侧往右压缩距离 (缺省值为0.0)"));
	AddData(_T("AddCatalogBodyAttrs"), _T("RightOffset"), _T("右侧往左压缩距离 (缺省值为0.0)"));
	AddData(_T("AddCatalogBodyAttrs"), _T("IsContentUseNumberSuffix"), _T("文本是否自动添加递增数字后缀 (缺省值为1)"));
	AddData(_T("AddCatalogBodyAttrs"), _T("DefaultLayer"), _T("默认图层 (缺省时,使用CAD当前)"));
	AddData(_T("AddCatalogBodyAttrs"), _T("DefaultTextStyle"), _T("默认文字样式 (缺省时,使用CAD当前)"));

	AddData(_T(""), _T(""), _T("创建签名框"));
	AddData(_T("AddRoleSignBox"), _T("IsUseSameHyperLinkAndName"), _T("第一第二字段始终保持相同 (缺省值为1)"));
	AddData(_T("AddRoleSignBox"), _T("IsAutoContinue"), _T("自动进行下一个创建 (缺省值为0)"));
	AddData(_T("AddRoleSignBox"), _T("DefaultLayer"), _T("默认图层 (缺省时,使用CAD当前)"));

	AddData(_T(""), _T(""), _T("批量单行转多行"));
	AddData(_T("TransTextToMText"), _T("XOffset"), _T("x方向偏移 (缺省值为0.2)"));
	AddData(_T("TransTextToMText"), _T("YOffset"), _T("y方向偏移 (缺省值为0.0)"));
	AddData(_T("TransTextToMText"), _T("IsSetSingleLine"), _T("文本始终保持单行 (缺省值为1)"));

	AddData(_T(""), _T(""), _T("提取所依赖的字体文件"));
	AddData(_T("ArchiveAllFonts"), _T("DefaultSavePath"), _T("存储路径 (缺省时,使用程序当前)"));
	return true;
}

bool EditConfigToolSettings::AddData(CString iniGroupName, CString iniKey, CString displayName)
{
	RowInfo rowInfo;
	rowInfo.iniGroupName = iniGroupName;
	rowInfo.iniKey = iniKey;
	rowInfo.displayName = displayName;
	CString curIniValue;
	::GetPrivateProfileString(iniGroupName, iniKey, _T(""), curIniValue.GetBuffer(MAX_PATH), MAX_PATH, ConfigFile);
	curIniValue.ReleaseBuffer();
	rowInfo.curIniValue = curIniValue;
	m_allRowInfos.push_back(rowInfo);
	return true;
}

void EditConfigToolSettings::OnOK()
{
	CDialog::OnOK();
	for (int i = 0; i < m_allRowInfos.size(); i++)
	{
		CString newValue = mGrid.GetItemText(i, 1);
		if (!m_allRowInfos[i].iniKey.IsEmpty())
			::WritePrivateProfileString(m_allRowInfos[i].iniGroupName, m_allRowInfos[i].iniKey, newValue, ConfigFile);
	}
}

void EditConfigToolSettings::HandleUserManual()
{
	GPath::OpenUserManual(_T("EditConfigToolSettingsPageId"));
}