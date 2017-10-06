/** 
@file  
@brief		΢������ӿ�
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
	HTTPRESULT_OK   = 0,		///<  �ɹ�	
	HTTPRESULT_TIMEOUT,         ///<  ����ʱ
	HTTPRESULT_CANTRESOLVE,     ///<  ���ܽ�����ַ
	HTTPRESULT_CANTCONNECT,     ///<  ��������
	HTTPRESULT_ERROR,           ///<  HTTP��������ݴ��� 
	HTTPRESULT_TOOLAGRE,        ///<  ���ص�������̫��
	HTTPRESULT_FAIL,		    ///<  ʧ��
};

namespace  TXWeibo
{
	/// ������
	namespace   Param
	{
		///oauth��صĲ�����
		const std::string strCustomKey			= "strCustomKey";     
		const std::string strCustomSecrect		= "strCustomSecrect";
		const std::string strTokenKey			= "strTokenKey";
		const std::string strTokenSecrect		= "strTokenSecrect";
		const std::string strVerifyCode			= "oauth_verifier";
		const std::string strCallbackUrl		= "oauth_callback";


		///���ݸ�ʽ�Ĳ������Ͳ���ֵ
		const std::string strFormat				= "format";		 ///< ������ļ��ĸ�ʽ��strJson��strXml
		const std::string strJson				= "json";	
		const std::string strXml				= "xml";	
	}
}

class CWeiboParam;

/// �첽΢������ص��ӿ�
class  VWeiboCallback
{
public:
	/// �첽΢������ص�����
	/** 
		@param  eHttpRetCode    in :    Http������
		@param  pReqUrl			in :	����URL
		@param  oParam			in :	�����������Ĳ��������
		@param  pData			in :    ���ص�����
		@param  nLen            in :    ���ص����ݳ���
	*/	
	virtual void OnRequestComplete(HTTP_RESULTCODE eHttpRetCode,const char* pReqUrl, CWeiboParam oParam, const char*pData,int nLen) = 0;
};


/// ͬ����������
/**
    @param  pReqUrl				in :    ����URL
	@param	eHttpMethod			in :	HTTP���������,ȡֵö��EHttpMethod
	@param  oQWBlogParam		in :	�����������Ĳ��������
	@param  pResultData			out��	ͬ�����ص����ݣ������ReleaseData�ͷ�
	@param  nLen				out:    ͬ�����������ݳ���

	@retval		HTTPRESULT_OK:�ɹ�	����:ʧ��
*/
HTTP_RESULTCODE TXWeibo_API  SyncRequest(const char* pReqUrl, EHttpMethod eHttpMethod, 
								CWeiboParam oQWBlogParam, char* &pResultData,int &nLen);


/// �첽��������
/** 
	@param  pReqUrl				in :    ����URL
	@param	eHttpMethod			in :	HTTP��������ͣ�ȡֵö��EHttpMethod
	@param  oQWBlogParam		in :	�����������Ĳ��������
	@param  pCallback			in ��	�ص�ָ��

	@retval		true:�ɹ�   false:ʧ��
*/
bool TXWeibo_API  AsyncRequest(const char* pReqUrl,  EHttpMethod eHttpMethod, 
								CWeiboParam oQWBlogParam, VWeiboCallback * pCallback );

///�ͷ���SDK��������ڴ�
void TXWeibo_API  ReleaseData(void* p);