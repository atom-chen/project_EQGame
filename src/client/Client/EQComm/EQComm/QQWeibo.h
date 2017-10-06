#pragma once

// #include "WeiboApi.h"
#include "txweibo/WeiboApi.h"
#include <string>


// CLoginDlg dialog

class QQWeibo : public VWeiboCallback
{
public:
	QQWeibo();
	~QQWeibo();

    // 是否已经绑定
    bool IsBind();

    void SetAccessKey(char* AccessKey, char*AccessSecret);

    bool GetToken();
	bool GetAccessKey(char* Verifier, std::string& AccessKey, std::string& AccessSecret);

    // 上传带图片的微博
    int Upload(char* content, char *PicPath);

	// 更新微博头像(2011/5/24 add by hezhr)
	int UpdateHead(char *PicPath);

    // 异步请求回调函数，工作在主线程
    void OnRequestComplete( HTTP_RESULTCODE eHttpRetCode,const char* pReqUrl, CWeiboParam oParam, const char*pData,int nLen );
public:
	// //appkey
	std::string m_sstrAppKey;
	// AppSecret
	std::string m_sstrAppSecret;
	// AccessKey
	std::string m_sstrAccessKey;
	// AccessSecret
	std::string m_sstrAccessSecret;

	std::string m_ssOauth_Verify;

protected:

	std::string m_sstrTokenKey;
	std::string m_sstrTokenSecrect;
};
