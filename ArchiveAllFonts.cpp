#include "StdAfx.h"
#include <set>
#include "ArchiveAllFonts.h"
#include "sdk/GoldTools/GCharSet.h"
#include "sdk/GoldTools/GPath.h"
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

ArchiveAllFonts* ArchiveAllFonts::mInstance = NULL;

ArchiveAllFonts* ArchiveAllFonts::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new ArchiveAllFonts();
	}
	mInstance->readConfig();
	return mInstance;
}

ArchiveAllFonts::ArchiveAllFonts()
{
	
}


ArchiveAllFonts::~ArchiveAllFonts()
{
}

void ArchiveAllFonts::readConfig()
{
	::GetPrivateProfileString(_T("ArchiveAllFonts"), _T("DefaultSavePath"), _T(""), m_destPath.GetBuffer(100), 100, ConfigFile);
	m_destPath.ReleaseBuffer();

	if (m_destPath.IsEmpty())
		m_destPath = DataPath + _T("Fonts\\");
	
	if (!PathIsDirectory(m_destPath) && !CreateDirectory(m_destPath, NULL))
	{
		m_destPath = DataPath + _T("Fonts\\");
		if (!PathIsDirectory(m_destPath))
			CreateDirectory(m_destPath, NULL);
	}

	STARTUPINFO si; //启动信息
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi; //进程信息

	CreateProcess(NULL, ( CString(_T("cmd.exe /c del /Q /F \"")) + m_destPath + _T("\"")).GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void ArchiveAllFonts::start()
{
	AcDbTextStyleTable *pTSTbl;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTSTbl, AcDb::kForRead);
	if (es != Acad::eOk)
		return;
	pTSTbl->close();

	std::set<CString> lostFontSet;

	AcDbTextStyleTableIterator *pTSTblIter;
	pTSTbl->newIterator(pTSTblIter);
	for (pTSTblIter->start(); !pTSTblIter->done(); pTSTblIter->step())
	{
		AcDbTextStyleTableRecord *pTSTblRcd;
		es = pTSTblIter->getRecord(pTSTblRcd, AcDb::kForRead);
		if (es != Acad::eOk)
			continue;
		pTSTblRcd->close();
		ACHAR *fontFileName;

		pTSTblRcd->fileName(fontFileName);
		CString fontFileNameStr(fontFileName);

		if (fontFileNameStr.IsEmpty())
			continue;

		//处理CAD上部分读取出来的字体没扩展名的bug
		if (fontFileNameStr.GetLength() <= 4 || fontFileNameStr.ReverseFind(_T('.')) != fontFileNameStr.GetLength() - 4)
			fontFileNameStr += _T(".shx");

		CString fontsPath = GPath::GetSystemFontsPath();
		if (PathFileExists(GPath::GetAcadAppPath() + _T("Fonts\\") + fontFileNameStr))
			CopyFile(GPath::GetAcadAppPath() + _T("Fonts\\") + fontFileNameStr, m_destPath + fontFileNameStr, FALSE);
		else if (PathFileExists(GPath::GetSystemFontsPath() + fontFileNameStr))
			CopyFile(GPath::GetSystemFontsPath() + fontFileNameStr, m_destPath + fontFileNameStr, FALSE);
		else
			lostFontSet.insert(fontFileNameStr);

		ACHAR *bigFontFileName;
		pTSTblRcd->bigFontFileName(bigFontFileName);
		CString bigFontFileNameStr(bigFontFileName);
		if (bigFontFileNameStr.IsEmpty())
			continue;

		//处理CAD上部分读取出来的字体没扩展名的bug
		if (bigFontFileNameStr.GetLength() <= 4 || bigFontFileNameStr.ReverseFind(_T('.')) != bigFontFileNameStr.GetLength() - 4)
			bigFontFileNameStr += _T(".shx");

		if (PathFileExists(GPath::GetAcadAppPath() + _T("Fonts\\") + bigFontFileNameStr))
			CopyFile(GPath::GetAcadAppPath() + _T("Fonts\\") + bigFontFileNameStr, m_destPath + bigFontFileNameStr, FALSE);
		else
			lostFontSet.insert(bigFontFileNameStr);
	}
	delete pTSTblIter;

	if (lostFontSet.size() > 0)
	{
		CString writeStr = _T("提取字体时发现缺失如下字体：\r\n");
		std::set<CString>::iterator lostIter = lostFontSet.begin();
		for (; lostIter != lostFontSet.end(); lostIter++)
		{
			writeStr += (*lostIter + _T("\r\n"));
		}
		try
		{
			CStdioFile file;
			file.Open(m_destPath + _T("缺失字体.txt"), CFile::modeCreate | CFile::modeWrite | CFile::typeText);
			file.WriteString(writeStr);
			file.Close();
		}
		catch (...)
		{
			AfxMessageBox(writeStr);
		}
	}
	ShellExecute(acedGetAcadFrame()->m_hWnd, _T("open"), _T("explorer.exe"), m_destPath, NULL, SW_SHOWNORMAL);
}

