/******************************************************************
* ����: ��_Mike
* ʱ��: 2010-4-23
* ����: ��Ϸ�������
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
��Ϸ�����������
*/
class ChatSystem : public Ogre::Singleton<ChatSystem>
{
public:
	ChatSystem(void);
	virtual ~ChatSystem(void);

	void setMainChatWnd(const char* wnd);

	/**
	*	���ܣ����������������ʾ������Ϣ
	*	������type:��0����ͨ�����������Ϣ���ѣ���1��ϵͳ��Ϣ����2��GM���ԣ���Ϸ����Ա��������Ϣ������3���������������ѣ�
	*		��4���������Ըı����Ϣ���ѣ���5���������죩��6������ʽ������ʾ������͹��ᣩ������Ƶ����
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