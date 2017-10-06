#include "EQCommPCH.h"
#include "GameEventMgr.h"
#include "CEGUI.h"
#include "LuaManager.h"

template<> GameEventMgr* Ogre::Singleton<GameEventMgr>::ms_Singleton = 0;
//--------------------------------------------------------------------------
void GameEvent::subscribeEvent(std::string fn)
{
	unsubscribeEvent(fn);
	mScriptFuns.push_back(fn);
}
//--------------------------------------------------------------------------
void GameEvent::subscribeEvent(EVENT_HANDLE eh)
{
	//unsubscribeEvent(eh);
	mFunPointer.push_back(eh);
}
//--------------------------------------------------------------------------
void GameEvent::unsubscribeEvent(std::string fn)
{
	GAME_EVENT_FUNS_ITOR it;
	it = find(mScriptFuns.begin(),mScriptFuns.end(),fn);
	if(it != mScriptFuns.end())
	{
		mScriptFuns.erase(it);
	}
}
//--------------------------------------------------------------------------
void GameEvent::unsubscribeEvent(EVENT_HANDLE eh)
{
	GAME_EVENT_FUNPOINTER_ITOR it;
	it = find(mFunPointer.begin(),mFunPointer.end(),eh);
	if(it != mFunPointer.end())
	{
		mFunPointer.erase(it);
	}
}
//--------------------------------------------------------------------------
void GameEvent::fireEvent(void* args)
{
	std::string scriptfun;
	GAME_EVENT_FUNS_ITOR it = mScriptFuns.begin();
	for (;it != mScriptFuns.end();it++)
	{
		scriptfun = (std::string)(*it);
		CEGUI::System::getSingleton().executeScriptGlobal(scriptfun); 
		//CLuaManager::getSingleton().CallFunction(scriptfun,args);
	}

	GAME_EVENT_FUNPOINTER_ITOR itor = mFunPointer.begin();
	for (;itor != mFunPointer.end();itor++)
	{
		(*itor)(args);
	}
}
//--------------------------------------------------------------------------
void GameEventMgr::regAllEvent()
{
	for(int i=0; i < GE_COUNT; i++)
		regEvent(game_event[i]);
}
//--------------------------------------------------------------------------
void GameEventMgr::regEvent(std::string evtName)
{
	assert(mEventset.find(evtName) == mEventset.end());
	GameEvent* ge = new GameEvent(evtName);
	mEventset.insert(std::make_pair(evtName,ge));
}
//--------------------------------------------------------------------------
void GameEventMgr::subscribeEvent(std::string evt, std::string scriptfn)
{
	assert(mEventset.find(evt) != mEventset.end());
	mEventset[evt]->subscribeEvent(scriptfn);
}
//--------------------------------------------------------------------------
void GameEventMgr::subscribeEvent(std::string evt, EVENT_HANDLE eh)
{
	assert(mEventset.find(evt) != mEventset.end());
	mEventset[evt]->subscribeEvent(eh);
}
//--------------------------------------------------------------------------
void GameEventMgr::unsubscribeEvent(std::string evt, std::string scriptfn)
{
	assert(mEventset.find(evt) != mEventset.end());
	mEventset[evt]->unsubscribeEvent(scriptfn);
}
//--------------------------------------------------------------------------
void GameEventMgr::unsubscribeEvent(std::string evt, EVENT_HANDLE eh)
{
	assert(mEventset.find(evt) != mEventset.end());
	mEventset[evt]->unsubscribeEvent(eh);
}
//--------------------------------------------------------------------------
void GameEventMgr::fireEvent(std::string evtname/*,void* args*/)
{
	assert(mEventset.find(evtname) != mEventset.end());
	mEventset[evtname]->fireEvent(/*args*/);
}
//--------------------------------------------------------------------------
void GameEventMgr::cleanup()
{
	GAME_EVENT_MAP::iterator itor = mEventset.begin();
	for(; itor != mEventset.end(); itor++)
	{
		delete((*itor).second);
		(*itor).second = NULL;
	}
	mEventset.clear();
}




//--------------------------------------------------------------------------
GameEventSet::~GameEventSet()
{

};
//--------------------------------------------------------------------------
void GameEventSet::subscribeEvent(std::string evt, EVENT_HANDLE eh)
{
	if(mEventset.find(evt) == mEventset.end())
	{
		GameEvent ge(evt);
		mEventset.insert(std::make_pair(evt,ge));
	}

	std::map<std::string, GameEvent>::iterator it = mEventset.find(evt);
	assert(it != mEventset.end());
	it->second.subscribeEvent(eh);
}
//--------------------------------------------------------------------------
void GameEventSet::subscribeEvent(std::string evt, std::string scriptfn)
{
	if(mEventset.find(evt) == mEventset.end())
	{
		GameEvent ge(evt);
		mEventset.insert(std::make_pair(evt,ge));
	}

	std::map<std::string, GameEvent>::iterator it = mEventset.find(evt);
	assert(it != mEventset.end());
	it->second.subscribeEvent(scriptfn);
}
//--------------------------------------------------------------------------
void GameEventSet::unsubscribeEvent(std::string evt, EVENT_HANDLE eh)
{
	std::map<std::string, GameEvent>::iterator it = mEventset.find(evt);
	assert(it != mEventset.end());
	it->second.unsubscribeEvent(eh);
}
//--------------------------------------------------------------------------
void GameEventSet::unsubscribeEvent(std::string evt, std::string fn)
{
	std::map<std::string, GameEvent>::iterator it = mEventset.find(evt);
	assert(it != mEventset.end());
	it->second.unsubscribeEvent(fn);
}
//--------------------------------------------------------------------------
bool GameEventSet::fireEvent(std::string evt/*,void* args*/)
{
	std::map<std::string, GameEvent>::iterator it = mEventset.find(evt);
	if(it != mEventset.end())
	{
		it->second.fireEvent();
		return true;
	}
	else
		return false;
}