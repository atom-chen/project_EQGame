#include "EQCommPCH.h"
#include "QQWeibo.h"
#include "WeiboParam.h"
#include "Util.h"
#include <ShellAPI.h>
#include <windows.h>
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
using namespace std;

// QQWeibo dialog
QQWeibo::QQWeibo()
{
    m_sstrAppKey = "e6efdcc43ad64c48a4ebee63c50505c0";
    m_sstrAppSecret = "b23073355b8f16125cc32635034cbf95";
}

QQWeibo::~QQWeibo()
{

}


bool QQWeibo::GetToken()
{
    if (!m_sstrAccessSecret.empty())
        return true;

	string strCallbackUrl = "null";//"http://www.qq.com";

	CWeiboParam oParam;

	std::string c_strCustomKey		= TXWeibo::Param::strCustomKey;
	std::string c_strCustomSecrect	= TXWeibo::Param::strCustomSecrect;
	std::string c_oauthCallback		= TXWeibo::Param::strCallbackUrl;
	

	oParam.AddParam(c_strCustomKey.c_str(), m_sstrAppKey.c_str());
	oParam.AddParam(c_strCustomSecrect.c_str(), m_sstrAppSecret.c_str());
	oParam.AddParam(c_oauthCallback.c_str(), strCallbackUrl.c_str());

	string strUrl = "https://open.t.qq.com/cgi-bin/request_token";

	char* pRetData = NULL;
	int nLen = 0;
	HTTP_RESULTCODE  eErrorCode =  SyncRequest(strUrl.c_str(), EHttpMethod_Get, oParam, pRetData,nLen);
	if(eErrorCode != HTTPRESULT_OK || pRetData == NULL)
	{
		return false;
	}
   
    string strResult = pRetData;
   
	ReleaseData(pRetData);

	string strMark1 = "oauth_token=";
	string strMark2 = "&oauth_token_secret=";
	string strMark3 = "&oauth_callback_confirmed=";

	string::size_type nTokenPos = strResult.find(strMark1);
	if (nTokenPos == 0)
	{
		string::size_type nSecrectPos = strResult.find(strMark2);
		if ((nSecrectPos != string::npos) && (nSecrectPos > nTokenPos))
		{
			m_sstrTokenKey = strResult.substr(strMark1.length(), nSecrectPos - strMark1.length());

			string::size_type nCallbackPos = strResult.find(strMark3);
			if (nTokenPos != string::npos)
			{
				m_sstrTokenSecrect = strResult.substr(nSecrectPos + strMark2.length(), nCallbackPos - (nSecrectPos + strMark2.length()));
			}
		}
	}

    string strVerify = "http://open.t.qq.com/cgi-bin/authorize";
    strVerify += "?oauth_token=";
    strVerify += m_sstrTokenKey;
    ShellExecute( NULL, "open", strVerify.c_str(), NULL, NULL, SW_SHOWNORMAL); 
    return true;
}

bool QQWeibo::GetAccessKey(char* Verifier, std::string &AccessKey, std::string& AccessSecret)
{
    m_ssOauth_Verify = Verifier;

	CWeiboParam oParam;

	std::string c_strCustomKey		= TXWeibo::Param::strCustomKey;
	std::string c_strCustomSecrect	= TXWeibo::Param::strCustomSecrect;
	std::string c_strTokenKey		= TXWeibo::Param::strTokenKey;
	std::string c_strTokenSecrect	= TXWeibo::Param::strTokenSecrect;

	oParam.AddParam(c_strCustomKey.c_str(), m_sstrAppKey.c_str());
	oParam.AddParam(c_strCustomSecrect.c_str(), m_sstrAppSecret.c_str());

	oParam.AddParam(c_strTokenKey.c_str(), m_sstrTokenKey.c_str());
	oParam.AddParam(c_strTokenSecrect.c_str(), m_sstrTokenSecrect.c_str());

	size_t nSizeReturn = 0;

	oParam.AddParam("oauth_verifier", m_ssOauth_Verify.c_str());

	string strUrl = "https://open.t.qq.com/cgi-bin/access_token";
	
	char* pRetData = NULL;
	int nLen = 0;
	HTTP_RESULTCODE eHttpCode = SyncRequest(strUrl.c_str(), EHttpMethod_Get, oParam, pRetData,nLen);

	if(eHttpCode != HTTPRESULT_OK || pRetData == NULL)
	{
		return false;
	}
  
	string strResult = pRetData;
 
	ReleaseData(pRetData);

	string strMark1 = "oauth_token=";
	string strMark2 = "&oauth_token_secret=";
	string strMark3 = "&name=";

	string::size_type nTokenPos = strResult.find(strMark1);
	if (nTokenPos == 0)
	{
		string::size_type nSecrectPos = strResult.find(strMark2);
		if ((nSecrectPos != string::npos) && (nSecrectPos > nTokenPos))
		{
			m_sstrAccessKey = strResult.substr(strMark1.length(), nSecrectPos - strMark1.length());
            AccessKey =m_sstrAccessKey;

			string::size_type nCallbackPos = strResult.find(strMark3);
			if (nTokenPos != string::npos)
			{
				m_sstrAccessSecret = strResult.substr(nSecrectPos + strMark2.length(), nCallbackPos - (nSecrectPos + strMark2.length()));
				AccessSecret = m_sstrAccessSecret;
			}
		}
		return true;
	}
	return false;
}

