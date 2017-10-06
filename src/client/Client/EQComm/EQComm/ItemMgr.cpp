#include "EQCommPCH.h"
#include "ItemMgr.h"

#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "MsgSystem.h"
#include "MainPlayer.h"
#include "BodyActionMgr.h"

//--------------------------------------------------------------------
template<> ItemMgr* Ogre::Singleton<ItemMgr>::ms_Singleton = 0;
//--------------------------------------------------------------------
ItemMgr::ItemMgr(void)
{
}
//-----------------------------------------------------------------------
ItemMgr::~ItemMgr(void)
{
}
//-----------------------------------------------------------------------
void ItemMgr::parseStr(std::string& str)
{
	if(str == "#T")
		str = MainPlayer::getSingleton().mTargetName;
}
//-----------------------------------------------------------------------
void ItemMgr::openUI(std::string win)
{
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "UIM_OpenWindow", win);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "UIM_DoOpenAndCloseUI", win);
}
//-----------------------------------------------------------------------
void ItemMgr::print(std::string msg)
{
	MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, msg);
}
//-----------------------------------------------------------------------
void ItemMgr::playAnimation(std::string ani, std::string target)
{
	parseStr(target);

	sMainPlayer.getBodyActionMgr()->reqStartBodyAction(ani,"Stand", target,false);
}
//-----------------------------------------------------------------------
