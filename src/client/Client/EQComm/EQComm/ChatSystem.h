/******************************************************************
* 作者: 麦_Mike
* 时间: 2010-4-23
* 描述: 游戏聊天管理
*       
******************************************************************/
#ifndef ChatSystem_H
#define ChatSystem_H
#include "CEGUI.h"
#include "CEGUIEx/CEGUIMultiLineBox.h"

enum eChatState
{
	CS_SCENE,
	CS_PRIVATE,
};
/**
游戏聊天管理单件体
*/
class ChatSystem : public Ogre::Singleton<ChatSystem>
{
public:
	ChatSystem(void);
	virtual ~ChatSystem(void);

	void setMainChatWnd(const char* wnd);

	/**
	*	功能：在聊天输出框中显示各种消息
	*	参数：type:（0，普通场景聊天的信息提醒）（1，系统消息）（2，GM发言（游戏管理员发出的消息））（3，好友上下线提醒）
	*		（4，人物属性改变的信息提醒）（5，密语聊天）（6，集团式聊天显示（家族和公会）的聊天频道）
	*/
	void putTextOnChatBox(std::string buf, int type = 0,bool needMask = true);
	void setTalkText(std::string buf, int type = 0);
	//
	void talkPickObj();
	void setSysMsgText(std::string );

	void chgChatState(int state){ mChatState = state; }
	int  getChatState(){ return mChatState; }

	void setTalktoAccount(std::string acc) { mTalktoAccount = acc; }
	void setTalktoName(std::string name) { mTalktoName = name; }

	std::string getTalktoAccount(void) { return mTalktoAccount; }	// (hezhr)
	std::string getTalktoName(void) { return mTalktoName; }	// (hezhr)

	//
	void requestTalk(/*const char* dest,  const char* src*/);
private:
	void _showMyText();
	void _talkPrivate();
	void _talkScene();

	CEGUI::MultiLineBox	*mMainChatWnd;
	CEGUI::Window	*mTalkTextWnd;

	int				mChatState;
	std::string		mTalktoAccount;
	std::string		mTalktoName;
	std::string		mTextPrefix;
};

#endif