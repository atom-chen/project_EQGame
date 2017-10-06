#pragma once

#ifndef __SFuns_H
#define __SFuns_H

#include <string>

class SFuns
{
public:
	static std::string WChar2Ansi(const wchar_t * pwszSrc);
	static std::string WS2S(std::wstring& inputws);

	static std::wstring Ansi2WChar(const char * pszSrc, int nLen);
	static std::wstring S2WS(const std::string& s);

	static void Trim(std::string &s);

public:
	SFuns(void);
	~SFuns(void);
};

#endif