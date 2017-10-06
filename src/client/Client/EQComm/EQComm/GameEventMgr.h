/*********************************************************************
* 作者: 麦_Mike
* 时间: 20010-2-22
* 描述: 游戏事件管理器
**********************************************************************/
#ifndef GAMEEVENTMGR_H
#define GAMEEVENTMGR_H

#include <map>
#include <string>
#include <Ogre.h>
#include "GameEventDef.h"
#include "GameEventArgs.h"


typedef int (*EVENT_HANDLE)(void*);

#define GAME_EVENT_FUNS std::vector<std::string>
#define GAME_EVENT_FUNS_ITOR std::vector<std::string>::iterator

#define GAME_EVENT_FUNPOINTER std::vector<EVENT_HANDLE>
#define GAME_EVENT_FUNPOINTER_ITOR std::vector<EVENT_HANDLE>::iterator


class GameEvent
{
public:
	GameEvent(std::string name){ mName = name; }
	virtual ~GameEvent(){}
	
	std::string& getName(){ return mName; }
	
	/**
	@brief 签订事件
	@param fn 脚本函数名称
	*/
	void subscribeEvent(std::string fn);
	void subscribeEvent(EVENT_HANDLE eh);

	/**
	@brief 撤消签订事件
	@param fn 脚本函数名称
	*/
	void unsubscribeEvent(std::string fn);
	void unsubscribeEvent(EVENT_HANDLE eh);

	/**
	@brief 执行事件触发
	*/
	void fireEvent(void* args=NULL);

private:
	std::string mName;				//事件名称
	GAME_EVENT_FUNS mScriptFuns;	//事件签订的脚本函数列表
	GAME_EVENT_FUNPOINTER mFunPointer;	//事件签订的函数指针列表
};

/**
*/
#define GAME_EVENT_MAP std::map<std::string, GameEvent*>
class GameEventMgr : public Ogre::Singleton<GameEventMgr>
{
public:
	GameEventMgr(void){}
	virtual ~GameEventMgr(void){}
	
	/**
	@brief 注册游戏事件
	@param evtname 事件名称
	*/
	void regEvent(std::string evtName);

	/**
	@brief 签订事件
	@param fn 脚本函数名称
	*/
	void subscribeEvent(std::string evt, std::string scriptfn);
	void subscribeEvent(std::string evt, EVENT_HANDLE eh);
	/**
	@brief 撤消签订事件
	@param fn 脚本函数名称
	*/
	void unsubscribeEvent(std::string evt, std::string fn);
	void unsubscribeEvent(std::string evt, EVENT_HANDLE eh);
	/**
	@brief 执行事件触发
	*/
	void fireEvent(std::string evtname/*,void* args=NULL*/);

	//void pushEvent(std::string evtname,void*arg=NULL);
	/**
	@brief 注册所有游戏事件
	*/
	void regAllEvent();

	/*
	*/
	void cleanup();

private:
	GAME_EVENT_MAP mEventset;	//游戏事件列表
};
#define sEventMgr GameEventMgr::getSingleton()

class GameEventSet
{
public:
	GameEventSet(){};
	virtual ~GameEventSet();

	void subscribeEvent(std::string evt, EVENT_HANDLE eh);
	void subscribeEvent(std::string evt, std::string scriptfn);
	void unsubscribeEvent(std::string evt, EVENT_HANDLE eh);
	void unsubscribeEvent(std::string evt, std::string fn);
	bool fireEvent(std::string evt);

protected:
	std::map<std::string, GameEvent> mEventset;	//游戏事件列表
};

#endif
