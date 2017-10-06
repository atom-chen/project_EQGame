/** 
@file  
@brief		微博对外接口
@version	2010-11-18		
*/

#pragma once
#include <string>

#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif

enum  EHttpMethod
{
	EHttpMethod_Get,
	EHttpMethod_Post,
};

enum HTTP_RESULTCODE
{
	HTTPRESULT_OK   = 0,		///<  成功	
	HTTPRESULT_TIMEOUT,         ///<  请求超时
	HTTPRESULT_CANTRESOLVE,     ///<  不能解析地址
	HTTPRESULT_CANTCONNECT,     ///<  不能连接
	HTTPRESULT_ERROR,           ///<  HTTP请求的数据错误 
	HTTPRESULT_TOOLAGRE,        ///<  返回的数据量太大
	HTTPRESULT_FAIL,		    ///<  失败
};

namespace  TXWeibo
{
	/// 参数名
	namespace   Param
	{
		///oauth相关的参数名
		const std::string strCustomKey			= "strCustomKey";     
		const std::string strCustomSecrect		= "strCustomSecrect";
		const std::string strTokenKey			= "strTokenKey";
		const std::string strTokenSecrect		= "strTokenSecrect";
		const std::string strVerifyCode			= "oauth_verifier";
		const std::string strCallbackUrl		= "oauth_callback";


		///数据格式的参数名和参数值
		const std::string strFormat				= "format";		 ///< 请求的文件的格式：strJson或strXml
		const std::string strJson				= "json";	
		const std::string strXml				= "xml";	
	}
}

class CWeiboParam;

/// 异步微博请求回调接口
class  VWeiboCallback
{
public:
	/// 异步微博请求回调函数
	/** 
		@param  eHttpRetCode    in :    Http错误码
		@param  pReqUrl			in :	请求URL
		@param  oParam			in :	该请求所带的参数类对象
		@param  pData			in :    返回的数据
		@param  nLen            in :    返回的数据长度
	*/	
	virtual void OnRequestComplete(HTTP_RESULTCODE eHttpRetCode,const char* pReqUrl, CWeiboParam oParam, const char*pData,int nLen) = 0;
};


/// 同步请求数据
/**
    @param  pReqUrl				in :    请求URL
	@param	eHttpMethod			in :	HTTP请求的类型,取值枚举EHttpMethod
	@param  oQWBlogParam		in :	该请求所带的参数类对象
	@param  pResultData			out：	同步返回的数据，需调用ReleaseData释放
	@param  nLen				out:    同步返沪的数据长度

	@retval		HTTPRESULT_OK:成功	其它:失败
*/
HTTP_RESULTCODE TXWeibo_API  SyncRequest(const char* pReqUrl, EHttpMethod eHttpMethod, 
								CWeiboParam oQWBlogParam, char* &pResultData,int &nLen);


/// 异步请求数据
/** 
	@param  pReqUrl				in :    请求URL
	@param	eHttpMethod			in :	HTTP请求的类型，取值枚举EHttpMethod
	@param  oQWBlogParam		in :	该请求所带的参数类对象
	@param  pCallback			in ：	回调指针

	@retval		true:成功   false:失败
*/
bool TXWeibo_API  AsyncRequest(const char* pReqUrl,  EHttpMethod eHttpMethod, 
								CWeiboParam oQWBlogParam, VWeiboCallback * pCallback );

///释放在SDK中申请的内存
void TXWeibo_API  ReleaseData(void* p);