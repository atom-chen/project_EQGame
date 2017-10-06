/******************************************************************************
* 作者: 何展然
* 时间: 2010-11-4
* 描述: 公告系统
******************************************************************************/
#ifndef BroadCastSystem_H
#define BroadCastSystem_H


#include "EQMainPrerequisites.h"
#include "Scroll.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include <Windows.h>


/*************************************************************************
*	公告系统
*************************************************************************/
class BroadCastSystem : public Ogre::Singleton<BroadCastSystem>
{
public:
	BroadCastSystem();
	~BroadCastSystem();
	// 定点公告
	void saveMsgAT(std::string id, std::string msg, int times);	// 保存公告内容
	void deleteMsgAT(std::string id);	// 删除指定消息
	bool isPlayingMsgAT(std::string id);	// 判断是否正在播放的消息
	void setCalledFuncNameAT(std::string beginName, std::string name, std::string endName);
	void setPlayParamsAT(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playAT(std::string pare, std::string child);	// 播放
	void stopAT(void);	// 停止播放
	void saveOldMsgAT(void);
	int playStateAT(void);	// 播放状态
	// GM公告
	void saveMsgGM(std::string id, std::string msg, int times);
	void deleteMsgGM(std::string id);
	bool isPlayingMsgGM(std::string id);
	void setCalledFuncNameGM(std::string beginName, std::string name, std::string endName);
	void setPlayParamsGM(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playGM(std::string pare, std::string child);
	void stopGM(void);
	// 玩家行为公告
	void saveMsgBE(std::string id, std::string msg, int times);
	void setCalledFuncNameBE(std::string beginName, std::string name, std::string endName);
	void setPlayParamsBE(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playBE(std::string pare, std::string child);
	void stopBE(void);
	// 更新
	void update(void);
	// 请求固定的公告
	void reqBroadCast(int type);

protected:
	// 定点公告响应函数
	static int _playBeginAT(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagAT = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", true);
		return 0;
	}
	static int _playEndAT(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagAT = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", false);
		return 0;
	}
	// GM公告响应函数
	static int _playBeginGM(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagGM = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", true);
		return 0;
	}
	static int _playEndGM(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagGM = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", false);
		return 0;
	}
	// 玩家行为公告响应函数
	static int _playBeginBE(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagBE = 1;
		return 0;
	}
	static int _playEndBE(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagBE = 1;
		return 0 ;
	}

protected:
	// 定点公告
	Scroll m_broadCastAT;	// 定点公告滚动效果控制器
	bool m_playFlagAT;		// 播放标识（true，在播放中）
	int m_playBeginFlagAT;	// 值为（1、0）
	int m_playEndFlagAT;	// 值为（1、0）
	bool m_stopATByGM;		// true（被GM公告类型停止），false（被优先级高的本类型公告停止）
	// GM公告
	Scroll m_broadCastGM;	// GM公告滚动效果控制器
	bool m_playFlagGM;		// 播放标识（true，在播放中）
	int m_playBeginFlagGM;
	int m_playEndFlagGM;
	// 玩家行为公告
	Scroll m_broadCastBE;	// 玩家行为公告闪烁效果控制器
	bool m_playFlagBE;		// 播放标识（true，在播放中）
	int m_playBeginFlagBE;
	int m_playEndFlagBE;
	DWORD m_startTimeBE;	// 开始播放的时刻

};	//	end of class BroadCastSystem


#endif	//	end of guard BroadCastSystem_H
