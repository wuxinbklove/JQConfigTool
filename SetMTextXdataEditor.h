#pragma once
#include "BaseGridEditor.h"


class SetMTextXdataEditor : public BaseGridEditor
{
public:
	SetMTextXdataEditor(CWnd* pParent = NULL);
	~SetMTextXdataEditor();

protected:
	bool InitGrid();
	void UpdateGrid();
	void OnOK();
	void HandleUserManual();

public:
	bool LoadData();

private:
	bool m_isLoadDataSuccess;
	AcDbObjectIdArray m_EntsIds;
};

