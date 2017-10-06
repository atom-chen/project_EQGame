/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		6/9/2010	9:32
* �ļ�: 	MsgSystem.h
* ����:		ϵͳ��Ϣ
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
		MSG_CHATBOX		= 0x00000001, //�����
		MSG_MSGBOX		= 0x00000002, //�Ի���
		MSG_ALL			= 0x00000003,
	};

	MsgSystem();

	//��ʾ�������Ϣ
	void showServerMsg();
	virtual void update();
	void setCreateMsgBoxTime();

	//��ʾ�ͻ�����Ϣ
	void showClientMsg(MSG_TYPE type, uint id);
	void showClientMsg(MSG_TYPE type, std::string msg);

private:
	DWORD mcreateMsgBoxTime;

protected:
private:
};
#define sSysMsg MsgSystem::getSingleton()
#endif