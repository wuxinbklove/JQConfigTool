#pragma once
#include <string>
#include <map>
#include "arxHeaders.h"

#ifndef  GOLD_EXPORT
#define  GOLD_EXPORT
#endif 

#ifdef UNICODE
#define gstring std::wstring
#else
#define gstring std::string
#endif

namespace GAttrMgr
{
	//获取实体key
	CString GOLD_EXPORT GetAttrKey(AcDbEntity* pText);

	//设置实体key
	Acad::ErrorStatus SetAttrKey(AcDbObjectId objId, const CString strKey);
	Acad::ErrorStatus SetAttrKey(AcDbEntity* pEnt, const CString strKey);
	
	//去掉读取到的多行文本中的控制字符）
	CString GOLD_EXPORT RemoveFormatChars(CString& contents);

	//获取文本内容，不包含控制字符
	CString GOLD_EXPORT GetMText(AcDbMText* pText);

	//设置MText文本值
	void GOLD_EXPORT SetMText(AcDbMText* pText, CString& text);

	//获取文字样式宽度比例
	double GOLD_EXPORT GetTextStyleWidthFactor(AcDbObjectId& id);

	//获取超链接属性，当isReturnUrl为false时返回的的是超链接名，为true时返回的是超链接的url，默认值为false。 
	CString GetRole(AcDbEntity* pEnt, bool isReturnUrl = false);
};
