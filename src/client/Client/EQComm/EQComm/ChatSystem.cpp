#include "EQCommPCH.h"
#include "ChatSystem.h"
#include "NetPacket.h"
#include "NetMsgType.h"
#include "INetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "ObjMgr.h"
#include "Player.h"
#include "GameObj.h"
#include "Ogre.h"
#include "CommFunc.h"
#include "MainPlayer.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "StringHandle.h"

//-------------------------------------------------------------
template<> ChatSystem* Ogre::Singleton<ChatSystem>::ms_Singleton = 0;

//--------------------------------------------------------------
ChatSystem::ChatSystem(void)
: mMainChatWnd(NULL)
, mTalkTextWnd(NULL)
, mChatState(CS_SCENE)
, mTalktoAccount("")
, mTalktoName("")
{
}
//--------------------------------------------------------------
ChatSystem::~ChatSystem(void)
{
}
//--------------------------------------------------------------
void ChatSystem::requestTalk()
{
	switch (mChatState)
	{
	case CS_SCENE:
		_showMyText();
		_talkScene();
		break;
	case CS_PRIVATE:
		_talkPrivate();
		chgChatState(CS_SCENE);
		break;
	}


}
//-------------------------------------------------------------
void ChatSystem::_talkScene()
{
	static req_chat_around chatAround;
	chatAround.content = mTalkTextWnd->getText().c_str();
	//LOGMSG(chatAround.content);
	sGameMgr.getEQMsgManager()->Send(chatAround);

	std::string playerName = MainPlayer::getSingleton().getName();
	std::string buf = GB2312ToUTF8("［");
	buf += playerName;
	buf += GB2312ToUTF8("］：");
	buf += chatAround.content;

	putTextOnChatBox(buf, 0);
}
//-------------------------------------------------------------
void ChatSystem::_talkPrivate()
{	
	const std::string textOneWnd = mTalkTextWnd->getText().c_str();
	// 判断是否存在前缀: XX悄悄对XX说
	if (textOneWnd.find(this->mTextPrefix) != 0)
		return;

	std::string sContent = std::string(textOneWnd.begin() + mTextPrefix.size(), textOneWnd.end());
	if (sContent.empty())
		return;
	std::string output;
	output = sStringHandle.stringFilter(const_cast<char *>(sContent.c_str()),0);
	static req_chat_tell chatTell;
	chatTell.content = output;

	chatTell.target_player = mTalktoAccount;	
	//LOGMSG(chatTell.content);
	sGameMgr.getEQMsgManager()->Send(chatTell);

	putTextOnChatBox(textOneWnd, 5);
}
//-------------------------------------------------------------
void ChatSystem::_showMyText()
{
	if(mTalkTextWnd == NULL)
		mTalkTextWnd = CEGUI::WindowManager::getSingleton().getWindow(
		"MainRoot/HMenubar/ETxt_Chat");
	 
	CEGUI::String talktext = ""; 
	talktext += mTalkTextWnd->getText();
	MainPlayer::getSingleton().showChatPaoPao(talktext.c_str());
}
//--------------------------------------------------------------
void ChatSystem::putTextOnChatBox(std::string buf, int type,bool needMask)
{
	if(buf.empty())
		return;

	if(mMainChatWnd == NULL)
		mMainChatWnd = (CEGUI::MultiLineBox*)(CEGUI::WindowManager::getSingleton().getWindow(
		"MainRoot/MTxt_ShowChatbox"));
	std::string output;
	if(needMask)
		output = sStringHandle.stringFilter(const_cast<char *>(buf.c_str()),0);
	else
		output = buf;
	std::vector<std::string> arr = Ogre::StringUtil::split(output, "\n");	// 除去末尾的'\n'(2011/6/14 add by hezhr)
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "parseChatShowBox", arr[0], type);	//	图文混排控件解析(hezhr)
}
//--------------------------------------------------------------
void ChatSystem::setTalkText(std::string buf, int type)
{
	if(mTalkTextWnd == NULL)
		mTalkTextWnd = CEGUI::WindowManager::getSingleton().getWindow(
		"MainRoot/HMenubar/ETxt_Chat");

	CEGUI::String sText = reinterpret_cast<const CEGUI::utf8 *>(buf.c_str());
	mTalkTextWnd->setText(sText);
	CEGUI::Editbox * pBox = dynamic_cast<CEGUI::Editbox *>(mTalkTextWnd);
	if (pBox)
	{
		// 这里使用正则表达式，来实现输入格式
		// 固定前缀
		pBox->setValidationString(sText + ".*"); 
		pBox->setCaratIndex(static_cast<size_t>(-1));
		pBox->activate();
	}
}
//--------------------------------------------------------------
void ChatSystem::setMainChatWnd(const char* wnd)
{
	mMainChatWnd = (CEGUI::MultiLineBox*)(CEGUI::WindowManager::getSingleton().getWindow(wnd));

	if (!mMainChatWnd)
		LOGERR("setMainChatWnd() invalid arg");
}
//-------------------------------------------------------------------------
bool EQMsgManager::handleByChat()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_chat_around:
		{
			notify_chat_around data;
			data.decode(mRecebuff);

			std::string buf = GB2312ToUTF8("［");
			buf += data.player_name; 
			buf += GB2312ToUTF8("］：");
			buf += data.content;

			Player* player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.account);
			if(player)
			{
				//add by hezhr
				Ogre::Real dist = player->getDistanceToTarget(MainPlayer::getSingleton().getPos());
				if (dist <= 960)	//960=600+480/2（为玩家间的距离）
				{
					ChatSystem::getSingleton().putTextOnChatBox(buf, 0);
					player->showChatPaoPao(data.content.c_str());
				}
			}
		}
		break;
	case NetMsgType::msg_notify_chat_big_expression:
		{
			notify_chat_big_expression data;
			data.decode(mRecebuff);
			Player* player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.account);
			if(player)
				player->showHeadupBigExpression(data.exp_id);
		}
		break;

	case NetMsgType::msg_notify_chat_tell:
		{
			notify_chat_tell data;
			data.decode(mRecebuff);

			std::string buf = GB2312ToUTF8("［");
			buf += data.speaker_name; 
			buf += GB2312ToUTF8("］悄悄地对你说：");
			buf += data.content;

			ChatSystem::getSingleton().putTextOnChatBox(buf, 5);
		}
		break;

	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------------------
void ChatSystem::talkPickObj()
{
	std::string buf = GB2312ToUTF8("你悄悄地对［");
	buf += mTalktoName;
	buf += GB2312ToUTF8("］说：");

	mTextPrefix = buf;

	chgChatState(CS_PRIVATE);
	setTalkText(mTextPrefix);
}

//-----------------huangdj(01-05-12)------------------------------------
void ChatSystem::setSysMsgText(std::string sysMsg)
{	
	std::string sysMsgHeader = GB2312ToUTF8("")/*("［系统］：")*/;

	if (sysMsg != "")
	{
		sysMsg = sysMsgHeader + sysMsg;
		putTextOnChatBox(sysMsg.c_str(), 1,false);
	}
}