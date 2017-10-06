#ifndef EQLogin_H
#define EQLogin_H

#include <string>
#include "gamestate.h"

class EQLogin :
	public GameState,
	public Ogre::Singleton<EQLogin>
{
public:
	EQLogin(void) {}
	virtual ~EQLogin(void) {}

	void init() {}
	bool handleMsg();

	// �����¼
	virtual BOOL LoginRequest();
	// ���󴴽���ɫ
	BOOL CreatPlayerRequest();
	// ������֤�汾
	BOOL VersionRequest();
	// �����ɫ�б�
	BOOL RolesRequest();
	// ��ʾ���볡��
	virtual BOOL ReqEnterGame();

	// ������֤���
	void VersionReply();
	// ���ص�¼���
	virtual void LoginReply();
	// ���ؽ�ɫ�б�
	virtual void RolesReply();
	// ���ش�����ɫ���
	void CreatPlayerReply();
	// ���ؽ��볡�����
	void EnterSceneReply();

	//
	void setServerIP(std::string ip){mServerIP = ip;}
	void setPort(int port){mServerPort=port;}
	void setAccount(std::string acc){mAccount=acc;}
	void setPassword(std::string pw){mPassword = pw;}
	void setUsername(std::string user){mUsername=user;}

protected:
	std::string	mAccount
		,mPassword
		,mUsername
		,mServerIP ;
	int mServerPort;
};

#endif