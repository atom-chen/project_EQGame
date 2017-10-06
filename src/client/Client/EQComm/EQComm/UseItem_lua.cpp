#include "EQCommPCH.h"

#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "ItemMgr.h"

using namespace luabind;
bool CLuaManager::RegisterUseItemFun()
{
	luabind::module(EQ_GLOBA_CLASS)
		[
			luabind::def("getItemMgr",		&ItemMgr::getSingleton),

			class_<ItemMgr>("ItemMgr")
			.def("openUI",			&ItemMgr::openUI)
			.def("print",			&ItemMgr::print)
			.def("playAnimation",	&ItemMgr::playAnimation)
		];

	return true;
}