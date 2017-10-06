#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "EQMsgManager.h"

class CMessageManager : public EQMsgManager
{
public:
	static CMessageManager& getSingleton(void)
	{
		static CMessageManager mgr;
		return mgr;
	}
	
	CMessageManager() {}
	virtual ~CMessageManager(void) {}

protected:
	virtual bool logicProc();

	// 登录第三方场景
	BOOL EnterOtherSceneRequest();
	// 离开场景请求
	BOOL LeaveSceneRequest();
	//
	void notifyDayOrNight(ByteArray& recBuff);
	// 返回角色详细信息
	void PlayerDataReply(ByteArray& recBuff);
	// 返回其它玩家详细信息
	void OtherPlayerDataReply(ByteArray& recBuff);
	void PlayerPlayAnimation(ByteArray& recBuff);
	void notifyPlayerBodyState(ByteArray& recBuff);
	// 返回离开场景结果
	void LeaveSceneReply(ByteArray& recBuff);
	//其它模块
	bool handleByOthers();
	//重复登入
	bool handleLoginRepeat();
};

#define sMsgMgr CMessageManager::getSingleton()

#endif