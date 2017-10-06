#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "ChatSystem.h"
#include "LuaManager.h"

using namespace luabind;
/**
注册GameObj及其相关用到的类
*/
bool CLuaManager::RegisterChatClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		*/
		luabind::def("getChatSystem",		&ChatSystem::getSingleton),
		/**
		*/
		class_<ChatSystem>("ChatSystem")
			.def(constructor<>())
			.def("talkPickObj",&ChatSystem::talkPickObj)
			.def("requestTalk",&ChatSystem::requestTalk)
			.def("setTalktoAccount",&ChatSystem::setTalktoAccount)
			.def("setTalktoName",&ChatSystem::setTalktoName)
			.def("getTalktoAccount",&ChatSystem::getTalktoAccount)
			.def("getTalktoName",&ChatSystem::getTalktoName)
			/**
			*/
	];

	return true;
}