#pragma once
#include <string>
#include <map>

#ifndef  GOLD_EXPORT
#define  GOLD_EXPORT
#endif 

#ifdef UNICODE
#define gstring std::wstring
#else
#define gstring std::string
#endif

namespace GCharSet
{
    /*!
     @brief 把mfc的CString转换成stl的string
     */
    std::string GOLD_EXPORT MFC2STL(CString src);

    /*!
     @brief 转换CString，如果是多字节环境就转成string，如果是unicode，就转成wstring
     */
    gstring GOLD_EXPORT GetGString(CString src);

    /*!
     @brief 本地字符串转换成utf8
     */
    std::string GOLD_EXPORT LocalToUTF8(CString src);

    /*!
     @brief utf8转换到本地
     */
    CString GOLD_EXPORT UTF8ToLocal(std::string src);

	//
	std::wstring GOLD_EXPORT Local2W(CString src);

	//
	std::string GOLD_EXPORT Local2U16BE(CString src);
	//void GOLD_EXPORT Local2U16BE(CString src, char*& dest, int& len);

	//std::string 转MFC  
	//ansi转到本地
	CString GOLD_EXPORT A2Local(std::string src);

	//
	CString GOLD_EXPORT URLEncode(CString& src);

};
