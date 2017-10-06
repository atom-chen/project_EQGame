/** 
@file  
@brief		请求的参数类,负责参数的组装。参数按照URL请求的参数格式组装。
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

	///增加参数
	/**
	@param prKey   in : 参数名
	@param pValue  in : 参数值
	*/
	TXWeibo_API void	AddParam(const char* pKey, const char* pValue);

	///获取参数
	/**
	@param pKey  in : 参数名
	*/
	/*返回值需调用ReleaseData释放*/
	TXWeibo_API char*	GetParam(const char* pKey);

	///增加图片名参数
	/**
	@param pKey    in : 参数名。
	@param pValue  in : 参数值
	*/
	TXWeibo_API void	AddPicNameParam(const char* pKey, const char* pValue);
	///得到图片名
	/**
	@param pKey  in : 参数名
	*/
	/*返回值需调用ReleaseData释放*/
	TXWeibo_API char*	GetPicNameParam(const char* pKey);

	/// 得到组装好的参数的字符串
	/*返回值需调用ReleaseData释放*/
	TXWeibo_API char*	GetUrlParamString();

	/// 清除所有参数，这样CWeiboParam对象可以重复使用
	TXWeibo_API void	Clear();

public:
	//SDK内部使用
	///获取customkey、customsecrect、tokenkey、tokensecrect，同时从参数中清除几个参数
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
	MapKey2Value									*m_pmapParam;		///< 保存参数的map，把参数名映射到参数值
	MapKey2Value									*m_pmapPicParam;    ///< 保存图片名的map，把文件参数名映射到文件名
};
