#pragma once
#include "arxHeaders.h"

class AddRoleSignBox
{
public:
	AddRoleSignBox();
	~AddRoleSignBox();

private:
	static AddRoleSignBox* mInstance;
	bool m_isUseSameHyperLinkAndName;			//第一第二字段是否相同，相同省去用户填写第二字段步骤
	bool m_isAutoContinue;		//程序是否自动继续进行
	CString m_layerName;			//创建的签名框所在图层

	void readConfig();
	AcDbObjectId getSelectedLayerId();
	
public:
	static AddRoleSignBox* GetInstance();
	void start();

};

