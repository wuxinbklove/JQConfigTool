#include "stdafx.h"
#include "gcharset.h"
using namespace std;

namespace GCharSet
{
    std::string MFC2STL(CString src)
    {
        if (src.IsEmpty())
        {
            return "";
        }
        std::string out;
#ifdef _UNICODE
        int size = src.GetLength() * 2;
        char* dest = new char[size + 1];
        int ret = WideCharToMultiByte(CP_ACP, NULL, src.GetBuffer(), 
                                      -1, dest, size, NULL, NULL);
        dest[size] = '\0';
        out = dest;
    #ifdef _WIN64
        delete dest;
    #else
        delete[] dest;
    #endif
#else
        out = src.GetBuffer(src.GetLength());
#endif
        src.ReleaseBuffer();
        return out;
	}

    gstring GetGString(CString src)
    {
#ifdef UNICODE
        std::wstring str(_T(""));
#else
        std::string str("");
#endif
        
        if (!src.IsEmpty())
        {
            str = src.GetBuffer();
            src.ReleaseBuffer();
        }
        return str;
    }

    std::string LocalToUTF8(CString local)
    {
        wchar_t* pUnicode = NULL;
        int size = 0;
#ifndef _UNICODE
        size = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)local, -1, NULL, 0);
        pUnicode = new wchar_t[size + 1];
        MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)local, -1, (LPWSTR)pUnicode, size);
#else
        pUnicode = local.GetBuffer(local.GetLength());
        local.ReleaseBuffer();
#endif

        size = WideCharToMultiByte(CP_UTF8, 0, pUnicode, -1, NULL, 0, NULL, NULL);
        char* pUft8 = new char[size + 1];
        WideCharToMultiByte(CP_UTF8, 0, pUnicode, -1, pUft8, size, NULL, NULL);
        //pUft8[size] = '\0';
        std::string utf8 = pUft8;
#ifdef _WIN64
        delete pUft8;
#else
        delete[] pUft8;
#endif
#ifndef _UNICODE
        delete[] pUnicode;
#endif
        return utf8;
    }

    CString UTF8ToLocal(std::string utf8)
    {
        WCHAR* strSrc = NULL;
        CString localStr;
        int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
        strSrc = new WCHAR[size + 1];
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, strSrc, size);
        localStr = strSrc;
#ifndef _UNICODE
        TCHAR* szRes = NULL;
        size = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
        szRes = new TCHAR[size + 1];
        WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, size, NULL, NULL);
        localStr = szRes;
        delete[] szRes;        
#endif
#ifdef _WIN64
        delete strSrc;
#else
        delete[] strSrc;
#endif
        return localStr;
    }

	wstring Local2W(CString src)
	{
#ifdef _UNICODE
		wstring result = src.GetBuffer(src.GetLength());
		src.ReleaseBuffer();
#else
		WCHAR* pUnicode;
		int size = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)src, -1, NULL, 0);
		pUnicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)src, -1, (LPWSTR)pUnicode, size);
		wstring result = pUnicode;
		delete[] pUnicode;
#endif
		return result;
	}

	/*void Local2U16BE(CString src, char*& dest, int& len)
	{
		wstring u16 = Local2W(src);
		len = u16.size() * 2 + 2;
		dest = new char[len];
		dest[0] = 0xFE;
		dest[1] = 0xFF;
		for (int i = 2; i < len; i += 2)
		{
			int index = (i - 2) / 2;
			dest[i] = (u16[index] & 0xFF00) >>8;
			dest[i + 1] = u16[index];
		}
	}*/

	string Local2U16BE(CString src)
	{
		wstring u16 = Local2W(src);
		string u16be = "<FEFF";
		//u16be.push_back(0xFE);
		//u16be.push_back(0xFF);
		int len = u16.size();
		for (int i = 0; i < len; i++)
		{
			unsigned char tmp = u16[i] >> 8;//(u16[i] & 0xFF00) >> 8;
			char str[4] = {0};
			sprintf(str, "%02x", tmp);
			u16be += str;
			//u16be.push_back(tmp);
			tmp = u16[i];// & 0x00FF;
			char str2[4] = {0};
			sprintf(str2, "%02x", tmp);
			u16be += str2;
			//u16be.push_back(tmp);
		}
		u16be += ">";
		
		return u16be;
	}

	CString A2Local(std::string src)
	{
		CString result;
#ifndef _UNICODE
		result = src.c_str();
#else
		WCHAR* pUnicode;
		int size = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
		pUnicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, (LPWSTR)pUnicode, size);
		result = pUnicode;
		delete[] pUnicode;
#endif
		return result;
	}

	CString URLEncode(CString& src)
	{
		CString result;
		string utf8 = LocalToUTF8(src);
		int len = utf8.size();
		for (int i = 0; i < len; i++)
		{
			CString tmp;
			if (isalnum(utf8[i]) || utf8[i] == '.' || utf8[i] == '-' || utf8[i] == '_' || utf8[i] == '=')
			{
				tmp.Format(_T("%c"), utf8[i]);
			}
			else
			{
				tmp.Format(_T("%%%2x"), (BYTE)utf8[i]);
			}
			result.Append(tmp);
		}
		return result;
	}
}