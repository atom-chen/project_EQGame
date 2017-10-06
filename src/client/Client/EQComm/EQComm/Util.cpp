#include "EQCommPCH.h"
#include "Util.h"
#include "UtilString.h"
#include "WeiboApi.h"

std::wstring Mbcs2Unicode( const char* pMbs )
{
	std::wstring wstrData;
	wchar_t* pwData = Util::String::MbcsToUnicode(pMbs);
	wstrData = pwData;
	ReleaseData(pwData);
	return wstrData;
}

std::string Unicode2Mbcs( const wchar_t* pUnicode )
{
	std::string strData;
	char* pData =  Util::String::UnicodeToMbcs(pUnicode);
	strData = pData;
	ReleaseData(pData);
	return strData;
}