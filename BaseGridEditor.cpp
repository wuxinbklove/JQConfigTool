// BaseGridEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "BaseGridEditor.h"
#include "sdk/GoldTools/GAttrMgr.h"


// BaseGridEditor 对话框

IMPLEMENT_DYNAMIC(BaseGridEditor, CDialog)
BaseGridEditor::BaseGridEditor(CWnd* pParent /*=NULL*/)
	: CDialog(BaseGridEditor::IDD, pParent)
{
	m_isInitDialogFinshed = false;
	m_minSize.SetSize(300, 200);
}

BaseGridEditor::~BaseGridEditor()
{
}

void BaseGridEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GRID_GRIDEDITOR, mGrid);
}


BEGIN_MESSAGE_MAP(BaseGridEditor, CDialog)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON_USERMANUAL, &BaseGridEditor::OnBnClickedButtonUsermanual)
END_MESSAGE_MAP()


BOOL BaseGridEditor::OnInitDialog()
{
    int ret = CDialog::OnInitDialog();
	if (ret)
	{
		m_isInitDialogFinshed = true;
		SetIcon(acedGetAcadFrame()->GetIcon(FALSE), FALSE);
		SetIcon(acedGetAcadFrame()->GetIcon(TRUE), TRUE);
	}

	return (ret && InitGrid());
}

void BaseGridEditor::OnOK()
{
	CDialog::OnOK();
}

afx_msg void BaseGridEditor::OnSizing(UINT nSide, LPRECT lpRect)
{
	if (m_isInitDialogFinshed)
		GetCurCtrlRect();
	CDialog::OnSizing(nSide, lpRect);
}

afx_msg void BaseGridEditor::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_isInitDialogFinshed)
	{
		m_gridRect.right += cx - m_selfOldRect.right;
		m_gridRect.bottom += cy - m_selfOldRect.bottom;
		mGrid.MoveWindow(&m_gridRect);
		//m_userManualRect.left += cx - m_selfOldRect.right;
		//m_userManualRect.right += cx - m_selfOldRect.right;
		m_userManualRect.top += cy - m_selfOldRect.bottom;
		m_userManualRect.bottom += cy - m_selfOldRect.bottom;
		GetDlgItem(IDC_BUTTON_USERMANUAL)->MoveWindow(&m_userManualRect);
		m_okRect.left += cx - m_selfOldRect.right;
		m_okRect.right += cx - m_selfOldRect.right;
		m_okRect.top += cy - m_selfOldRect.bottom;
		m_okRect.bottom += cy - m_selfOldRect.bottom;
		GetDlgItem(IDOK)->MoveWindow(&m_okRect);
		GetDlgItem(IDOK)->Invalidate();
		m_cancelRect.left += cx - m_selfOldRect.right;
		m_cancelRect.right += cx - m_selfOldRect.right;
		m_cancelRect.top += cy - m_selfOldRect.bottom;
		m_cancelRect.bottom += cy - m_selfOldRect.bottom;
		GetDlgItem(IDCANCEL)->MoveWindow(&m_cancelRect);
		GetDlgItem(IDCANCEL)->Invalidate();

		mGrid.ExpandColumnsToFit();
	}
}

afx_msg void BaseGridEditor::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_minSize.cx;
	lpMMI->ptMinTrackSize.y = m_minSize.cy;
	if (m_isInitDialogFinshed)
		GetCurCtrlRect();
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void BaseGridEditor::GetCurCtrlRect()
{
	mGrid.GetWindowRect(&m_gridRect);
	ScreenToClient(&m_gridRect);
	GetDlgItem(IDC_BUTTON_USERMANUAL)->GetWindowRect(&m_userManualRect);
	ScreenToClient(&m_userManualRect);
	GetDlgItem(IDOK)->GetWindowRect(&m_okRect);
	ScreenToClient(&m_okRect);
	GetDlgItem(IDCANCEL)->GetWindowRect(&m_cancelRect);
	ScreenToClient(&m_cancelRect);
	GetClientRect(&m_selfOldRect);
}

void BaseGridEditor::SetWindowMinSize(CSize minSize)
{
	m_minSize = minSize;
}


void BaseGridEditor::OnBnClickedButtonUsermanual()
{
	HandleUserManual();
}

void BaseGridEditor::HandleUserManual()
{
	MessageBox(_T("无该主题帮助文档！"), 0, MB_ICONWARNING | MB_OK);
}