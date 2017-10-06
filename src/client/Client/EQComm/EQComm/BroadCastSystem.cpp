/******************************************************************************
* 作者: 何展然
* 时间: 2010-11-4
* 描述: 公告系统
******************************************************************************/
#include "EQCommPCH.h"

#include "BroadCastSystem.h"
#include "NetMsgType.h"
#include "NetPacket.h"
#include "CommFunc.h"
#include "EnumDef.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"

//-----------------------------------------------------------------------
bool EQMsgManager::handleBroadCast()
{
	switch (mMsgtype)
	{
	case NetMsgType::msg_notify_sys_broadcast:
		{
			notify_sys_broadcast data;
			data.decode(mRecebuff);
			char buf[64] = "";
			_snprintf_s(buf, sizeof(buf), "%d", data.id);
			std::string id(buf);
			std::string content = data.content;
			if (3 != data.type)	// 非玩家行为公告，是GBK编码格式，需要这里转换为UTF8
			{
				content = GB2312ToUTF8(data.content.c_str());
			}
			int year = data.start_time.year;
			int month = data.start_time.month;
			int day = data.start_time.day;
			int hour = data.start_time.hour;
			int minute= data.start_time.minute;
			int second = data.start_time.second;
			std::string startTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_HandleMsg", id, data.type, content, data.play_times, data.priority, data.show_seconds, startTime);
		}
		break;
	case NetMsgType::msg_notify_del_broadcast:
		{
			notify_del_broadcast data;
			data.decode(mRecebuff);
			char buf[64] = "";
			_snprintf_s(buf, sizeof(buf), "%d", data.id);
			std::string id(buf);
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_DeleteMsg", data.type, id);
		}
		break;
	default:
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------
template<> BroadCastSystem* Ogre::Singleton<BroadCastSystem>::ms_Singleton = 0;
//-----------------------------------------------------------------------
BroadCastSystem::BroadCastSystem() :
// 定点公告
m_playFlagAT(false),
m_playBeginFlagAT(0),
m_playEndFlagAT(0),
m_stopATByGM(false),
// GM公告
m_playFlagGM(false),
m_playBeginFlagGM(0),
m_playEndFlagGM(0),
// 玩家行为公告
m_playFlagBE(false),
m_playBeginFlagBE(0),
m_playEndFlagBE(0),
m_startTimeBE(0)
{
	m_broadCastGM.setFuncPointer(&BroadCastSystem::_playBeginGM, &BroadCastSystem::_playEndGM);
	m_broadCastAT.setFuncPointer(&BroadCastSystem::_playBeginAT, &BroadCastSystem::_playEndAT);
	m_broadCastBE.setFuncPointer(&BroadCastSystem::_playBeginBE, &BroadCastSystem::_playEndBE);
}
//-----------------------------------------------------------------------
BroadCastSystem::~BroadCastSystem()
{
}
//-----------------------------------------------------------------------
// 定点公告
void BroadCastSystem::saveMsgAT(std::string id, std::string msg, int times)
{
	m_broadCastAT.saveMsg(id, msg, times);
}
//-----------------------------------------------------------------------
void BroadCastSystem::deleteMsgAT(std::string id)
{
	m_broadCastAT.deleteMsg(id);
}
//-----------------------------------------------------------------------
bool BroadCastSystem::isPlayingMsgAT(std::string id)
{
	std::string playingId = m_broadCastAT.getPlayingId();
	return id == playingId;
}
//-----------------------------------------------------------------------
void BroadCastSystem::setCalledFuncNameAT(std::string beginName, std::string name, std::string endName)
{
	m_broadCastAT.setCalledFuncName(beginName, name, endName);
}
//-----------------------------------------------------------------------
void BroadCastSystem::setPlayParamsAT(std::string pMode, int dTime, int rTime, float sStep, int tStep)
{
	m_broadCastAT.setPlayParams(pMode, dTime, rTime, sStep, tStep);
}
//-----------------------------------------------------------------------
void BroadCastSystem::playAT(std::string pare, std::string child)
{
	if (0 == m_broadCastAT.getMsgCount())	// 没有公告消息，返回
		return;
	
	if (false == m_playFlagAT)	// 不在播放则播放
	{
		if (1 == m_broadCastAT.play(pare, child))
			return;
		m_playFlagAT = true;
	}
}
//-----------------------------------------------------------------------
void BroadCastSystem::stopAT(void)
{
	m_broadCastAT.stop();
	m_playFlagAT = false;

	if (0 == m_broadCastAT.getMsgCount())
		return;

	if (false == m_stopATByGM)	// 不是被GM类型公告所停止，保存当前播放的公告的信息（公告id，内容，播放次数），以待下次再次播放
	{
		saveOldMsgAT();
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_SaveNewMsg");
	}
}
//-----------------------------------------------------------------------
void BroadCastSystem::saveOldMsgAT(void)
{
	const Scroll::MsgMap AT_MsgMap = m_broadCastAT.getMsgMap();
	Scroll::MsgMap::const_iterator iter = AT_MsgMap.begin();
	for (; iter!=AT_MsgMap.end(); ++iter)
	{
		const Scroll::Msg AT_Msg = iter->second;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_SaveOldMsg", AT_Msg.id, AT_Msg.content, AT_Msg.playTimes);
	}
	m_broadCastAT.clearUp();
}
//-----------------------------------------------------------------------
int BroadCastSystem::playStateAT(void)
{
	if (false == m_playFlagAT)
	{
		if (0 == m_broadCastAT.getMsgCount())
			return 0;	// 没有消息
		if (true == m_stopATByGM)
			return 2;	// 有消息，停止状态（被GM停止）
	}
	if (0 == m_playBeginFlagAT)
	{
		m_playFlagAT = false;
		return 3;	// 播放状态，处于延时期间
	}
	return 1;	// 播放状态，处于正在滚动期间
}
//-----------------------------------------------------------------------
// GM公告
void BroadCastSystem::saveMsgGM(std::string id, std::string msg, int times)
{
	m_broadCastGM.saveMsg(id, msg, times);
}
//-----------------------------------------------------------------------
void BroadCastSystem::deleteMsgGM(std::string id)
{
	m_broadCastGM.deleteMsg(id);
}
//-----------------------------------------------------------------------
bool BroadCastSystem::isPlayingMsgGM(std::string id)
{
	std::string playingId = m_broadCastGM.getPlayingId();
	return id == playingId;
}
//-----------------------------------------------------------------------
void BroadCastSystem::setCalledFuncNameGM(std::string beginName, std::string name, std::string endName)
{
	m_broadCastGM.setCalledFuncName(beginName, name, endName);
}
//-----------------------------------------------------------------------
void BroadCastSystem::setPlayParamsGM(std::string pMode, int dTime, int rTime, float sStep, int tStep)
{
	m_broadCastGM.setPlayParams(pMode, dTime, rTime, sStep, tStep);
}
//-----------------------------------------------------------------------
void BroadCastSystem::playGM(std::string pare, std::string child)
{
	if (0 == m_broadCastGM.getMsgCount())	// 没有公告消息，返回
		return;

	if (false == m_playFlagGM)	// 不在播放则播放
	{
		if (1 == m_broadCastGM.play(pare, child))
			return;
		m_playFlagGM = true;
		stopAT();	// 播放GM则停止播放定点公告
		m_stopATByGM = true;
	}
}
//-----------------------------------------------------------------------
void BroadCastSystem::stopGM(void)
{
	m_broadCastGM.stop();
	m_broadCastGM.clearUp();
	m_broadCastGM.setPlayFirst(true);
}
//-----------------------------------------------------------------------
// 玩家行为公告
void BroadCastSystem::saveMsgBE(std::string id, std::string msg, int times)
{
	m_broadCastBE.saveMsg(id, msg, times);
}
//-----------------------------------------------------------------------
void BroadCastSystem::setCalledFuncNameBE(std::string beginName, std::string name, std::string endName)
{
	m_broadCastBE.setCalledFuncName(beginName, name, endName);
}
//-----------------------------------------------------------------------
void BroadCastSystem::setPlayParamsBE(std::string pMode, int dTime, int rTime, float sStep, int tStep)
{
	m_broadCastBE.setPlayParams(pMode, dTime, rTime, sStep, tStep);
}
//-----------------------------------------------------------------------
void BroadCastSystem::playBE(std::string pare, std::string child)
{
	if (0 == m_broadCastBE.getMsgCount())	// 没有公告消息，返回
		return;

	if (false == m_playFlagBE)	// 不在播放则播放
	{
		m_broadCastBE.play(pare, child);
		m_playFlagBE = true;
		m_startTimeBE = GetTickCount();
	}
}
//-----------------------------------------------------------------------
void BroadCastSystem::stopBE(void)
{
	m_broadCastBE.stop();
	m_broadCastBE.clearUp();
	m_broadCastBE.setPlayFirst(true);
}
//-----------------------------------------------------------------------
// 更新
void BroadCastSystem::update(void)
{
	// 处理定点公告
	int atFlag = m_broadCastAT.update(m_playBeginFlagAT, m_playEndFlagAT);
	if ((1==atFlag) || (2==atFlag))	// 播放结束
	{
		m_playFlagAT = false;
		if (1 == atFlag)	// 没有消息，结束
		{
			m_broadCastAT.setPlayFirst(true);
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_PlayEnd");
		}
		else if (2 == atFlag)	// 外部停止，结束
		{
			if (false == m_stopATByGM)	// 不是被GM公告停止
				luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_Play");
		}
	}

	// 处理GM公告
	int gmFlag = m_broadCastGM.update(m_playBeginFlagGM, m_playEndFlagGM);
	if ((1==gmFlag) || (2==gmFlag))	// 播放结束
	{
		m_playFlagGM = false;
		m_playFlagAT = false;
		m_stopATByGM = false;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_Play");	// GM播放结束，转去播放定点公告
	}

	// 处理玩家行为公告
	if (true == m_playFlagBE)	// 玩家行为公告文字闪烁效果
	{
		DWORD now = GetTickCount();
		if (now-m_startTimeBE >= 400)	// 每400毫秒闪一次
		{
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_BE_ChangeColor");
			m_startTimeBE = now;
		}
	}
	int beFlag = m_broadCastBE.update(m_playBeginFlagBE, m_playEndFlagBE);
	if ((1==beFlag) || (2==beFlag))	// 播放结束
	{
		m_playFlagBE = false;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_BE_Play");
	}
}
//-----------------------------------------------------------------------
// 请求固定的公告
void BroadCastSystem::reqBroadCast(int type)
{
	req_fixed_broadcast req;
	req.type = type;
	sGameMgr.getEQMsgManager()->Send(req);
}
