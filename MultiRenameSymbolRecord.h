#pragma once
#include "BaseGridEditor.h"
#include <set>

class MultiRenameSymbolRecord :
	public BaseGridEditor
{
public:
	enum SymboTableType {BlockTable , LayerTable , TextStyleTable};

	MultiRenameSymbolRecord(CWnd* pParent = NULL);
	~MultiRenameSymbolRecord();

private:
	SymboTableType m_tableType;
	AcDbObjectIdArray m_recordIds;
	set<CString> m_forbiddenNames;

	bool LoadData();
	bool CheckNewName();
	void HandleUserManual();

protected:
	bool InitGrid();
	void UpdateGrid();
	void OnOK();

public:
	bool LoadData(SymboTableType tableType);
};

