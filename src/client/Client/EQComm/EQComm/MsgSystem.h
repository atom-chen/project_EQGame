/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		6/9/2010	9:32
* 文件: 	MsgSystem.h
* 描述:		系统消息
******************
*********************************************************************/
#ifndef MSGSYSTEM_H
#define MSGSYSTEM_H
#include <Ogre.h>

class MsgSystem : public Ogre::Singleton<MsgSystem>
{
public:
	enum MSG_TYPE
	{
		MSG_CHATBOX		= 0x00000001, //聊天框
		MSG_MSGBOX		= 0x00000002, //对话框
		MSG_ALL			= 0x00000003,
	};

	MsgSystem();

	//显示服务端消息
	void showServerMsg();
	virtual void update();
	void setCreateMsgBoxTime();

	//显示客户端消息
	void showClientMsg(MSG_TYPE type, uint id);
	void showClientMsg(MSG_TYPE type, std::string msg);

private:
	DWORD mcreateMsgBoxTime;

protected:
private:
};
#define sSysMsg MsgSystem::getSingleton()
#endif