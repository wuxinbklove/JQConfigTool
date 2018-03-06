#include "StdAfx.h"
#include "stdafx.h"
#include "gpath.h"
#include "shlwapi.h"


namespace GPath
{
	CString GetGoldAppPath()
	{
		TCHAR path[1024] = { 0 };
		GetModuleFileName(_hdllInstance, path, 1023);
		CString file(path);
		return GetFilePath(file);
	}

	CString GetGoldAppDataPath()
	{
		CString path;
		SHGetSpecialFolderPath(0, path.GetBuffer(MAX_PATH), CSIDL_APPDATA, 0);
		path.ReleaseBuffer();
		path += _T("\\JinQuConfigTool\\");
		if (!PathFileExists(path))
			CreateDirectory(path, NULL);
		return path;
	}

	CString GetAcadAppPath()
	{
		TCHAR path[1024] = { 0 };
		GetModuleFileName(NULL, path, 1023);
		CString file(path);
		return GetFilePath(file);
	}

	CString GOLD_EXPORT GetSystemFontsPath()
	{
		CString fontsPath;
		SHGetSpecialFolderPath(0, fontsPath.GetBuffer(MAX_PATH), CSIDL_FONTS, 0);
		fontsPath.ReleaseBuffer();
		return fontsPath + _T("\\");
	}

	CString GetFilePath(CString& file)
	{
		int index = file.ReverseFind('\\');
		if (index < 0)
		{
			//todo log
			return _T("");
		}
		return file.Left(index + 1);
	}

	CString GetFileType(CString& file)
	{
		int index = file.ReverseFind('.');
		if (index < 1)
		{
			return _T("");
		}
		return file.Right(file.GetLength() - index - 1);
	}

	CString GetFileTitle(const CString& file)
	{
		int index = file.ReverseFind('\\');
		if (index < 1)
		{
			return file;
		}
		return file.Right(file.GetLength() - index - 1);
	}

	CString GetFileNameNoExt(const CString& file)
	{
		int index = file.ReverseFind('.');
		if (index < 1)
		{
			return file;
		}
		return file.Left(index);
	}

	CString GetFileTitleNoExt(const CString& file)
	{
		CString title = GetFileTitle(file);
		return GetFileNameNoExt(title);
	}

	CString GetNewTypeFile(CString& file, CString newType)
	{
		if (file.IsEmpty() || newType.IsEmpty())
		{
			//todo log
			return _T("");
		}
		int index = file.ReverseFind('.');
		if (index < 1)
		{
			//todo log
			return _T("");
		}
		CString newFile = file.Left(index);
		if (newType.GetAt(0) != '.')
		{
			newFile += _T(".");
		}
		newFile += newType;
		return newFile;
	}

	CString AppendPath(CString path, CString append)
	{
		TCHAR srcPath[MAX_PATH + 1] = { 0 };
		lstrcpy(srcPath, path);
		PathAppend(srcPath, append);
		return CString(srcPath);
	}

	void GetValidPath(CString& path)
	{
		path.Replace(_T("\\"), _T("-"));
		path.Replace(_T("/"), _T("-"));
		path.Replace(_T(":"), _T("-"));
		path.Replace(_T("*"), _T("-"));
		path.Replace(_T("?"), _T("-"));
		path.Replace(_T("\""), _T("-"));
		path.Replace(_T("<"), _T("-"));
		path.Replace(_T(">"), _T("-"));
		path.Replace(_T("|"), _T("-"));
		path.Remove('\t');
		path.Remove('\n');
		path.Remove('\r');
		path.Remove(' ');
	}

	//
	CString GetTitleNoExt(CString& file)
	{
		int index = file.ReverseFind('.');
		if (index < 1)
		{
			return _T("");
		}
		return file.Left(index);
	}

	CString OpenUserManual(CString topic)
	{
		CString pageId;
		::GetPrivateProfileString(_T("UserManualConfig"), topic, _T(""), pageId.GetBuffer(100), 100, GPath::GetGoldAppPath() + _T("..\\UserManual\\UserManualConfig.ini"));
		pageId.ReleaseBuffer();
		CString url;
		if (topic.IsEmpty())
			url = GPath::GetGoldAppPath() + _T("..\\UserManual\\聞喘返過.chm");
		else
			url = CString(_T("mk:@MSITStore:")) + GPath::GetGoldAppPath() + _T("..\\UserManual\\聞喘返過.chm") + pageId;
		ShellExecute(NULL, _T("open"), _T("hh.exe"), url, NULL, SW_SHOW);
		return url;
	}
}
