#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "PlayerHouse.h"
#include "EQGameSceneMgr.h"

#include "LuaManager.h"
#include "NetPacket.h"

using namespace luabind;

/**
注册GameItem及其相关用到的类
*/
bool CLuaManager::RegisterPlayerHouseClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		 */
		luabind::def("getEQSceneMgr",		&EQGameSceneMgr::getSingleton),

		class_<PlayerHouse>("PlayerHouse")
			.def("setCurUsingBagIdx",&PlayerHouse::setCurUsingBagIdx)
			.def("getCurUsingBagIdx",&PlayerHouse::getCurUsingBagIdx)
			.def("setTempMark",&PlayerHouse::setTempMark)
			.def("showAllTempMark",&PlayerHouse::showAllTempMark)
			.def("hideAllTempMark",&PlayerHouse::hideAllTempMark)
			.def("getLevel",&PlayerHouse::getLevel)
			.def("changeRoomMaterial",&PlayerHouse::changeRoomMaterial)
			.def("getRoomMaterial",&PlayerHouse::getRoomMaterial)
			.def("isPreview",&PlayerHouse::isPreview)
		,

		class_<EQGameSceneMgr>("EQGameSceneMgr")
			.enum_("eSceneType")
			[
				value("SCENE_NONE",EQGameSceneMgr::SCENE_NONE),
				value("SCENE_HOUSE",EQGameSceneMgr::SCENE_HOUSE),
				value("SCENE_HOUSE_PREVIEW",EQGameSceneMgr::SCENE_HOUSE_PREVIEW),
				value("SCENE_COMMON",EQGameSceneMgr::SCENE_COMMON),
				value("SCENE_FARM",EQGameSceneMgr::SCENE_FARM)
			]
			.def("reqChangeScene",&EQGameSceneMgr::reqChangeScene)
			.def("reqChangeSceneByGuide",&EQGameSceneMgr::reqChangeSceneByGuide)
			.def("reqCommSeceneState",&EQGameSceneMgr::reqCommSeceneState)
			.def("reqTeleport",&EQGameSceneMgr::reqTeleport)
			.def("reqReseeScene",&EQGameSceneMgr::reqReseeScene)
			.def("reqLeaveFarm",&EQGameSceneMgr::reqLeaveFarm)
			.def("getSceneType",&EQGameSceneMgr::getSceneType)
			.def("getHouse",&EQGameSceneMgr::getHouse)
			.def("inMyHouse",&EQGameSceneMgr::inMyHouse)
			.def("startLoading",&EQGameSceneMgr::startLoading)
			.def("endLoading",&EQGameSceneMgr::endLoading)
			.def("getIsLoading",&EQGameSceneMgr::getIsLoading)
	];

	return true;
}