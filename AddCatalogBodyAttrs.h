#pragma once
class AddCatalogBodyAttrs
{
public:
	AddCatalogBodyAttrs();
	~AddCatalogBodyAttrs();

private:
	static AddCatalogBodyAttrs* mInstance;
	CString m_layerName;			//创建的多行文本所在的图层
	CString m_textStyleName;		//创建的多行文本所在的文字样式
	double m_leftOffset;			//多行文本范围左侧往右压缩的距离
	double m_rightOffset;			//多行文本范围右侧往左压缩的距离
	bool m_isContentUseNumberSuffix;		//多行文本是否自动添加递增数字后缀
	
	void readConfig();
	AcDbObjectId getSelectedLayerId();
	AcDbObjectId getSelectedTextStyleId();

public:
	static AddCatalogBodyAttrs* GetInstance();
	bool start();
};

