/***********************************************************************
* 作者: xiaoMing
* 时间: 2010-06-12
* 描述: 负责将SmallModules的函数透露给LUA
***********************************************************************/
#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"

#include "SmallModules.h"

using namespace luabind;

bool CLuaManager::RegisterSmallModulesClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		*/
		luabind::def("getSmallModules", &SmallModules::getSingleton),
		/**
		*/
		class_<SmallModules>("SmallModules")
			.def(constructor<>())
			.def_readonly("mCostMoney",&SmallModules::mCostMoney)
			.def_readonly("changeActive",&SmallModules::mChangeActive)
			.def_readonly("changeHouseClean",&SmallModules::mChangeHouseClean)
			.def_readonly("changeClean",&SmallModules::mChangeClean)
			.def_readonly("changeHealth",&SmallModules::mChangeHealth)
			.def_readonly("changeCharm",&SmallModules::mChangeCharm)
			.def_readonly("workEventID",&SmallModules::mWorkEventID)
			.def_readonly("workMoney",&SmallModules::mWorkMoney)
			.def_readonly("eventMoney",&SmallModules::mEventMoney)
			.def_readonly("eventItem",&SmallModules::mEventItem)
			.def_readonly("workId",&SmallModules::mWorkId)
			// 邀请
			.def("inviteOtherPlayer",&SmallModules::inviteOtherPlayer)
			.def("agreeInvite",		 &SmallModules::agreeInvite)
			.def("disagreeInvite",   &SmallModules::disagreeInvite)
			.def("getInvitee",		 &SmallModules::getInvitee)
			.def("getInvitorAcc",	&SmallModules::getInvitorAcc)
			.def("getInviteType",	 &SmallModules::getInviteType)
			// 交易模块
			.def("inviteDealByPicked",&SmallModules::inviteDealByPicked)
			.def("sendUnputItem",	  &SmallModules::sendUnputItem)
			.def("reqCancelTrade",	  &SmallModules::reqCancelTrade)
			.def("reqSureTrade",	  &SmallModules::reqSureTrade)
			.def("sendUpdateMoney",   &SmallModules::sendUpdateMoney)
			.def("sendPutItem",		  &SmallModules::sendPutItem)
			.def("getTradeItemId",    &SmallModules::getTradeItemId)
			.def("getTradeItemCount", &SmallModules::getTradeItemCount)
			.def("getTradeItemEffectTime",&SmallModules::getTradeItemEffectTime)
			.def("getAcquireMoney",   &SmallModules::getAcquireMoney)
			.def("getLoseMoney",	  &SmallModules::getLoseMoney)
			.def("getTraderName",	  &SmallModules::getTraderName)
			.def("sendSwap",		  &SmallModules::sendSwap)
	
			// 房屋买卖
			.def("saveHouseShopData", &SmallModules::saveHouseShopData)
			.def("reqBuyHouse",		  &SmallModules::reqBuyHouse)
			.def("reqBuyHouseAdd",    &SmallModules::reqBuyHouseAdd)
			.def("reqBuyHouseReplace",&SmallModules::reqBuyHouseReplace)
			.def("reqPreviewHouse",	  &SmallModules::reqPreviewHouse)
			.def("reqPreviewHouse1",  &SmallModules::reqPreviewHouse1)
			.def("reqEndPreviewHouse",&SmallModules::reqEndPreviewHouse)
			// 家政
			.def("reqDomesticService",&SmallModules::reqDomesticService)
			.def("getServiceId",	  &SmallModules::getServiceId)
			.def("getMoneyRate",	  &SmallModules::getMoneyRate)
			.def("changeValue",		  &SmallModules::changeValue)
			.def("reqEndService",	  &SmallModules::reqEndService)
			.def("saveDoServiceData", &SmallModules::saveDoServiceData)	
			.def("reqOtherPlayer",	  &SmallModules::reqOtherPlayer)
			.def("reqStartDomestic",  &SmallModules::reqStartDomestic)		
			// 其他玩家RTT
			.def("initOtherPlayerRTT", &SmallModules::initOtherPlayerRTT)
			// 美容院RTT
			.enum_("BeautyMoneyType")
			[
				value("BMT_EQ",SmallModules::BMT_EQ),
				value("BMT_GOLD",SmallModules::BMT_GOLD)
			]
			.enum_("BeautyAdjustType")
			[
				value("BAT_HAIR",SmallModules::BAT_HAIR),
				value("BAT_HAIRCOLOR",SmallModules::BAT_HAIRCOLOR),
				value("BAT_FACE",SmallModules::BAT_FACE),
				value("BAT_SKINCOLOR",SmallModules::BAT_SKINCOLOR),
				value("BAT_BEARD",SmallModules::BAT_BEARD)
			]
			.enum_("BeautySectionType")
			[
				value("BST_HEAD",SmallModules::BST_HEAD),
				value("BST_HAIR",SmallModules::BST_HAIR),
				value("BST_BEARD",SmallModules::BST_BEARD)
			]
			.def("initBeautyRTT", &SmallModules::initBeautyRTT)
			.def("reqStartBeauty", &SmallModules::reqStartBeauty)
			.def("reqCancelBeauty", &SmallModules::reqCancelBeauty)
			.def("reqEndBeauty", &SmallModules::reqEndBeauty)
			.def("beautyRTTRotate", &SmallModules::beautyRTTRotate)
			.def("beautyRTTChangeLooks", &SmallModules::beautyRTTChangeLooks)
			.def("getSectionVal", &SmallModules::getSectionVal)
			.def("setBeautyChooseUI", &SmallModules::setBeautyChooseUI)
			.def("clearBeautyChooseUI", &SmallModules::clearBeautyChooseUI)
			.def("saveBeautySectionTypes", &SmallModules::saveBeautySectionTypes)
			.def("saveBeautyColors", &SmallModules::saveBeautyColors)
			.def("getBeautyTypes", &SmallModules::getBeautyTypes)
			.def("insertCurVal", &SmallModules::insertCurVal)
			.def("getBeautyPages", &SmallModules::getBeautyPages)
			.def("getBeautySectionVal", &SmallModules::getBeautySectionVal)
			.def("clearBeautySectionTypes", &SmallModules::clearBeautySectionTypes)
			.def("getBeautySectionMoney", &SmallModules::getBeautySectionMoney)
			// 房屋信息
			.def("reqHouseBaseInfo", &SmallModules::reqHouseBaseInfo)
			.def("reqHouseList", &SmallModules::reqHouseList)
			.def("reqHouseListByAcc", &SmallModules::reqHouseListByAcc)
			.def("reqChangeHouse", &SmallModules::reqChangeHouse)
			.def("getHouseSize", &SmallModules::getHouseSize)
			.def("getHouseListSize", &SmallModules::getHouseListSize)
			.def("getHouseListData", &SmallModules::getHouseListData)
			.def("getFHouseSize", &SmallModules::getFHouseSize)
			.def("getFHouseListSize", &SmallModules::getFHouseListSize)
			.def("getFHouseListData", &SmallModules::getFHouseListData)
			.def("reqClearHouseLog", &SmallModules::reqClearHouseLog)
			.def("reqSetHousePermission", &SmallModules::reqSetHousePermission)
			.def("reqSetHouseWelWords", &SmallModules::reqSetHouseWelWords)
			.def("otherPlayerRttRotate", &SmallModules::otherPlayerRttRotate)
			.def("saveWorks", &SmallModules::saveWorks)
			.def("reqWork", &SmallModules::reqWork)
			.def("reqEndWork", &SmallModules::reqEndWork)
			.def("reqStartWork", &SmallModules::reqStartWork)
			.def("setBlur", &SmallModules::setBlur)	
			.def("reqGiftLog", &SmallModules::reqGiftLog)	
			.def("showLogGift", &SmallModules::showLogGift)
			.def("showLogGiftByIcon", &SmallModules::showLogGiftByIcon)
			.def("delLogGiftByIcon", &SmallModules::delLogGiftByIcon)	
			.def("initGiftLog", &SmallModules::initGiftLog)	
			.def("reqCleanGiftLog", &SmallModules::reqCleanGiftLog)
		,
		class_<THouseData>("THouseData")
		.def(constructor<>())
		.def_readonly("name",&THouseData::name)
		.def_readonly("buy_time",&THouseData::buy_time)
		.def_readonly("level",&THouseData::level)
		.def_readonly("price",&THouseData::price)
		.def_readonly("max_player",&THouseData::max_player)
		.def_readonly("type",&THouseData::type)
		.def_readonly("used",&THouseData::used)
		.def_readonly("empty",&THouseData::empty)
		.def_readonly("template_id",&THouseData::template_id)
	];

	return true;
}
