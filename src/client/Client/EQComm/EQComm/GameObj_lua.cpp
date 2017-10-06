#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"
#include "GameObj.h"
#include "Player.h"
#include "MainPlayer.h"
#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "ObjMgr.h"
#include "EQGameMgr.h"
#include "AvatarRTT.h"
#include "ItemTable.h"
#include "CEGUIMouseCursor.h"
#include "EQGameSceneMgr.h"
#include "FriendSystem.h"
#include "PlayerSearch.h"
#include "NPC.h"
#include "LogicTable.h"
#include "UIAnimationMgr.h"
#include "ItemInteractSys.h"
#include "OffLineMsgMgr.h"
#include "GameState.h"
#include "EQOIS.h"
#include "DynamicUIMgr.h"

using namespace luabind;
/**
注册GameObj及其相关用到的类
*/
bool CLuaManager::RegisterGameObjClass()
{
	module( EQ_GLOBA_CLASS )
		[

		class_<EQOIS>("EQOIS")
		.def("getSkip",&EQOIS::getSkip)
		.def("setSkip",&EQOIS::setSkip)
		,
		luabind::def("getEQOIS", &EQOIS::getSingleton),
		
		/**
		*/
		//def("getMainPlayer",&MainPlayer::getSingleton()),
		class_<EQGameMgr>("GameMgr")
			.def("getRTT",&EQGameMgr::getRTT)
		,
		luabind::def("getGameMgr",		&EQGameMgr::getSingleton),
		/**
		*/
		class_<Avatar>("Avatar")
			.def(constructor<uint>())
			.def("getAdjuster",&Avatar::getAdjuster)
			.def("takeOffAll",&Avatar::takeOffAll)
		,
		/**
		*/
		class_<AvatarRTT>("AvatarRTT")
			.def(constructor<>())
			.def("startRTT", &AvatarRTT::startRTT)
			.def("getAvatar",&AvatarRTT::getAvatar)
			.def("changeSex", &AvatarRTT::changeSex)
			.def("getSex",&AvatarRTT::getSex)
			.def_readwrite("mAvatar",&AvatarRTT::mAvatar)
		,
		/**
		*/
		class_<AvatarAdjuster>("AvatarAdjuster")
			.def(constructor<Avatar*>())
			.enum_("AvatarAdjustType")
			[
				value("atSex",AvatarAdjuster::atSex),
				value("atHairType",AvatarAdjuster::atHairType),
				value("atHairColor",AvatarAdjuster::atHairColor),
				value("atFace",AvatarAdjuster::atFace),
				value("atSkin",AvatarAdjuster::atSkin),
				value("atBeard",AvatarAdjuster::atBeard)
			]
			.def("doNext",&AvatarAdjuster::doNext)
			.def("doPrev",&AvatarAdjuster::doPrev)
			.def("getCurrentVal", &AvatarAdjuster::getCurrentVal)
			.def("doChange", &AvatarAdjuster::doChange)
			.def("setForCreateRole", &AvatarAdjuster::setForCreateRole)
			.def("reset",&AvatarAdjuster::reset)
		,
		/**
		*/
		class_<GameObj>("GameObj")
			.def(constructor<>())
			.def("getAvatar",&GameObj::getAvatar)
			.def("getType",&GameObj::getType)
			.def("getYaw",&GameObj::getYaw)
			.def("addParticle",&GameObj::addParticle)
			.def("delParticleByHandle",&GameObj::delParticleByHandle)
			.def("delParticleByID",&GameObj::delParticleByID)
			.def("startParticle",&GameObj::startParticle)
			.def("pauseParticle",&GameObj::pauseParticle)
			.def("stopParticle",&GameObj::stopParticle)
			.def("setState",&GameObj::setState)
			.def("setMoveType",&GameObj::setMoveType)
		,
		/**
		*/
		class_<player_property>("player_property")
			.def(constructor<>())
			.def_readwrite("account",&player_property::account)
			.def_readwrite("player_clean",&player_property::player_clean)
			.def_readwrite("player_health",&player_property::player_health)
			.def_readwrite("player_charm",&player_property::player_charm)
			.def_readwrite("active_value",&player_property::active_value)
		,
		/**
		*/
		class_<Player,GameObj>("Player")
			.def(constructor<>())
			.enum_("eBody")
			[
				value("BODY_HAT",Player::BODY_HAT),
				value("BODY_CLOTHES",Player::BODY_CLOTHES),
				value("BODY_TROUSERS",Player::BODY_TROUSERS),
				value("BODY_SHOES",Player::BODY_SHOES),
				value("BODY_GLASSES",Player::BODY_GLASSES),
				value("BODY_BACK",Player::BODY_BACK),
				value("BODY_HAND",Player::BODY_HAND),
				value("BODY_TRINKET",Player::BODY_TRINKET)
			]
			.def("updateBaseLook",&Player::updateBaseLook)
			.def_readwrite("mPlayerBD",&Player::mPlayerBD)
			.def("getItemRow",&Player::getItemRow)
			.def("getItemID",&Player::getItemID)
			.def("getItemInsIdStr",&Player::getItemInsIdStr)
			.def("getItemIDByGlobaIdx",&Player::getItemIDByGlobaIdx)
			.def("getItemDelTime",&Player::getItemDelTime)
			.def("getItemDelTimeByGlobalIdx",&Player::getItemDelTimeByGlobalIdx)
			
			.def("getItemGird",&Player::getItemGird)
			.def("getItemGridByGlobaIdx",&Player::getItemGridByGlobaIdx)
			.def("getProperty", &Player::getProperty)
			.def("getLevel",	&Player::getLevel)
			.def("getHealtyState", &Player::getHealtyState)
			.def("getName",		&Player::getName)
			.def("getSex",		&Player::getSex)
			.def("getState",	&Player::getState)
			.def("getPlayerAccount",	&Player::getPlayerAccount)
			.def("isMale",&Player::isMale)
			.def("getShopCenterRow",&Player::getShopCenterRow)
			.def("setShopCenterType",&Player::setShopCenterType)
			.def("setPackLastPage",&Player::setPackLastPage)
			.def("setPackNextPage",&Player::setPackNextPage)
			.def("setPackCurPage",&Player::setPackCurPage)
			.def("getPackCurPage",&Player::getPackCurPage)
			.def("getPackPageCount",&Player::getPackPageCount)
			.def("getGameGold",&Player::getGameGold)
			.def("getEQGold",&Player::getEQGold)
			.def("getTask",&Player::getTask)
			.def("addShoppingCart",&Player::addShoppingCart)
			.def("delShoppingCart",&Player::delShoppingCart)
			.def("delShoppingCartByData",&Player::delShoppingCartByData)
			.def("clearShoppingCart",&Player::clearShoppingCart)
			.def("sendShoppingCart",&Player::sendShoppingCart)
			.def("getShoppingCartPrice",&Player::getShoppingCartPrice)
			.def("getStorageCoin",&Player::getStorageCoin)
			.def("reqSaveMoneyToStorage",&Player::reqSaveMoneyToStorage)
			.def("reqGetMoneyFromStorage",&Player::reqGetMoneyFromStorage)
			.def("isInItemPack",&Player::isInItemPack)
			.def("isGmEnabled",&Player::isGmEnabled)
			.def("showHeadupBigExpression",&Player::showHeadupBigExpression)
			.def("getChatPaoPaoIsVisible",&Player::getChatPaoPaoIsVisible)
		,
		/**
		*/
		class_<CreateEquip>("CreateEquip")
			.def(constructor<>())
			.def_readwrite("curHatEquip", &CreateEquip::curHatEquip)
			.def_readwrite("curClothEquip", &CreateEquip::curClothEquip)
			.def_readwrite("curTrouserEquip", &CreateEquip::curTrouserEquip)
			.def_readwrite("curShoesEquip", &CreateEquip::curShoesEquip)
			.def_readwrite("curGlassesEquip", &CreateEquip::curGlassesEquip)
			.def_readwrite("curBackEquip", &CreateEquip::curBackEquip)
			.def_readwrite("curHandEquip", &CreateEquip::curHandEquip)
			.def_readwrite("curTrinketEquip", &CreateEquip::curTrinketEquip)
		,
		/**
		*/
		class_<MainPlayer,Player>("MainPlayer")
			.def(constructor<>())
			.def("setCeateEquip", &MainPlayer::setCeateEquip)
			.def("getState",	&MainPlayer::getState)
			.def("reqContinueWalk",	&MainPlayer::reqContinueWalk)
			.def("getMoveForWhat",	&MainPlayer::getMoveForWhat)
			.def("getFlowerCount",	&MainPlayer::getFlowerCount)
			.def("getDecoration",	&MainPlayer::getDecoration)
			.def("getFlowerToday",	&MainPlayer::getFlowerToday)
			.def("getEggToday",		&MainPlayer::getEggToday)
			.def("reqPlayBigExpression",		&MainPlayer::reqPlayBigExpression)
		,
		/**
		*/
		class_<ObjMgr>("ObjMgr")
			.def(constructor<>())
			.enum_("GAMEOBJ_TYPE")
			[
				value("GOT_OBJ",ObjMgr::GOT_OBJ),
				value("GOT_PLAYER",ObjMgr::GOT_PLAYER),
				value("GOT_MAINPLAYER",ObjMgr::GOT_MAINPLAYER),
				value("GOT_NPC",ObjMgr::GOT_NPC)
			]
			.def("reqestObj",&ObjMgr::reqestObj)
			//.def("findObj",&ObjMgr::findObj)
			.def("getPickObj",&ObjMgr::getPickObj)
			.def("getPickNPC",&ObjMgr::getPickNPC)
			.def("getPickPlayer",&ObjMgr::getPickPlayer)
			.def("isMainPlayer",&ObjMgr::isMainPlayer)
			.def("setVisibleType",&ObjMgr::setVisibleType)
		,

		class_<FriendSystem>("FriendSystem")
			.def(constructor<>())
			.def("reqAddasFriend",&FriendSystem::reqAddasFriend)
			.def("reqAsFriend",	&FriendSystem::reqAsFriend)
			.def("reqFriendList", &FriendSystem::reqFriendList)
			.def("reqEnterFriendHouseByLua",	&FriendSystem::reqEnterFriendHouseByLua)
			.def("reqInviteEnterHouseByLua",	&FriendSystem::reqInviteEnterHouseByLua)
			.def("reqKickPlayerByLua",			&FriendSystem::reqKickPlayerByLua)
			.def("getFriendName",				&FriendSystem::getFriendName)
			.def("getFriendAccount", (std::string(FriendSystem::*)(std::string)) &FriendSystem::getFriendAccount)
			.def("insertFriendPhoto",			&FriendSystem::insertFriendPhoto)
			.def("getFriendPhotoName",			&FriendSystem::getFriendPhotoName)
			.def("setCurListPhotos",			&FriendSystem::setCurListPhotos)
			.def("searchEx",					&FriendSystem::searchEx)
			.def("search_matchKey",			&FriendSystem::search_matchKey)
			.def("getSearchExResult",			&FriendSystem::getSearchExResult)
			.def("getSearchExCnt",				&FriendSystem::getSearchExCnt)
			.def("getSearchExMaxCnt",			&FriendSystem::getSearchExMaxCnt)
			.def("setSearchExMaxCnt",			&FriendSystem::setSearchExMaxCnt)
			.def("reqInviteSelFriendHouse",		&FriendSystem::reqInviteSelFriendHouse)
			.def("reqPreviewFHousedataByFriendList", &FriendSystem::reqPreviewFHousedataByFriendList)
			.def("reqPreviewFHousedataByCircleMenu", &FriendSystem::reqPreviewFHousedataByCircleMenu)
			.def("reqHouseListFromFriendList",		&FriendSystem::reqHouseListFromFriendList)
			.def("reqHouseListFromCircleMenu",		&FriendSystem::reqHouseListFromCircleMenu)
		,

		class_<PlayerSearch>("PlayerSearch")
			.def(constructor<>())
			.def("req_searchPlayer",			&PlayerSearch::req_search)
			.def("clearPlayerList",				&PlayerSearch::clearPlayerList)
		,

		luabind::def("getDynamicUIMgr",			&DynamicUIMgr::getSingleton),

		class_<DynamicUIMgr>("DynamicUIMgr")
		.def(constructor<>())

		.def("createCommon",				&DynamicUIMgr::createCommon)
		.def("createChatPaoPao",			&DynamicUIMgr::createChatPaoPao)
		.def("createNumberPaoPao",			&DynamicUIMgr::createNumberPaoPao)
		.def("createHeadup",				&DynamicUIMgr::createHeadup)
		.def("createCircleMenuDynamicInfo", &DynamicUIMgr::createCircleMenuDynamicInfo)
		.def("createFarmPaoPao",			&DynamicUIMgr::createFarmPaoPao)
		.def("createFarmHarvest",			&DynamicUIMgr::createFarmHarvest)
		,

		class_<DynamicUIInfo>("DynamicUIInfo")
		.def(constructor<>())
		.enum_("DynamicUIType")
		[
			value("EDUI_COMMON",EDUI_COMMON),
			value("EDUI_CHATPAOPAO",EDUI_CHATPAOPAO),
			value("EDUI_NUMBERPAOPAO",EDUI_NUMBERPAOPAO),
			value("EDUI_HEADUP",EDUI_HEADUP),
			value("EDUI_FARMPAOPAO",EDUI_FARMPAOPAO),
			value("EDUI_FARMHARVEST",EDUI_FARMHARVEST),
			value("EDUI_FARMHEADUP",EDUI_FARMHEADUP),
			value("EDUI_CIRCLEMENU",EDUI_CIRCLEMENU),
			value("EDUI_BIGEXPRESSION",EDUI_BIGEXPRESSION)
		]
		.def_readwrite("type",&DynamicUIInfo::type)
		.def_readwrite("winname",&DynamicUIInfo::winname)
		.def_readwrite("life",&DynamicUIInfo::life)
		.def_readwrite("autoRemove", &DynamicUIInfo::autoRemove)
		.def_readwrite("autoDel",&DynamicUIInfo::autoDel)
		.def_readwrite("objname",&DynamicUIInfo::objname)
		.def_readwrite("objtype",&DynamicUIInfo::objtype)
		,

		class_<UIAnimationInfo>("UIAnimationInfo")
		.def(constructor<>())
		.enum_("UIAnimationType")
		[
			value("EUIA_MOVE",UIAnimationInfo::EUIA_MOVE),
			value("EUIA_SCALE",UIAnimationInfo::EUIA_SCALE),
			value("EUIA_FADE",UIAnimationInfo::EUIA_FADE),
			value("EUIA_FLOATUP",UIAnimationInfo::EUIA_FLOATUP),
			value("EUIA_UPDOWN",UIAnimationInfo::EUIA_UPDOWN),
			value("EUIA_POPUP",UIAnimationInfo::EUIA_POPUP)
		]
		.def_readwrite("luaPlayBegin",&UIAnimationInfo::mLuaPlayBegin)
		.def_readwrite("luaPlayEnd",&UIAnimationInfo::mLuaPlayEnd)
		.def_readwrite("type",&UIAnimationInfo::mType)
		.def_readwrite("beginTime", &UIAnimationInfo::mBeginTime)
		.def_readwrite("delayTime",	&UIAnimationInfo::mDelayTime)
		.def_readwrite("playTime",	&UIAnimationInfo::mPlayTime)
		.def_readwrite("startAlpha",&UIAnimationInfo::mStartAlpha)
		.def_readwrite("endAlpha",	&UIAnimationInfo::mEndAlpha)
		.def_readwrite("startOffsetX",&UIAnimationInfo::mStartOffsetX)
		.def_readwrite("startOffsetY",&UIAnimationInfo::mStartOffsetY)
		.def_readwrite("endOffsetX",&UIAnimationInfo::mEndOffsetX)
		.def_readwrite("endOffsetY",&UIAnimationInfo::mEndOffsetY)
		,

		class_<ItemInteractSys>("ItemInteractSys")
		.def(constructor<>())
		.def("getItemID",&ItemInteractSys::getItemID)
		.def("getItemDelTime",&ItemInteractSys::getItemDelTime)
		.def("getItemInsIdStr",&ItemInteractSys::getItemInsIdStr)
		.def("getItemGird",&ItemInteractSys::getItemGird)
		.def("getPackCurPage",&ItemInteractSys::getPackCurPage)
		.def("setPackCurPage",&ItemInteractSys::setPackCurPage)
		.def("getPackPageCount",&ItemInteractSys::getPackPageCount)
		.def("getmoney",	&ItemInteractSys::getmoney)

		,
		class_<IITDataConnect>("IITDataConnect")
		.def(constructor<>())
		.def("reqFarmisActive",	&IITDataConnect::reqFarmisActive)
		.def("reqMoveMoney",	&IITDataConnect::reqMoveMoney)
		.def("reqSwithItem",	&IITDataConnect::reqSwithItem)

		,
		class_<GameStateMgr>("GameStateMgr")
		.def(constructor<>())
		.def("isFunnyFarm",&GameStateMgr::isFunnyFarm)
		.def("inMainGame",&GameStateMgr::inMainGame)

		,

		class_<OffLineMsgMgr>("OffLineMsgMgr")
		.def(constructor<>())
		.def("addAnimation",&OffLineMsgMgr::addAnimation)
		.def("removeAnimation",&OffLineMsgMgr::removeAnimation)
		,


		luabind::def("getObjMgr",		&ObjMgr::getSingleton),

		luabind::def("getMainPlayer",		&MainPlayer::getSingleton),

		luabind::def("getSceneMgr",		&EQGameSceneMgr::getSingleton),

		luabind::def("getFriendSys",&FriendSystem::getSingleton),

		luabind::def("getPlayerSearch", &PlayerSearch::getSingleton),

		luabind::def("getItemInteractSys", &ItemInteractSys::getSingleton),
		luabind::def("getIITConn",	&IITDataConnect::getSingleton),
		luabind::def("getGameStateMgr",	&GameStateMgr::getSingleton),
		luabind::def("getOfflineMsgMgr",&OffLineMsgMgr::getSingleton)
		
	];

	return true;
}