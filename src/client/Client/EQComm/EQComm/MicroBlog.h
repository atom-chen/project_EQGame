/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-18
* 描述: 微博
******************************************************************************/
#ifndef MicroBlog_H
#define MicroBlog_H

#include "EQMainPrerequisites.h"
#include "MsgHandleRegist.h"
#include "NetPacket.h"
#include "QQWeibo.h"


class MicroBlogManager : public Ogre::Singleton<MicroBlogManager>, public MemberMsgHandleRegistHelper
{
public:
	// 微博类型
	enum MicroBlogType
	{
		MBT_QQ = 1,		// QQ微博
		MBT_SINA = 2,	// 新浪微博
	};

	// 发布操作类型
	enum SendOperatorType
	{
		SOT_NONE,		// 空操作
		SOT_UPLOAD,		// 只发布带图片的微博内容
		SOT_UPDATEHEAD,	// 只更新微博头像
		SOT_ALL			// 同时以上两种操作
	};

public:
	MicroBlogManager(void);
	~MicroBlogManager(void);

	/*	截取全屏
	*	dirPath：截图保存的目录路径
	*/
	std::string fullScreenShot(std::string dirPath);
	/*	截取指定区域
	*	x，y，w，h：所要截取的位置坐标和宽高；dirPath：截图保存的目录路径
	*/
	std::string partScreenShot(float x, float y, float w, float h, std::string dirPath);

	void handle_weibo_access_secret(MsgPack &recBuff);

	void SendTo(MicroBlogType mbType, SendOperatorType soType, std::string text, std::string image);	// 上传

	void saveAccessSecret(MicroBlogType mbType, std::string verifier);	// 保存验证码

	void getTokenOfQQ();	// 打开QQ授权网站

protected:
	void _sendToQQ(SendOperatorType soType, std::string text, std::string image);

	void _uploadToQQ(std::string text, std::string image);

	void _updateHeadToQQ(std::string image);

	void _saveQQ(std::string verifier);

protected:
	QQWeibo m_qqWeibo;
	SendOperatorType m_sendType;
	std::string m_qqText;
	std::string m_qqImage;

};	//	end of class MicroBlogManager


#endif	//	end of guard MicroBlog_H
