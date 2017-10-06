/***********************************************************************
* 作者: xiaoMing
* 时间: 2010-12-02
* 描述: 负责将GiftSystem的函数透露给LUA
***********************************************************************/
#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"

#include "GiftSystem.h"
#include "GiftLogManager.h"

using namespace luabind;

bool CLuaManager::RegisterGiftSystemClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		*/
		luabind::def("getGiftSystem", &GiftSystem::getSingleton),
		/**
		*/
		class_<GiftLogManager>("GiftLogManager")
		.enum_("GiftLogType")
		[
			value("NIL",GiftLogManager::NIL),
			value("DIY_SEND",GiftLogManager::DIY_SEND),
			value("DIY_RECIVE_VIEW",GiftLogManager::DIY_RECIVE_VIEW),
			value("DIY_RECIVE_NO_VIEW",GiftLogManager::DIY_RECIVE_NO_VIEW),
			value("MAIL_SEND",GiftLogManager::MAIL_SEND),
			value("MAIL_RECIVE_VIEW",GiftLogManager::MAIL_RECIVE_VIEW),
			value("MAIL_RECIVE_NO_VIEW",GiftLogManager::MAIL_RECIVE_NO_VIEW)

		],

		class_<GiftSystem>("GiftSystem")
			.enum_("GiftServiceType")
			[
				value("GS_PACK",	GiftSystem::Pack),
				value("GS_PICK_UP", GiftSystem::PickUp),
				value("GS_PUT",		GiftSystem::Put),
				value("GS_GET_EXPRESS_GIFT",GiftSystem::GetExpressGift),
				value("GS_REPLY_MAIL_GIFT",GiftSystem::ReplyMailGift)
			]
			.def(constructor<>())
			.def("cleanCurMakingGiftInfo",		    &GiftSystem::cleanCurMakingGiftInfo)
			.def("loadGiftBoxData",		            &GiftSystem::loadGiftBoxData)
			.def("loadGiftCardData",		        &GiftSystem::loadGiftCardData)
			.def("reqPackGift",					    &GiftSystem::reqPackGift)
			.def("reqPutGift",						&GiftSystem::reqPutGift)
			.def("reqPickUpGift",		            &GiftSystem::reqPickUpGift)
			.def("reqCanclePackGift",		        &GiftSystem::reqCanclePackGift)
			.def("reqEndGiftService",		        &GiftSystem::reqEndGiftService)
			.def("reqStartGiftService",		        &GiftSystem::reqStartGiftService)
			.def("setDIYGiftInfo",				    &GiftSystem::setDIYGiftInfo)	
			.def("getCurPickGiftInfo",		        &GiftSystem::getCurPickGiftInfo)	
			.def("setGiftItem",					    &GiftSystem::setGiftItem)	
			.def("setMailGiftInfo",					&GiftSystem::setMailGiftInfo)
			.def("reqExpressGift",					&GiftSystem::reqExpressGift)
			.def("playGiftCardFlash",				&GiftSystem::playGiftCardFlash)
	];
	return true;
}