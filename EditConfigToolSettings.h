#pragma once
#include "BaseGridEditor.h"
#include <vector>

using std::vector;

class EditConfigToolSettings :
	public BaseGridEditor
{
public:
	EditConfigToolSettings(CWnd* pParent = NULL);
	~EditConfigToolSettings();

private:
	typedef struct RowInfo
	{
		CString iniGroupName;
		CString iniKey;
		CString curIniValue;
		CString displayName;
	}RowInfo;

	vector<RowInfo> m_allRowInfos;
	bool InitGrid();
	void UpdateGrid();
	bool LoadData();
	bool AddData(CString iniGroupName, CString iniKey, CString displayName);
	void OnOK();
	void HandleUserManual();
};

