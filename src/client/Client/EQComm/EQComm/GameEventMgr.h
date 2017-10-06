/*********************************************************************
* ����: ��_Mike
* ʱ��: 20010-2-22
* ����: ��Ϸ�¼�������
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
	@brief ǩ���¼�
	@param fn �ű���������
	*/
	void subscribeEvent(std::string fn);
	void subscribeEvent(EVENT_HANDLE eh);

	/**
	@brief ����ǩ���¼�
	@param fn �ű���������
	*/
	void unsubscribeEvent(std::string fn);
	void unsubscribeEvent(EVENT_HANDLE eh);

	/**
	@brief ִ���¼�����
	*/
	void fireEvent(void* args=NULL);

private:
	std::string mName;				//�¼�����
	GAME_EVENT_FUNS mScriptFuns;	//�¼�ǩ���Ľű������б�
	GAME_EVENT_FUNPOINTER mFunPointer;	//�¼�ǩ���ĺ���ָ���б�
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
	@brief ע����Ϸ�¼�
	@param evtname �¼�����
	*/
	void regEvent(std::string evtName);

	/**
	@brief ǩ���¼�
	@param fn �ű���������
	*/
	void subscribeEvent(std::string evt, std::string scriptfn);
	void subscribeEvent(std::string evt, EVENT_HANDLE eh);
	/**
	@brief ����ǩ���¼�
	@param fn �ű���������
	*/
	void unsubscribeEvent(std::string evt, std::string fn);
	void unsubscribeEvent(std::string evt, EVENT_HANDLE eh);
	/**
	@brief ִ���¼�����
	*/
	void fireEvent(std::string evtname/*,void* args=NULL*/);

	//void pushEvent(std::string evtname,void*arg=NULL);
	/**
	@brief ע��������Ϸ�¼�
	*/
	void regAllEvent();

	/*
	*/
	void cleanup();

private:
	GAME_EVENT_MAP mEventset;	//��Ϸ�¼��б�
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
	std::map<std::string, GameEvent> mEventset;	//��Ϸ�¼��б�
};

#endif
