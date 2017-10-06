#include "SFuns.h"

#include "Windows.h"

SFuns::SFuns(void)
{
}

SFuns::~SFuns(void)
{
}

//==============================================================================================================================
//Converting a WChar string to a Ansi string
std::string SFuns::WChar2Ansi(const wchar_t * pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen<= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;

	std::string strTemp(pszDst);
	delete [] pszDst;

	return strTemp;
}

std::string SFuns::WS2S(std::wstring& inputws)
{
	return WChar2Ansi(inputws.c_str());
}

//Converting a Ansi string to WChar string
std::wstring SFuns::Ansi2WChar(const char * pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if(nSize <= 0) return std::wstring(L"");

	WCHAR *pwszDst = new WCHAR[nSize+1];
	if( NULL == pwszDst) return std::wstring(L"");

	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
		for(int i = 0; i < nSize; i ++)
			pwszDst[i] = pwszDst[i+1];

	std::wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}

std::wstring SFuns::S2WS(const std::string& s)
{ 
	return Ansi2WChar(s.c_str(),s.size());
} 

void SFuns::Trim( std::string &s )
{
	size_t i, c;

	for(i = 0; i < s.size(); i ++)
	{
		if(s[i] != ' ' && s[i] != '	')
			break;
	}
	if(i)
		s.erase(0, i);

	if(s.size())
	{
		c = 0;
		i = s.size() - 1;
		while(1)
		{
			if(s[i] != ' ' && s[i] != '	')
				break;

			c ++;

			if(i == 0)
				break;

			i --;
		}

		if(c)
			s.erase(i, c);
	}

}