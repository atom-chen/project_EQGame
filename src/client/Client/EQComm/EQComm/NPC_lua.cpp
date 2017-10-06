#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "NPC.h"
#include "LuaManager.h"
#include "NPCTable.h"

using namespace luabind;
/**
注册GameObj及其相关用到的类
*/
bool CLuaManager::RegisterNPCClass()
{
	module( EQ_GLOBA_CLASS )
		[
			/**
			*/
			//luabind::def("getChatSystem",		&ChatSystem::getSingleton),
			/**
			*/
			class_<NPC,GameObj>("NPC")
				.def(constructor<>())
				.def("getOptCmd",&NPC::getOptCmd)
				.def("getMsg",&NPC::getMsg)
				.def("reqNPCommand",&NPC::reqNPCommand)
				.def("getOptTpltID",&NPC::getOptTpltID)
				.def("setText",&NPC::setText)
				.def("setClickCmdIdx",&NPC::setClickCmdIdx)
				.def("reqNPCommandBymsg",&NPC::reqNPCommandBymsg)
				.def("reqNPCommandByOpt",&NPC::reqNPCommandByOpt)
				
				
				.def_readwrite("mOptCmdSize",&NPC::mOptCmdSize)
			,
			/**
			*/
			class_<NPCTableMgr>("NPCTableMgr")
				.def(constructor<>())
				.def("getNPCMsgRow",&NPCTableMgr::getNPCMsgRow)
			,

			luabind::def("getNPCTable",		&NPCTableMgr::getSingleton)
			
		];

	return true;
}