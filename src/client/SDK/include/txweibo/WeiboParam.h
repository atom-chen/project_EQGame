/** 
@file  
@brief		����Ĳ�����,�����������װ����������URL����Ĳ�����ʽ��װ��
@version	2010-11-16		

**@exmaple:
format=json&pageflag=0&reqnum=20

CWeiboParam oParam;
oParam.AddParam(TXWeibo::Param::strFormat,TXWeibo::Param::strJson);
oParam.AddParam("pageflag","0");
oParam.AddParam("reqnum","20");
*
*/

#pragma once
#include <map>
#include <string>
#include "WeiboApi.h"
#include "UtilString.h"

#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif


class CWeiboParam
{
public:
	TXWeibo_API CWeiboParam(void);
	TXWeibo_API CWeiboParam(const CWeiboParam& v);
	TXWeibo_API CWeiboParam& operator=(const CWeiboParam& v);
	TXWeibo_API ~CWeiboParam(void);

	///���Ӳ���
	/**
	@param prKey   in : ������
	@param pValue  in : ����ֵ
	*/
	TXWeibo_API void	AddParam(const char* pKey, const char* pValue);

	///��ȡ����
	/**
	@param pKey  in : ������
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	TXWeibo_API char*	GetParam(const char* pKey);

	///����ͼƬ������
	/**
	@param pKey    in : ��������
	@param pValue  in : ����ֵ
	*/
	TXWeibo_API void	AddPicNameParam(const char* pKey, const char* pValue);
	///�õ�ͼƬ��
	/**
	@param pKey  in : ������
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	TXWeibo_API char*	GetPicNameParam(const char* pKey);

	/// �õ���װ�õĲ������ַ���
	/*����ֵ�����ReleaseData�ͷ�*/
	TXWeibo_API char*	GetUrlParamString();

	/// ������в���������CWeiboParam��������ظ�ʹ��
	TXWeibo_API void	Clear();

public:
	//SDK�ڲ�ʹ��
	///��ȡcustomkey��customsecrect��tokenkey��tokensecrect��ͬʱ�Ӳ����������������
	/**
	@param strCustomKey			out : strCustomKey
	@param strCustomSecrect		out : strCustomSecrect
	@param strTokenKey			out : strTokenKey
	@param strTokenSecrect		out : strTokenSecrect
	*/
	void GetAndRemoveKey(std::string& strCustomKey, std::string& strCustomSecrect, 
		std::string& strTokenKey, std::string& strTokenSecrect );


	typedef	std::map<std::string, std::string>	MapKey2Value;
	MapKey2Value& GetParamMap();
	MapKey2Value& GetPicParamMap();

protected:
	MapKey2Value									*m_pmapParam;		///< ���������map���Ѳ�����ӳ�䵽����ֵ
	MapKey2Value									*m_pmapPicParam;    ///< ����ͼƬ����map�����ļ�������ӳ�䵽�ļ���
};
