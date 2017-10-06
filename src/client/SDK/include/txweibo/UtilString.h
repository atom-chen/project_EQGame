#pragma once
#include <string>

#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif

namespace Util
{
	namespace String
	{
		//��ͬ��ʽ�ַ���֮���ת������
        //����ֵ����������ReleaseData�ͷ�
		TXWeibo_API char*	    MbcsToUtf8(const char* pMbcs);
		TXWeibo_API char*	    Utf8ToMbcs(const char* pUtf8);
		TXWeibo_API wchar_t*	Utf8ToUnicode(const char* pUtf8);
		TXWeibo_API char*		UnicodeToUtf8(const wchar_t* pUnicode);
		TXWeibo_API wchar_t*	MbcsToUnicode(const char* pMbcs);
		TXWeibo_API char*		UnicodeToMbcs(const wchar_t* pUnicode);

		//�ַ��������ֵ�ת������
		TXWeibo_API bool		StringToInt(const char* pValue, int & nValue);
		TXWeibo_API bool		StringToDWord(const char* pValue, unsigned int & dwValue);
		TXWeibo_API bool		StringToInt64(const char* pValue, long long & i64Value);
		TXWeibo_API bool		StringToUnsignedInt64(const char* pValue, unsigned long long & ui64Value);

	
		///��aouth��ص�URL������뺯��
		//����ֵ����������ReleaseData�ͷ�
		TXWeibo_API char*		FormUrlEncode(const char* pEncode);
		TXWeibo_API char*		FormUrlDecode(const char* pDecode);
		inline int	UrlEncode(const char *source, char *dest, unsigned max);
	}
}
