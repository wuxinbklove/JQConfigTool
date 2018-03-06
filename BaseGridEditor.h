#pragma once
#include <vector>
#include "Resource.h"
#include "dbents.h"
#include "sdk/GridCtrl/GridCtrl.h"
using namespace std;

// BaseGridEditor ¶Ô»°¿ò

class BaseGridEditor : public CDialog
{
	DECLARE_DYNAMIC(BaseGridEditor)

public:
	BaseGridEditor(CWnd* pParent = NULL); 
	virtual ~BaseGridEditor();
	enum { IDD = IDD_DIALOG_BASEGRIDEDITOR};

protected:
	CGridCtrl mGrid;
	DECLARE_MESSAGE_MAP()

private:
	void DoDataExchange(CDataExchange* pDX);
	BOOL OnInitDialog();
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void GetCurCtrlRect();
	void OnBnClickedButtonUsermanual();

	CSize m_minSize;
	bool m_isInitDialogFinshed;
	RECT m_selfOldRect;
	RECT m_gridRect;
	RECT m_okRect;
	RECT m_cancelRect;
	RECT m_userManualRect;

protected:
	void SetWindowMinSize(CSize minSize);
	virtual bool InitGrid() = 0;
	virtual void UpdateGrid() = 0;
	virtual bool LoadData() = 0;
	virtual void HandleUserManual();
	virtual void OnOK();

};
