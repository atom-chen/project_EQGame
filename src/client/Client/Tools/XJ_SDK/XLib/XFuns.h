#pragma once

#ifndef __XFuns_H
#define __XFuns_H

#include <string>
#include <vector>

class XFuns
{
public:
	/*
	说明：Unicode到多字节字符串转换
	参数
	   pwszSrc：Unicode字符串
    返回：多字节std:string
	*/
	static std::string WChar2Ansi(const wchar_t * pwszSrc);
	/*
	说明：Unicode到多字节字符串转换
	参数
	inputws：Unicode字符串std:wstring
	返回：多字节std:string
	*/
	static std::string WS2S(std::wstring& inputws);
	/*
	说明：多字节到Unicode字符串转换
	参数
	pszSrc：多字节字符串
	nLen：字符串长度
	返回：多字节std:wstring
	*/
	static std::wstring Ansi2WChar(const char * pszSrc, int nLen);
	/*
	说明：多字节到Unicode字符串转换
	参数
	s：多字节字符串std:string
	返回：多字节std:wstring
	*/
	static std::wstring S2WS(const std::string& s);
	/*
	说明：检查一个目录或完整文件名的目录是否存在
	参数
	dir：目录或完整文件名
	create：若不存在是否创建，默认为true
	返回：是否创建成功 true/false
	*/
	static bool CheckDir(const char *dir, bool create = true);
	/*
	说明：展开一个含有环境变量的字符串
	参数
	str：需要展开的字符串
	返回：
	*/
	static void ExpandEnvVars(std::string &str);
	/*
	说明：判断一个exe是否运行
	参数
	szExeName：exe名称
	返回：
	*/
	static bool CheckIsExeRun(const char* szExeName);
	static bool CheckHasExeRun(const char **szExeArray, int cnt);
	static void DelDir(const char *fdir, bool delMe);
	static bool CheckFileExists(const char *file);
	static size_t GetFileSize(const char *file);
	static bool RunExe(const char *file, const char *dir, const char *vars, bool imm = false);
	static bool CheckFileMd5(const char *file, const char *md5);
	static bool Check2FileMd5( const char *file1, const char *file2 );
	static void SplitString(const char *str, std::vector<std::string> &strArry, char spc);
	static void ReplaceChars(std::string &str, const char *str1, const char *str2);

public:
	XFuns(void);
	~XFuns(void);
};

#endif