/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		2/6/2011	10:55
* 文件: 	DriftBottleSys_lua.cpp
* 描述:		透露到LUA脚本
******************
*********************************************************************/
#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"
#include "DriftBottleSystem.h"

using namespace luabind;

bool CLuaManager::RegisterDriftBottleSysClass()
{
	module( EQ_GLOBA_CLASS )
		[
			/**
			*/
			luabind::def("getDriftBottleSys", &DriftBottleSystem::getSingleton),
			/**
			*/

			class_<DriftBottleSystem>("DriftBottleSystem")
				.def(constructor<>())
				.def("dropBottle",		    &DriftBottleSystem::dropBottle)
				.def("pickBottle",			&DriftBottleSystem::pickBottle)
				.def("deleteBottle",		&DriftBottleSystem::deleteBottle)
				.def("getDriftBottleMgrPtr",&DriftBottleSystem::getDriftBottleMgrPtr)
				.def("caculateTimeInterval",&DriftBottleSystem::caculateTimeInterval)
				.def("reqEqUserInfo",		&DriftBottleSystem::reqEqUserInfo)
// 				.def("loadGiftCardData",		        &GiftSystem::loadGiftCardData)
// 				.def("reqPackGift",					    &GiftSystem::reqPackGift)
// 				.def("reqPutGift",						&GiftSystem::reqPutGift)
// 				.def("reqPickUpGift",		            &GiftSystem::reqPickUpGift)
// 				.def("reqCanclePackGift",		        &GiftSystem::reqCanclePackGift)
// 				.def("reqEndGiftService",		        &GiftSystem::reqEndGiftService)
// 				.def("reqStartGiftService",		        &GiftSystem::reqStartGiftService)
// 				.def("setDIYGiftInfo",				    &GiftSystem::setDIYGiftInfo)	
// 				.def("getCurPickGiftInfo",		        &GiftSystem::getCurPickGiftInfo)	
// 				.def("setGiftItem",					    &GiftSystem::setGiftItem)	
// 				.def("setMailGiftInfo",					&GiftSystem::setMailGiftInfo)
// 				.def("reqExpressGift",					&GiftSystem::reqExpressGift)
// 				.def("playGiftCardFlash",				&GiftSystem::playGiftCardFlash)
		];
	return true;
}