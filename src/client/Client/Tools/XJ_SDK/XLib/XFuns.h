#pragma once

#ifndef __XFuns_H
#define __XFuns_H

#include <string>
#include <vector>

class XFuns
{
public:
	/*
	˵����Unicode�����ֽ��ַ���ת��
	����
	   pwszSrc��Unicode�ַ���
    ���أ����ֽ�std:string
	*/
	static std::string WChar2Ansi(const wchar_t * pwszSrc);
	/*
	˵����Unicode�����ֽ��ַ���ת��
	����
	inputws��Unicode�ַ���std:wstring
	���أ����ֽ�std:string
	*/
	static std::string WS2S(std::wstring& inputws);
	/*
	˵�������ֽڵ�Unicode�ַ���ת��
	����
	pszSrc�����ֽ��ַ���
	nLen���ַ�������
	���أ����ֽ�std:wstring
	*/
	static std::wstring Ansi2WChar(const char * pszSrc, int nLen);
	/*
	˵�������ֽڵ�Unicode�ַ���ת��
	����
	s�����ֽ��ַ���std:string
	���أ����ֽ�std:wstring
	*/
	static std::wstring S2WS(const std::string& s);
	/*
	˵�������һ��Ŀ¼�������ļ�����Ŀ¼�Ƿ����
	����
	dir��Ŀ¼�������ļ���
	create�����������Ƿ񴴽���Ĭ��Ϊtrue
	���أ��Ƿ񴴽��ɹ� true/false
	*/
	static bool CheckDir(const char *dir, bool create = true);
	/*
	˵����չ��һ�����л����������ַ���
	����
	str����Ҫչ�����ַ���
	���أ�
	*/
	static void ExpandEnvVars(std::string &str);
	/*
	˵�����ж�һ��exe�Ƿ�����
	����
	szExeName��exe����
	���أ�
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