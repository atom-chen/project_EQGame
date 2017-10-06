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

	// 请求登录
	virtual BOOL LoginRequest();
	// 请求创建角色
	BOOL CreatPlayerRequest();
	// 请求验证版本
	BOOL VersionRequest();
	// 请求角色列表
	BOOL RolesRequest();
	// 请示进入场景
	virtual BOOL ReqEnterGame();

	// 返回验证结果
	void VersionReply();
	// 返回登录结果
	virtual void LoginReply();
	// 返回角色列表
	virtual void RolesReply();
	// 返回创建角色结果
	void CreatPlayerReply();
	// 返回进入场景结果
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