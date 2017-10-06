/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-18
* 描述: 微博
******************************************************************************/

#include "EQCommPCH.h"
#include "MicroBlog.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "NetMsgType.h"
#include "EnumDef.h"
#include "EQOgreSys.h"
#include "CommFunc.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"


//--------------------------------------------------------------------
template<> MicroBlogManager* Ogre::Singleton<MicroBlogManager>::ms_Singleton = 0;
//--------------------------------------------------------------------
MicroBlogManager::MicroBlogManager(void)
:m_sendType(SOT_NONE)
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_weibo_access_secret, NewMsgHandle(MicroBlogManager, handle_weibo_access_secret));
}
//--------------------------------------------------------------------
MicroBlogManager::~MicroBlogManager(void)
{
}
//--------------------------------------------------------------------
std::string MicroBlogManager::fullScreenShot(std::string dirPath)
{
	// 若目录路径不存在，则自动创建
	ForceDir(dirPath);
	// 截图前强制渲染一遍
	EQOgreSys::getSingleton().getRenderWindow()->update(true);
	// 截屏并返回包含完整目录路径的文件名
	return EQOgreSys::getSingleton().getRenderWindow()->writeContentsToTimestampedFile(dirPath, ".jpg");
}
//--------------------------------------------------------------------
std::string MicroBlogManager::partScreenShot(float x, float y, float w, float h, std::string dirPath)
{
	// 若目录路径不存在，则自动创建
	ForceDir(dirPath);
	// 获取游戏渲染窗口和计时器
	Ogre::RenderWindow *renderWindow = EQOgreSys::getSingleton().getRenderWindow();
	Ogre::Timer *timer = EQOgreSys::getSingleton().getRoot()->getTimer();
	// 截图前强制渲染一遍
	renderWindow->update(true);
	// 根据当前时间生成文件名
	struct tm *pTime;
	time_t ctTime; 
	time(&ctTime);
	pTime = localtime(&ctTime);
	std::ostringstream oss;
	oss	<< std::setw(2) << std::setfill('0') << (pTime->tm_mon + 1)
		<< std::setw(2) << std::setfill('0') << pTime->tm_mday
		<< std::setw(2) << std::setfill('0') << (pTime->tm_year + 1900)
		<< "_" << std::setw(2) << std::setfill('0') << pTime->tm_hour
		<< std::setw(2) << std::setfill('0') << pTime->tm_min
		<< std::setw(2) << std::setfill('0') << pTime->tm_sec
		<< std::setw(3) << std::setfill('0') << (timer->getMilliseconds() % 1000);
	Ogre::String filename = dirPath + Ogre::String(oss.str()) + ".jpg";
	// 保存全屏
	unsigned int winW = renderWindow->getWidth(), winH = renderWindow->getHeight();
	Ogre::PixelFormat pf = renderWindow->suggestPixelFormat();
	unsigned int bytes = Ogre::PixelUtil::getNumElemBytes(pf);
	Ogre::uchar *full = OGRE_ALLOC_T(Ogre::uchar, winW * winH * bytes, Ogre::MEMCATEGORY_RENDERSYS);
	Ogre::PixelBox pb(winW, winH, 1, pf, full);
	renderWindow->copyContentsToMemory(pb);
	Ogre::Image fullImage = Ogre::Image().loadDynamicImage(full, winW, winH, 1, pf, false, 1, 0);
	// 保存部分
	unsigned int width = (unsigned int)ceil(w), height = (unsigned int)ceil(h);
	int xPos = (int)ceil(x), yPos = (int)ceil(y);
	Ogre::uchar *part = OGRE_ALLOC_T(Ogre::uchar, width * height * bytes, Ogre::MEMCATEGORY_RENDERSYS);
	for (unsigned int row=0; row<height; ++row)	// 行
	{
		Ogre::uchar *temp = part + row * width * bytes;
		for (unsigned int col=0; col<width; ++col)	// 列
		{
			Ogre::ColourValue val = fullImage.getColourAt(xPos + col, yPos + row, 0);
			Ogre::PixelUtil::packColour(val, pf, (void*)(temp + col * bytes));
		}
	}
	Ogre::Image().loadDynamicImage(part, width, height, pf).save(filename);
	OGRE_FREE(part, Ogre::MEMCATEGORY_RENDERSYS);
	OGRE_FREE(full, Ogre::MEMCATEGORY_RENDERSYS);
	// 截屏并返回包含完整目录路径的文件名
	return filename;
}
//--------------------------------------------------------------------
void MicroBlogManager::SendTo(MicroBlogType mbType, SendOperatorType soType, std::string text, std::string image)
{
	switch (mbType)
	{
	case MBT_QQ:
		{
			m_sendType = soType;
			m_qqText = text;
			m_qqImage = image;

			if (m_qqWeibo.IsBind())	// 已绑定
			{
				_sendToQQ(soType, text, image);
			}
			else	// 未绑定
			{
				req_get_weibo_access_secret msg;
				sGameMgr.getEQMsgManager()->Send(msg);
			}
		}
		break;
	case MBT_SINA:
		break;
	default:
		return;
	}
}
//--------------------------------------------------------------------
void MicroBlogManager::handle_weibo_access_secret(MsgPack &recBuff)
{
	notify_weibo_access_secret data;
	data.decode(recBuff);

	m_qqWeibo.SetAccessKey((char*)data.tx_access_key.c_str(), (char*)data.tx_access_secret.c_str());

	if (m_qqWeibo.IsBind())	// 已绑定
	{
		_sendToQQ(m_sendType, m_qqText, m_qqImage);
	}
	else	// 未绑定
	{
		luabind::call_function<void>(sLuaMgr.getL(), "MicroBlog_showFirstUseTip");	// 弹出第一次注册提示
	}
}
//--------------------------------------------------------------------
void MicroBlogManager::saveAccessSecret(MicroBlogType type, std::string verifier)
{
	switch (type)
	{
	case MBT_QQ:
		_saveQQ(verifier);
		_sendToQQ(m_sendType, m_qqText, m_qqImage);
		break;
	case MBT_SINA:
		break;
	default:
		return;
	}
}
//--------------------------------------------------------------------
void MicroBlogManager::getTokenOfQQ(void)
{
	m_qqWeibo.GetToken();
}
//--------------------------------------------------------------------
void MicroBlogManager::_sendToQQ(SendOperatorType soType, std::string text, std::string image)
{
	switch (soType)
	{
	case SOT_UPLOAD:
		_uploadToQQ(text, image);
		break;
	case SOT_UPDATEHEAD:
		_updateHeadToQQ(image);
		break;
	case SOT_ALL:
		_uploadToQQ(text, image);
		_updateHeadToQQ(image);
		break;
	default:
		return;
	}
}
//--------------------------------------------------------------------
void MicroBlogManager::_uploadToQQ(std::string text, std::string image)
{
	m_qqWeibo.Upload((char*)text.c_str(), (char*)image.c_str());
}
//--------------------------------------------------------------------
void MicroBlogManager::_updateHeadToQQ(std::string image)
{
	m_qqWeibo.UpdateHead((char*)image.c_str());
}
//--------------------------------------------------------------------
void MicroBlogManager::_saveQQ(std::string verifier)
{
	std::string accessKey, accessSecret;
	m_qqWeibo.GetAccessKey((char*)verifier.c_str(), accessKey, accessSecret);

	req_save_access_secret msg;
	msg.tx_access_key = accessKey;
	msg.tx_access_secret = accessSecret;
	msg.sina_access_key = "";
	msg.sina_access_secret = "";
	sGameMgr.getEQMsgManager()->Send(msg);
}
//--------------------------------------------------------------------