#pragma once

#ifndef GOLD_EXPORT
#ifdef GOLDTOOLS_MODULE
    #define GOLD_EXPORT
#else
    #define GOLD_EXPORT
#endif
#endif

namespace GPath
{
    /*!
     @brief 获取程序的安装路径
     */
    CString GOLD_EXPORT GetGoldAppPath();

	/*!
	@brief 获取程序AppData路径: 即%Appdata%/JinQuConfigTool
	*/
	CString GOLD_EXPORT GetGoldAppDataPath();


	/*!
	@brief 获取AutoCAD的安装路径
	*/
	CString GOLD_EXPORT GetAcadAppPath();

	/*!
	@brief 获取系统fonts目录
	*/
	CString GOLD_EXPORT GetSystemFontsPath();

    /*!
     @brief 获取文件路径的目录，包含\
     @param file 文件名
     */
    CString GOLD_EXPORT GetFilePath(CString& file);

    /*!
     @brief 得到文件类型（通过后缀名获取，不带.）
     */
    CString GOLD_EXPORT GetFileType(CString& file);

    /*!
     @brief 得到文件名，不包含路径部分
     */
    CString GOLD_EXPORT GetFileTitle(const CString& file);

    //得到不包含文件类型的名称
    CString GOLD_EXPORT GetFileNameNoExt(const CString& file);

    //得到文件名，不包含路径和文件类型
    CString GOLD_EXPORT GetFileTitleNoExt(const CString& file);

    /*!
     @brief 得到用newType替换类型的文件名
     */
    CString GOLD_EXPORT GetNewTypeFile(CString& file, CString newType);

    /*!
     @brief 合并路径
     @param path 基础路径
     @param append 需要合并的路径
     */
    CString GOLD_EXPORT AppendPath(CString path, CString append);

    /*!
     @brief 获取合法文件名，去除一些非法字符
     */
    //CString GOLD_EXPORT GetValidPath(CString& path);

    /*!
     @brief 获取合法文件名
     */
    void GOLD_EXPORT GetValidPath(CString& path);

	/*!
	@brief 打开topic主题帮助文档，返回值为文档路径。
	*/
	CString GOLD_EXPORT OpenUserManual(CString topic);
};