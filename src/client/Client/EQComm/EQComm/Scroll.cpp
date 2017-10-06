/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-12-27
* ����: ʵ�ִ��ڵĹ���
******************************************************************************/
#include "EQCommPCH.h"

#include "Scroll.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "CEGUI.h"
#include "UIAnimationMgr.h"


//-----------------------------------------------------------------------
//	���캯��
Scroll::Scroll() :
m_playingIndex(0),
m_playingMsg(NULL),
m_playingId(""),
m_pareWinname(""),
m_childWinname(""),
m_playBeginFuncName(""),
m_playFuncName(""),
m_playEndFuncName(""),
m_beginFunc(NULL),
m_endFunc(NULL),
m_spaceStep(1),
m_timeStep(1),
m_scrollMode(SM_NONE),
m_delayTime(0),
m_replayTime(0),
m_replayFlag(false),
m_playStop(false),
m_playFirst(true)
{
}
//-----------------------------------------------------------------------
//	��������
Scroll::~Scroll()
{
	clearUp();
}
//-----------------------------------------------------------------------
//	������Ϣ
void Scroll::saveMsg(std::string id, std::string msg, int times)
{
	// ��Ϣ�Ѵ���
	if (m_msgMap.end() != m_msgMap.find(id))
	{
		return;
	}
	// ������Ϣ
	Msg row;
	row.id = id;
	row.content = msg;
	row.playTimes = times;
	m_msgMap.insert(std::make_pair(id, row));
	m_msgIdVec.push_back(id);
}
//-----------------------------------------------------------------------
//	��ȡ��Ϣ
Scroll::Msg* Scroll::getMsg(std::string id)
{
	// û����Ϣ
	if (m_msgIdVec.empty())
	{
		return NULL;
	}
	// ��ȡ��Ϣ
	MsgMap::iterator iter = m_msgMap.find(id);
	if (m_msgMap.end() == iter)
	{
		return NULL;
	}
	return &(iter->second);
}
//-----------------------------------------------------------------------
//	��ȡ���ڲ��ŵ���Ϣ
std::string Scroll::getPlayingId(void)
{
	return m_playingId;
}
//-----------------------------------------------------------------------
//	��ȡ��Ϣ����
const Scroll::MsgMap& Scroll::getMsgMap(void) const
{
	return m_msgMap;
}
//-----------------------------------------------------------------------
//	��ȡ��Ϣ����
int Scroll::getMsgCount(void)
{
	return m_msgMap.size();
}
//-----------------------------------------------------------------------
//	ɾ��ĳ����Ϣ
void Scroll::deleteMsg(std::string id)
{
	// û����Ϣ
	if (m_msgMap.empty())
	{
		return;
	}
	// ����Ϣ����ɾ����Ϣ
	MsgMap::const_iterator iter = m_msgMap.find(id);
	if (m_msgMap.end() == iter)
	{
		return;
	}
	m_msgMap.erase(iter);
	// ����Ϣid������ɾ����Ϣid
	MsgIdVec::const_iterator it = m_msgIdVec.begin();
	for (int playId=0; m_msgIdVec.end()!=it; ++it,++playId)
	{
		if (id == *it)
		{
			m_msgIdVec.erase(it);
			if (m_playingIndex > playId)
			{
				--m_playingIndex;
			}
			return;
		}
	}
}
//-----------------------------------------------------------------------
//	�����Ϣ
void Scroll::clearUp(void)
{
	m_msgMap.clear();
	m_msgIdVec.clear();
	m_playingIndex = 0;
	m_playFirst = true;
	m_playingMsg = NULL;
	m_playingId = "";
}
//-----------------------------------------------------------------------
//	��ȡ��Ϣ��id
std::string Scroll::_getMsgId(size_t id)
{
	if (m_msgIdVec.empty())
	{
		return "-1";
	}
	if ((id<0) || (id>=m_msgIdVec.size()))
	{
		return "-2";
	}
	return m_msgIdVec[id];
}
//-----------------------------------------------------------------------
//	���ŵ�����Ϣ
void Scroll::_playMsg(std::string winname, float pareW, float pareH, float childW, float childH, ScrollMode scrollM)
{
	int delayTime, playT;
	int xStart, yStart, xEnd, yEnd;

	if (m_replayFlag)
	{
		delayTime = m_replayTime;
		m_replayFlag = false;
	}
	else
	{
		if (true == m_playFirst)
		{
			delayTime = 0;
			m_playFirst = false;
		}
		else
		{
			delayTime = m_delayTime;
		}
	}

	switch (scrollM)
	{
	case SM_NONE:		//	������
		{
			xStart = 0;
			yStart = 0;
			xEnd = 0;
			yEnd = 0;
			playT = m_timeStep;
		}
		break;
	case SM_UP_DOWN:	//	���ϵ��¹���
		{
			xStart = 0;
			yStart = -childH;
			xEnd = 0;
			yEnd = pareH;
			playT = ((pareH + childH) / m_spaceStep) * m_timeStep;
		}
		break;
	case SM_DOWN_UP:	//	���µ��Ϲ���
		{
			xStart = 0;
			yStart = pareH;
			xEnd = 0;
			yEnd = -childH;
			playT = ((pareH + childH) / m_spaceStep) * m_timeStep;
		}
		break;
	case SM_LEFT_RIGHT:	//	�����ҹ���
		{
			xStart = -childW;
			yStart = 0;
			xEnd = pareW;
			yEnd = 0;
			playT = ((pareW + childW) / m_spaceStep) * m_timeStep;
		}
		break;
	case SM_RIGHT_LEFT:	//	���ҵ������
		{
			xStart = pareW;
			yStart = 0;
			xEnd = -childW;
			yEnd = 0;
			playT = ((pareW + childW) / m_spaceStep) * m_timeStep;
		}
		break;
	}

	_playScroll(winname, delayTime, playT, xStart, yStart, xEnd, yEnd);
}
//-----------------------------------------------------------------------
//	��������
void Scroll::_playScroll(std::string winname, int dT, int pT, int xS, int yS, int xE, int yE)
{
	UIAnimationInfo info;
	info.mType = UIAnimationInfo::EUIA_MOVE;
	info.mDelayTime = dT;
	info.mPlayTime = pT;
	info.mStartOffsetX = xS;
	info.mStartOffsetY = yS;
	info.mEndOffsetX = xE;
	info.mEndOffsetY = yE;
	info.subscribeEvent(UIAnimationInfo::EventPlayBegin, m_beginFunc);
	info.subscribeEvent(UIAnimationInfo::EventPlayEnd, m_endFunc);
	UIAnimationMgr::getSingleton().playAnimation(winname, info);
}
//-----------------------------------------------------------------------
//	����������Ϣ
int Scroll::play(std::string pareName, std::string childName)
{
	m_playStop = false;
	m_pareWinname = pareName;
	m_childWinname = childName;

	std::string msgId = _getMsgId(m_playingIndex);
	if ("-1" == msgId)	//	û��Ҫ���ŵ�����
	{
		return 1;
	}
	else if ("-2" == msgId)	//	msgId������Χ
	{
		m_playingIndex = 0;
		m_replayFlag = true;
		msgId = _getMsgId(m_playingIndex);
	}
	m_playingMsg = getMsg(msgId);
	if (NULL == m_playingMsg)
	{
		return 1;
	}
	m_playingId = m_playingMsg->id;
	luabind::call_function<void>(sLuaMgr.getL(), m_playFuncName.c_str(), m_playingMsg->content);

	CEGUI::Window *pareWin = CEGUI::WindowManager::getSingleton().getWindow(m_pareWinname);
	float pareWidth = pareWin->getWidth().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getWidth());
	float pareHeight = pareWin->getHeight().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getHeight());
	CEGUI::Window *childWin = CEGUI::WindowManager::getSingleton().getWindow(m_childWinname);
	float childWidth = childWin->getWidth().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getWidth());
	float childHeight = childWin->getHeight().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getHeight());
	_playMsg(m_childWinname, pareWidth, pareHeight, childWidth, childHeight, m_scrollMode);
	return 0;
}
//-----------------------------------------------------------------------
//	ֹͣ����
void Scroll::stop(void)
{
	m_playStop = true;
	UIAnimationMgr::getSingleton().stopAnimation(m_childWinname);
}
//-----------------------------------------------------------------------
//	����
int Scroll::update(int& begin, int& end)
{
	// �������ſ�ʼ�¼�
	if (1 == begin)
	{
		luabind::call_function<void>(sLuaMgr.getL(), m_playBeginFuncName.c_str());
		begin = 0;
		// ����ǰ���ŵ���Ϣ
		if (NULL != m_playingMsg)
		{
			--m_playingMsg->playTimes;			// ��ǰ���ŵ���Ϣ������1
			++m_playingIndex;
			if (0 == m_playingMsg->playTimes)	// ��ǰ���ŵ���Ϣ�����ѵ�
			{
				deleteMsg(m_playingMsg->id);	// ɾ������Ϣ
			}
		}
	}
	// �������Ž����¼�
	if(1 == end)
	{
		luabind::call_function<void>(sLuaMgr.getL(), m_playEndFuncName.c_str());
		end = 0;
		m_playingMsg = NULL;
		m_playingId = "";
		// û����Ϣ����������
		if (0 == getMsgCount())
		{
			m_playingIndex = 0;
			m_playFirst = true;
			return 1;
		}
		// �ⲿֹͣ����������
		if (true == m_playStop)
		{
			m_playFirst = true;
			return 2;
		}
		play(m_pareWinname, m_childWinname);
	}
	return 0;
}
//-----------------------------------------------------------------------
//	���ã����ſ�ʼ�����š����Ž���������ʱ�̵��õ�Lua������
void Scroll::setCalledFuncName(std::string beginName, std::string name, std::string endName)
{
	m_playBeginFuncName = beginName;
	m_playFuncName = name;
	m_playEndFuncName = endName;
}
//-----------------------------------------------------------------------
//	���ò��ŵĲ���
int Scroll::setPlayParams(std::string pMode, int dTime, int rTime, float sStep, int tStep)
{
	if ("NONE" == pMode)
		m_scrollMode = SM_NONE;
	else if ("UP_DOWN" == pMode)
		m_scrollMode = SM_UP_DOWN;
	else if ("DOWN_UP" == pMode)
		m_scrollMode = SM_DOWN_UP;
	else if ("LEFT_RIGHT" == pMode)
		m_scrollMode = SM_LEFT_RIGHT;
	else if ("RIGHT_LEFT" == pMode)
		m_scrollMode = SM_RIGHT_LEFT;
	else
		return -1;

	if ((0>sStep) || (0>=tStep))
		return -2;
		
	m_delayTime = dTime;
	m_replayTime = rTime;
	m_spaceStep = sStep;
	m_timeStep = tStep;
	return 0;
}
//-----------------------------------------------------------------------
//	������Ӧ��ʼ���źͽ������ŵĺ���ָ��
void Scroll::setFuncPointer(EVENT_HANDLE begin, EVENT_HANDLE end)
{
	m_beginFunc = begin;
	m_endFunc = end;
}
//-----------------------------------------------------------------------
//	�����ǵ�һ�β�����Ϣ������ĵ�һ����Ϣ
void Scroll::setPlayFirst(bool first)
{
	m_playFirst = first;
}
