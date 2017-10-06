/******************************************************************************
* 作者: 何展然
* 时间: 2010-12-27
* 描述: 实现窗口的滚动，作为父类使用
******************************************************************************/
#ifndef Scroll_H
#define Scroll_H


#include "EQMainPrerequisites.h"
#include "GameEventMgr.h"


class Scroll
{
public:
	//	滚动模式
	enum ScrollMode
	{
		SM_NONE,		//	不滚动
		SM_UP_DOWN,		//	从上到下滚动
		SM_DOWN_UP,		//	从下到上滚动
		SM_LEFT_RIGHT,	//	从左到右滚动
		SM_RIGHT_LEFT	//	从右到左滚动
	};

	//	消息
	struct Msg
	{
		Msg()
			: id("")
			, content("")
			, playTimes(0)
		{
		}

		std::string id;	// 消息id
		std::string content;//	消息内容
		int playTimes;		//	播放次数
	};

	typedef std::map<std::string, Msg> MsgMap;	//	消息集
	typedef std::vector<std::string> MsgIdVec;	//	消息id集

public:
	/**
	*	构造函数
	*/
	Scroll();

	/**
	*	析构函数
	*/
	virtual ~Scroll();

	/**
	*	功	能：保存消息
	*	参	数：id: 消息id， msg: 消息， times: 播放次数
	*	返回值：void
	*/
	void saveMsg(std::string id, std::string msg, int times);

	/**
	*	功	能：获取消息
	*	参	数：id: 消息id
	*	返回值：Msg*，不加const
	*/
	Msg* getMsg(std::string id);

	/**
	*	功	能：获取正在播放的消息的id
	*	参	数：void
	*	返回值：std::string
	*/
	std::string getPlayingId(void);

	/**
	*	功	能：获取消息集合
	*	参	数：void
	*	返回值：const Msg&
	*/
	const MsgMap& getMsgMap(void) const;

	/**
	*	功	能：获取消息数量
	*	参	数：void
	*	返回值：int
	*/
	int getMsgCount(void);

	/**
	*	功	能：删除消息
	*	参	数：id: 消息id
	*	返回值：void
	*/
	void deleteMsg(std::string id);

	/**
	*	功	能：清空消息
	*	参	数：void
	*	返回值：void
	*/
	void clearUp(void);

	/**
	*	功	能：播放消息
	*	参	数：pareName: 静止背景窗口名， childName: 滚动窗口名
	*	返回值：0（有内容播放），1（没内容播放）
	*/
	int play(std::string pareName, std::string childName);

	/**
	*	功	能：停止播放
	*	参	数：void
	*	返回值：void
	*/
	void stop(void);

	/**
	*	功	能：更新
	*	参	数：begin: 播放开始标志， end: 播放结束标志
	*	返回值：0，1（没有消息，播放完毕），2（外部停止，播放完毕）
	*/
	int update(int& begin, int& end);

	/**
	*	功	能：设置（播放开始、播放、播放结束）三个时刻调用的Lua函数名
	*	参	数：beginName: 开始时刻函数名， name: 播放时刻函数名， endName: 结束时刻函数名
	*	返回值：void
	*/
	void setCalledFuncName(std::string beginName, std::string name, std::string endName);

	/**
	*	功	能：设置播放的参数
	*	参	数：pMode: 播放模式，不滚动（"NONE"），上到下（"UP_DOWN"），下到上（"DOWN_UP"），左到右（"LEFT_RIGHT"），右到左（"RIGHT_LEFT"）
	*			dTime: 延迟时间， rTime: 重新播放的延迟时间， sStep: 距离步进， tStep: 时间步进
	*	返回值：-1（pMode参数错误），-2（sStep小于0或tStep小于等于0）
	*/
	int setPlayParams(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);

	/**
	*	功	能：设置响应开始播放和结束播放的函数指针
	*	参	数：begin: 开始播放响应函数， end: 结束播放响应函数
	*	返回值：void
	*/
	void setFuncPointer(EVENT_HANDLE begin, EVENT_HANDLE end);

	/**
	*	功	能：设置是第一次播放消息集合里的第一条消息
	*	参	数：first: 是
	*	返回值：void
	*/
	void setPlayFirst(bool first=true);

private:
	/**
	*	功	能：获取消息id集里的消息id
	*	参	数：id: 消息id
	*	返回值：std::string
	*/
	std::string _getMsgId(size_t id);

	/**
	*	功	能：播放单条消息
	*	参	数：winname: 窗口名称
	*			pareW: 父窗口宽度， pareH: 父窗口高度， childW: 子窗口宽度， childH: 子窗口高度， scrollM: 滚动模式
	*	返回值：void
	*/
	void _playMsg(std::string winname, float pareW, float pareH, float childW, float childH, ScrollMode scrollM);

	/**
	*	功	能：播放滚动效果
	*	参	数：winname: 窗口名， dT: 延迟时间， pT: 播放时间，
	*			xS: x开始坐标， yS: y开始坐标， xE: x结束坐标， yE: y结束坐标
	*	返回值：void
	*/
	void _playScroll(std::string winname, int dT, int pT, int xS, int yS, int xE, int yE);

private:
	MsgMap m_msgMap;		//	消息
	MsgIdVec m_msgIdVec;	//	消息id集合
	int m_playingIndex;		//	当前播放的消息在集合中的索引
	Msg* m_playingMsg;		//	当前播放的消息
	std::string m_playingId;//	当前播放的消息的id

	std::string m_pareWinname;		//	静止背景窗口
	std::string m_childWinname;		//	滚动窗口

	std::string m_playBeginFuncName;//	播放开始时刻调用的Lua函数名
	std::string m_playFuncName;		//	播放时刻调用的Lua函数名
	std::string m_playEndFuncName;	//	播放结束时刻调用的Lua函数名

	EVENT_HANDLE m_beginFunc;	//	开始播放响应函数
	EVENT_HANDLE m_endFunc;		//	结束播放响应函数

	float m_spaceStep;	//	滚动距离步进
	int m_timeStep;		//	滚动时间步进

	ScrollMode m_scrollMode;		//	滚动模式
	int m_delayTime;	//	延迟时间
	int m_replayTime;	//	重新播放时间间隔
	bool m_replayFlag;	//	重新播放标识
	bool m_playStop;	//	当前播放消息被停止标识
	bool m_playFirst;	//	第一遍播放消息集合里的第一条消息

};	//	end of class Scroll


#endif	//	end of guard Scroll_H
