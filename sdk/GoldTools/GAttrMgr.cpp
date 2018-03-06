#include "stdafx.h"
#include "GAttrMgr.h"
using namespace std;

namespace GAttrMgr
{
	CString GetAttrKey(AcDbEntity* pEntity)
	{
		CString key("");
		struct resbuf* pXdata = pEntity->xData(_T("ZYCXDATA"));
		if (pXdata != NULL && pXdata->rbnext != NULL)
		{
			key = pXdata->rbnext->resval.rstring;
		}
		if (pXdata != NULL)
		{
			acutRelRb(pXdata);
		}
		return key;
	}

	Acad::ErrorStatus SetAttrKey(AcDbObjectId objId, const CString strKey)
	{
		AcDbEntity* pEnt = NULL;
		Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, objId, AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			return es;
		}
		es = SetAttrKey(pEnt, strKey);
		pEnt->close();
		return es;
	}

	Acad::ErrorStatus SetAttrKey(AcDbEntity* pEnt, const CString strKey)
	{
		if (!pEnt->isWriteEnabled())
			return Acad::eNotOpenForWrite;

		struct resbuf* pXdata = pEnt->xData(_T("ZYCXDATA"));
		if (pXdata == NULL)
		{
			acdbRegApp(_T("ZYCXDATA"));
			pXdata = acutNewRb(AcDb::kDxfRegAppName);
			pXdata->resval.rstring = (ACHAR*)malloc(sizeof(ACHAR)*lstrlen(_T("ZYCXDATA")) + 1);
			lstrcpy(pXdata->resval.rstring, _T("ZYCXDATA"));
		}
		else
		{
			acutRelRb(pXdata->rbnext);
		}
		struct resbuf* pKey = NULL;
		pKey = acutNewRb(AcDb::kDxfXdAsciiString);
		pXdata->rbnext = pKey;
		pKey->resval.rstring = (ACHAR*)malloc(sizeof(ACHAR)*strKey.GetLength() + 1);
		lstrcpy(pKey->resval.rstring, strKey);

		Acad::ErrorStatus es = pEnt->setXData(pXdata);
		acutRelRb(pXdata);

		return es;
	}

	void RemoveFormatChar(CString& contents, CString formatChar)
	{
		int start = contents.Find(formatChar);
		if (start >= 0)
		{
			int end = contents.Find(_T(";"), start);
			if (end > 0)
			{
				CString left;
				if (start > 0)
				{
					left = contents.Left(start);
				}
				contents = left + contents.Right(contents.GetLength() - end - 1);
			}
		}
	}

	CString RemoveFormatChars(CString& contents)
	{
		while (contents.Find(_T("{")) == 0)
		{
			int len = contents.GetLength();
			if (contents.Find(_T("}")) == len - 1)
			{
				contents = contents.Mid(1, len - 2);
			}
			else
			{
				contents = contents.Mid(1, len - 1);
			}

			RemoveFormatChar(contents, AcDbMText::alignChange());
			RemoveFormatChar(contents, AcDbMText::colorChange());
			RemoveFormatChar(contents, AcDbMText::fontChange());
			RemoveFormatChar(contents, AcDbMText::heightChange());
			RemoveFormatChar(contents, AcDbMText::lineBreak());
			RemoveFormatChar(contents, AcDbMText::nonBreakSpace());
			RemoveFormatChar(contents, AcDbMText::obliqueChange());
			RemoveFormatChar(contents, AcDbMText::overlineOff());
			RemoveFormatChar(contents, AcDbMText::overlineOn());
			RemoveFormatChar(contents, AcDbMText::paragraphBreak());
			RemoveFormatChar(contents, AcDbMText::stackStart());
			RemoveFormatChar(contents, AcDbMText::trackChange());
			RemoveFormatChar(contents, AcDbMText::underlineOff());
			RemoveFormatChar(contents, AcDbMText::underlineOn());
			RemoveFormatChar(contents, AcDbMText::widthChange());
		}
		return contents;
	}

	/*!
	@brief 得到文本内容，适用于cad07以下版本
	*/
	CString GetMText(AcDbMText* pText)
	{
#if _MSC_VER >= 1500
		return pText->text();
#else
		ACHAR* pContent = pText->contents();
		CString contents(pContent);
		free(pContent);

#ifndef _UNICODE
		//转换可能存在的unicode字符串
		int index = -1;
		while ((index = contents.Find("\\U+")) >= 0)
		{
			CString code = contents.Mid(index + 3, 4);
			WCHAR wCode = (short)strtol(code, NULL, 16);
			char result[3] = { 0 };
			WideCharToMultiByte(CP_ACP, 0, &wCode, 1, result, 2, NULL, FALSE);
			code = "\\U+" + code;
			contents.Replace(code, result);
		}
#endif

		return RemoveFormatChars(contents);
#endif
	}

	/*!
	@brief 设置MText文本值
	*/
	void SetMText(AcDbMText* pText, CString& text)
	{
		if (pText == NULL)
		{
			return;
		}
		ACHAR* pContent = pText->contents();
		CString contents(pContent);
		free(pContent);

		//处理换行
		text.Replace(_T("\r\r\n"), _T("\\P"));
		text.Replace(_T("\r\n"), _T("\\P"));
		text.Replace(_T("\n"), _T("\\P"));
		text.Replace(_T("\r"), _T("\\P"));

		int len = contents.GetLength();
		if (contents.IsEmpty() || contents[len - 1] != '}')
		{
			pText->setContents(text);
			return;
		}
		int index = 0;
		for (int i = len - 2; i >= 0; i--)
		{
			if (contents[i] == ';')
			{
				index = i;
			}
			else if (contents[i] == '\\')
			{
				break;
			}
		}
		if (index == 0)
		{
			pText->setContents(text);
		}
		else
		{
			CString newContents;
			newContents.Format(_T("%s%s}"), contents.Left(index + 1), text);
			pText->setContents(newContents);
		}
	}

	double GetTextStyleWidthFactor(AcDbObjectId& id)
	{
		AcDbTextStyleTableRecord* pTextStyle = NULL;
		Acad::ErrorStatus es;
		es = acdbOpenAcDbObject((AcDbObject*&)pTextStyle, id, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			pTextStyle->close();
			return pTextStyle->xScale();
		}
		return 1.0;
	}

	CString GetRole(AcDbEntity* pEnt, bool isReturnUrl)
	{
		CString role(_T(""));
		if (pEnt == NULL)
		{
			return role;
		}
		struct resbuf* rb = pEnt->xData(_T("PE_URL"));
		if (rb == NULL)
		{
			return role;
		}
		int i = 0;
		const struct resbuf* pRbIter = rb;
		while (pRbIter != NULL)
		{
			if (pRbIter->restype == 1000)
			{
				i++;
				if (isReturnUrl)
				{
					if (i == 1)
					{
						role = pRbIter->resval.rstring;
						break;
					}
				}
				else
				{
					if (i == 2)
					{
						role = pRbIter->resval.rstring;
						break;
					}
				}

			}
			pRbIter = pRbIter->rbnext;
		}
		acutRelRb(rb);
		return role;
	}
}