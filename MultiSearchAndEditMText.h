#pragma once
#include "resource.h"
#include "arxHeaders.h"
#include <map>
#include <list>
#include "BaseGridEditor.h"
#include "sdk/GridCtrl/GridCellCombo.h"

using std::map;
using std::list;

class MultiEditMTextProcess :public BaseGridEditor
{
public:
	typedef struct MTextNewInfo
	{
		AcDbMText *pMText;
		CString newBlockName;
		CString newTextStyleName;
		CString newLayerName;
		CString newXdata;
		CString newContents;

		MTextNewInfo()
		{
			pMText = NULL;
		}

	} MTextNewInfo;

	MultiEditMTextProcess(CWnd* pParent = NULL);
	~MultiEditMTextProcess();

private:
	enum GridColDefine {
		OldContents = 0 , 
		OldXdata , 
		OldBlockName , 
		OldLayerName , 
		OldTextStyleName , 
		NewContents , 
		NewXdata,
		NewLayerName , 
		NewTextStyleName
	};

	map<CString, AcDbObjectId> m_allLayers;
	map<CString, AcDbObjectId> m_allTextStyles;
	map<CString, AcDbObjectId> m_allBlocks;
	CStringArray m_allLayersArray;
	CStringArray m_allTextStylesArray;
	CStringArray m_allBlocksArray;
	list<MTextNewInfo> m_allMTextsInfos;
	CGridCellCombo *m_gridActiveCombo;

	DECLARE_MESSAGE_MAP()

	bool InitGrid();
	void UpdateGrid();
	bool LoadData();
	void OnOK();
	void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);

public:
	bool LoadData(const map<CString, AcDbObjectId>& allBlocks ,const map<CString, AcDbObjectId>& allLayers , 
		const map<CString, AcDbObjectId>& allTextStyles , const list<MTextNewInfo>& allMTextInfos);
};



// MultiSearchAndEditMText 对话框

class MultiSearchAndEditMText : public CDialog
{
	DECLARE_DYNAMIC(MultiSearchAndEditMText)

public:
	MultiSearchAndEditMText(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MultiSearchAndEditMText();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MULTIMODIFYMTEXT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

public:
	bool LoadLayerData();
	bool LoadTextStyleData();
	bool LoadBlockData();

private:
	map<CString, AcDbObjectId> m_allLayers;
	map<CString, AcDbObjectId> m_allTextStyles;
	map<CString, AcDbObjectId> m_allBlocks;
	list<AcDbMText*> m_allSearchedMTexts;

	void InitCombos();
	void SearchMText();
	list<MultiEditMTextProcess::MTextNewInfo> CalcNewInfos();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonUsermanual();
};

