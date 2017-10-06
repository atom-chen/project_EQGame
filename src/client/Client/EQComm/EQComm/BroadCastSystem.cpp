/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-11-4
* ����: ����ϵͳ
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
			if (3 != data.type)	// �������Ϊ���棬��GBK�����ʽ����Ҫ����ת��ΪUTF8
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
// ���㹫��
m_playFlagAT(false),
m_playBeginFlagAT(0),
m_playEndFlagAT(0),
m_stopATByGM(false),
// GM����
m_playFlagGM(false),
m_playBeginFlagGM(0),
m_playEndFlagGM(0),
// �����Ϊ����
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
// ���㹫��
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
	if (0 == m_broadCastAT.getMsgCount())	// û�й�����Ϣ������
		return;
	
	if (false == m_playFlagAT)	// ���ڲ����򲥷�
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

	if (false == m_stopATByGM)	// ���Ǳ�GM���͹�����ֹͣ�����浱ǰ���ŵĹ������Ϣ������id�����ݣ����Ŵ��������Դ��´��ٴβ���
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
			return 0;	// û����Ϣ
		if (true == m_stopATByGM)
			return 2;	// ����Ϣ��ֹͣ״̬����GMֹͣ��
	}
	if (0 == m_playBeginFlagAT)
	{
		m_playFlagAT = false;
		return 3;	// ����״̬��������ʱ�ڼ�
	}
	return 1;	// ����״̬���������ڹ����ڼ�
}
//-----------------------------------------------------------------------
// GM����
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
	if (0 == m_broadCastGM.getMsgCount())	// û�й�����Ϣ������
		return;

	if (false == m_playFlagGM)	// ���ڲ����򲥷�
	{
		if (1 == m_broadCastGM.play(pare, child))
			return;
		m_playFlagGM = true;
		stopAT();	// ����GM��ֹͣ���Ŷ��㹫��
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
// �����Ϊ����
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
	if (0 == m_broadCastBE.getMsgCount())	// û�й�����Ϣ������
		return;

	if (false == m_playFlagBE)	// ���ڲ����򲥷�
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
// ����
void BroadCastSystem::update(void)
{
	// �����㹫��
	int atFlag = m_broadCastAT.update(m_playBeginFlagAT, m_playEndFlagAT);
	if ((1==atFlag) || (2==atFlag))	// ���Ž���
	{
		m_playFlagAT = false;
		if (1 == atFlag)	// û����Ϣ������
		{
			m_broadCastAT.setPlayFirst(true);
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_PlayEnd");
		}
		else if (2 == atFlag)	// �ⲿֹͣ������
		{
			if (false == m_stopATByGM)	// ���Ǳ�GM����ֹͣ
				luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_Play");
		}
	}

	// ����GM����
	int gmFlag = m_broadCastGM.update(m_playBeginFlagGM, m_playEndFlagGM);
	if ((1==gmFlag) || (2==gmFlag))	// ���Ž���
	{
		m_playFlagGM = false;
		m_playFlagAT = false;
		m_stopATByGM = false;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_AT_Play");	// GM���Ž�����תȥ���Ŷ��㹫��
	}

	// ���������Ϊ����
	if (true == m_playFlagBE)	// �����Ϊ����������˸Ч��
	{
		DWORD now = GetTickCount();
		if (now-m_startTimeBE >= 400)	// ÿ400������һ��
		{
			luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_BE_ChangeColor");
			m_startTimeBE = now;
		}
	}
	int beFlag = m_broadCastBE.update(m_playBeginFlagBE, m_playEndFlagBE);
	if ((1==beFlag) || (2==beFlag))	// ���Ž���
	{
		m_playFlagBE = false;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_BE_Play");
	}
}
//-----------------------------------------------------------------------
// ����̶��Ĺ���
void BroadCastSystem::reqBroadCast(int type)
{
	req_fixed_broadcast req;
	req.type = type;
	sGameMgr.getEQMsgManager()->Send(req);
}