int QQWeibo::Upload( char *content, char* PicPath )
{
    string strUrl = "http://open.t.qq.com/api/t/add_pic";

    CWeiboParam oParam;
    //以下四个参数在每次请求必须带上
    std::string c_strCustomKey		= TXWeibo::Param::strCustomKey;
    std::string c_strCustomSecrect	= TXWeibo::Param::strCustomSecrect;
    std::string c_strTokenKey		= TXWeibo::Param::strTokenKey;
    std::string c_strTokenSecrect	= TXWeibo::Param::strTokenSecrect;

    oParam.AddParam(c_strCustomKey.c_str(), m_sstrAppKey.c_str());
    oParam.AddParam(c_strCustomSecrect.c_str(), m_sstrAppSecret.c_str());
    oParam.AddParam(c_strTokenKey.c_str(),m_sstrAccessKey.c_str());
    oParam.AddParam(c_strTokenSecrect.c_str(),m_sstrAccessSecret.c_str());


    oParam.AddParam("content", content);
    //上传图片接口，在这里添加图片信息
    oParam.AddPicNameParam("pic", PicPath);

    //char *pResultData;
    //int len;
    //HTTP_RESULTCODE result = SyncRequest(strUrl.c_str(),  EHttpMethod_Post, oParam, pResultData, len );
    if(!AsyncRequest(strUrl.c_str(), EHttpMethod_Post, oParam, this))
        return -1;
    return 0;
}

int QQWeibo::UpdateHead(char *PicPath)
{
	string strUrl = "http://open.t.qq.com/api/user/update_head";

	CWeiboParam oParam;
	//以下四个参数在每次请求必须带上
	std::string c_strCustomKey		= TXWeibo::Param::strCustomKey;
	std::string c_strCustomSecrect	= TXWeibo::Param::strCustomSecrect;
	std::string c_strTokenKey		= TXWeibo::Param::strTokenKey;
	std::string c_strTokenSecrect	= TXWeibo::Param::strTokenSecrect;

	oParam.AddParam(c_strCustomKey.c_str(), m_sstrAppKey.c_str());
	oParam.AddParam(c_strCustomSecrect.c_str(), m_sstrAppSecret.c_str());
	oParam.AddParam(c_strTokenKey.c_str(),m_sstrAccessKey.c_str());
	oParam.AddParam(c_strTokenSecrect.c_str(),m_sstrAccessSecret.c_str());

	//上传图片接口，在这里添加图片信息
	oParam.AddPicNameParam("pic", PicPath);

	if(!AsyncRequest(strUrl.c_str(), EHttpMethod_Post, oParam, this))
		return -1;
	return 0;
}

/*异步请求回调函数，工作在主线程*/
void QQWeibo::OnRequestComplete( HTTP_RESULTCODE eHttpRetCode,const char* pReqUrl, CWeiboParam oParam, 
                                      const char*pData,int nLen )
{
	luabind::call_function<void>(sLuaMgr.getL(), "MicroBlog_showResult", eHttpRetCode, pData);
}

bool QQWeibo::IsBind()
{
    if (m_sstrAccessSecret.empty())
        return false;
    return true;
}

void QQWeibo::SetAccessKey( char* AccessKey, char*AccessSecret )
{
    m_sstrAccessKey = AccessKey;
    m_sstrAccessSecret = AccessSecret;
}