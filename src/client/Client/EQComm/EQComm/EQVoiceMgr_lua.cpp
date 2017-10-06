#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"
#include "EQVoiceMgr.h"

using namespace luabind;

bool CLuaManager::RegisterVoiceMgrClass()
{
	module( EQ_GLOBA_CLASS )
		[
			luabind::def("getVoiceMgr",&EQVoiceMgr::getSingleton)
			,
			class_<EQVoiceMgr>("EQVoiceMgr")
			.def("play",(bool (EQVoiceMgr::*)(uint, bool))&EQVoiceMgr::play)
			.def("stop",(void (EQVoiceMgr::*)(uint))&EQVoiceMgr::stop)
			.def("play",(bool (EQVoiceMgr::*)(const char*, bool))&EQVoiceMgr::play)
			.def("stop",(void (EQVoiceMgr::*)(const char*))&EQVoiceMgr::stop)
			.def("stopACls", &EQVoiceMgr::stopACls)
			.def("stopAll",&EQVoiceMgr::stopAll)
			.def("setVolume",&EQVoiceMgr::setVolume)
			.def("setAllVolume",&EQVoiceMgr::setAllVolume)
			.def("getVolume",&EQVoiceMgr::getVolume)
		];
	
	return true;
}
