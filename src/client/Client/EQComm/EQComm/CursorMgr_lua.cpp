#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "CursorMgr.h"
#include "LuaManager.h"

using namespace luabind;
/**
注册GameObj及其相关用到的类
*/
bool CLuaManager::RegisterCursorMgrClass()
{
	module( EQ_GLOBA_CLASS )
		[
			/**
			*/
			luabind::def("getCursorMgr",		&CursorMgr::getSingleton),
			/**
			*/
			class_<CursorMgr>("CursorMgr")
				.enum_("CursorState")
				[
					value("CS_NORMAL",CursorMgr::CS_NORMAL),
					value("CS_LBDRAG_SCENE",CursorMgr::CS_LBDRAG_SCENE),
					value("CS_FARM_SCENE",CursorMgr::CS_FARM_SCENE),
					value("CS_SPLIT_ITEM",CursorMgr::CS_SPLIT_ITEM),
					value("CS_HYPERLINK",CursorMgr::CS_HYPERLINK),
					value("CS_CHANGE_FLOOR",CursorMgr::CS_CHANGE_FLOOR),
					value("CS_CHANGE_WALL",CursorMgr::CS_CHANGE_WALL)
				]
				.def("setState",&CursorMgr::setState)
				.def("getState",&CursorMgr::getState)
				.def("backtoLastState",&CursorMgr::backtoLastState)
				.def("setSpliting",&CursorMgr::setSpliting)
				.def("getSpliting",&CursorMgr::getSpliting)
				
			/**
			*/
		];

	return true;
}