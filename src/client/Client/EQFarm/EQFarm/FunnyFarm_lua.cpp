#include "EQFarmPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"
#include "FunnyFarm.h"
#include "FarmTable.h"
#include "EQCamera.h"
#include "EQGameMgr.h"
#include "ScriptFunction.h"

using namespace luabind;
/**
注册GameObj及其相关用到的类
*/
bool CLuaManager::RegisterFarmClass()
{
	module( EQ_GLOBA_CLASS )
		[
			/**
			*/
			luabind::def("getFunnyFarm",	&FunnyFarm::getSingleton),
			luabind::def("getFarmTableMgr", &FarmTableMgr::getSingleton),


			class_<FunnyFarm>("FunnyFarm")
				.def(constructor<>())
				.def("reqEnterFarm",			&FunnyFarm::reqEnterFarm)
				.def("reqLogList",				&FunnyFarm::reqLogList)
				.def("getFarmMoney",			&FunnyFarm::getFarmMoney)
				.def("getFarmEqMoney",			&FunnyFarm::getFarmEqMoney)
				.def("reqComment",				&FunnyFarm::reqComment)
				.def("reqCommentList",			&FunnyFarm::reqCommentList)
				.def("reqClearComment",			&FunnyFarm::reqClearComment)
				.def("reqclearLog",				&FunnyFarm::reqclearLog)
				.def("reqReplyComment",			&FunnyFarm::reqReplyComment)
				.def("reqFriendFarmState",		&FunnyFarm::reqFriendFarmState)
				.def("reqSetting",				&FunnyFarm::reqSetting)
				.def("reqSettingDefault",		&FunnyFarm::reqSettingDefault)
				.def("reqSetSetting",			&FunnyFarm::reqSetSetting)
				.def("setSettingFlag",			&FunnyFarm::setSettingFlag)
				.def("getSettingFlag",			&FunnyFarm::getSettingFlag)

				.def("reqLockStorageItem",		&FunnyFarm::reqLockStorageItem)
				.def("reqUnLockStorageItem",	&FunnyFarm::reqUnLockStorageItem)
				.def("reqAllSell",				&FunnyFarm::reqAllSell)
				.def("reqOneSell",				&FunnyFarm::reqOneSell)
				.def("reqBuyGoods",				&FunnyFarm::reqBuyGoods)
				.def("reqChangeDec",			&FunnyFarm::reqChangeDec)
				.def("reqRemoveDec",			&FunnyFarm::reqRemoveDec)
				//.def("getCrop",					&FunnyFarm::getCrop)
				.def("getSelfFarmInfo",			&FunnyFarm::getSelfFarmInfo)
				.def("getAction",				&FunnyFarm::getAction)
				.def("setPushFlag",				&FunnyFarm::setPushFlag)
				.def("reqCanUpgradeLand",		&FunnyFarm::reqCanUpgradeLand)
				.def("reqUpgradeLand",			&FunnyFarm::reqUpgradeLand)
				.def("checkAllpickAward",		&FunnyFarm::checkAllpickAward)
				.def("reqUsePropStr",			&FunnyFarm::reqUsePropStr)
				.def("checkAllHintData",		&FunnyFarm::checkAllHintData)
				.def("setHintDymanicUI",		&FunnyFarm::setHintDymanicUI)
				.def("insertAccountList",		&FunnyFarm::insertAccountList)
				.def("reqEqUsersInfo",			&FunnyFarm::reqEqUsersInfo)
				.def("reqFarmTaskList",			&FunnyFarm::reqFarmTaskList)
				.def("reqFarmTaskComplete",		&FunnyFarm::reqFarmTaskComplete)
				.def("getFarmTaskCount",		&FunnyFarm::getFarmTaskCount)
				.def("getFarmTaskIdByIndex",	&FunnyFarm::getFarmTaskIdByIndex)
				.def("existFarmTask",			&FunnyFarm::existFarmTask)

				//.def("previewDec",				&FunnyFarm::previewDec)
				//.def("endPreviewDec",				&FunnyFarm::endPreviewDec)

				.def_readonly("mAccount",		&FunnyFarm::mAccount)
				.def_readonly("mUsername",		&FunnyFarm::mUsername)
				.def_readonly("mLev",			&FunnyFarm::mLev)
				.def_readonly("mExp",			&FunnyFarm::mExp)
				.def_readonly("mGameGold",		&FunnyFarm::mGameGold)
				.def_readonly("mBulletin",		&FunnyFarm::mBulletin)
				.def_readonly("mCropNum",		&FunnyFarm::mCropNum)
				.def_readwrite("pushFlag",		&FunnyFarm::mpushFlag)
			,
			class_<FarmShopRow>("FarmShopRow")
				.def_readonly("item_id",		&FarmShopRow::item_id)
				.def_readonly("gameprice",		&FarmShopRow::gameprice)
				.def_readonly("eqprice",		&FarmShopRow::eqprice)
				.def_readonly("intro",			&FarmShopRow::intro)
				.def_readonly("itemLevel",		&FarmShopRow::itemLevel)
				.def_readonly("special",		&FarmShopRow::special)
				.def_readonly("repeatBuy",		&FarmShopRow::repeatBuy)
			,

			class_<SeedRow>("SeedRow")
				.def_readonly("seed_id",		&SeedRow::id)
				.def_readonly("seed_name",		&SeedRow::name)
				.def_readonly("expect_output",	&SeedRow::expect_output)
				.def_readonly("fruit_time",		&SeedRow::fruit_time)
				.def_readonly("sprout_time",	&SeedRow::sprout_time)
				.def_readonly("s_leaf_time",	&SeedRow::s_leaf_time)
				.def_readonly("b_leaf_time",	&SeedRow::b_leaf_time)
				.def_readonly("bloom_time",		&SeedRow::bloom_time)
				.def_readonly("pick_time",		&SeedRow::pick_time)
				.def_readonly("use_level",		&SeedRow::use_level)
				.def_readonly("pick_exp",		&SeedRow::pick_exp)
				.def_readonly("fruit_id",		&SeedRow::fruit_id)
			,

			class_<CropStaNameRow>("CropStaNameRow")
				.def_readonly("id",				&CropStaNameRow::id)
				.def_readonly("sprout_name",	&CropStaNameRow::sprout_name)
				.def_readonly("s_leaf_name",	&CropStaNameRow::s_leaf_name)
				.def_readonly("b_leaf_name",	&CropStaNameRow::b_leaf_name)
				.def_readonly("bloom_name",		&CropStaNameRow::bloom_name)
				.def_readonly("fruit_name",		&CropStaNameRow::fruit_name)
				.def_readonly("pick_name",		&CropStaNameRow::pick_name)
			,

			class_<OrnamentRow>("OrnamentRow")
				.def_readonly("id",				&OrnamentRow::id)
				.def_readonly("type",			&OrnamentRow::type)
			,

			class_<FarmPropRow>("FarmPropRow")
			.def_readonly("id",				&FarmPropRow::prop_id)
			.def_readonly("type",			&FarmPropRow::prop_type)
			,

			class_<FarmTaskRow>("FarmTaskRow")
				.def_readonly("id",				&FarmTaskRow::id)
				.def_readonly("title",			&FarmTaskRow::title)
				.def_readonly("intention",		&FarmTaskRow::intention)
				.def_readonly("describe",		&FarmTaskRow::describe)
				.def_readonly("item_reward",	&FarmTaskRow::item_reward)
				.def_readonly("item_count",		&FarmTaskRow::item_count)
				.def_readonly("game_reward",	&FarmTaskRow::game_reward)
				.def_readonly("eq_reward",		&FarmTaskRow::eq_reward)
				.def_readonly("exp",			&FarmTaskRow::exp)
				.def_readonly("time_limit",		&FarmTaskRow::time_limit)
				.def_readonly("is_repeat",		&FarmTaskRow::is_repeat)
			,
			class_<FarmLevelRow>("FarmLevelRow")
				.def_readonly("level",			&FarmLevelRow::level)
				.def_readonly("next_exp",		&FarmLevelRow::next_exp)
				.def_readonly("sow_exp",		&FarmLevelRow::sow_exp)
				.def_readonly("water_exp",		&FarmLevelRow::water_exp)
				.def_readonly("weed_exp",		&FarmLevelRow::weed_exp)
				.def_readonly("worm_exp",		&FarmLevelRow::worm_exp)
				.def_readonly("assart_exp",		&FarmLevelRow::assart_exp)
			,
			class_<farm_decoration>("farm_decoration")
				.def_readonly("background",		&farm_decoration::background)
				.def_readonly("house",			&farm_decoration::house)
				.def_readonly("doghouse",		&farm_decoration::doghouse)
				.def_readonly("signpost",		&farm_decoration::signpost)
				.def_readonly("fence",			&farm_decoration::fence)
				.def_readonly("scarecrow",		&farm_decoration::scarecrow)
				.def_readonly("billboard",		&farm_decoration::billboard)
				.def_readonly("billboard_content",		&farm_decoration::billboard_content)
			,
			
			class_<SelfFarmInfo>("SelfFarmInfo")
				.def_readonly("mSaccount",		&SelfFarmInfo::mSaccount)
				.def_readonly("mSusername",		&SelfFarmInfo::mSusername)
				.def_readonly("mSlev",			&SelfFarmInfo::mSlev)
				.def_readonly("mSexp",			&SelfFarmInfo::mSexp)
				//.def_readonly("mSpet",			&SelfFarmInfo::mSpet)
				.def_readonly("mSdeco",			&SelfFarmInfo::mSdeco)
				.def_readonly("mfarmState",		&SelfFarmInfo::mfarmState)
			,
			class_<item>("item")
				.def_readonly("instance_id",	&item::instance_id)
				.def_readonly("template_id",	&item::template_id)
			,

			class_<FarmTableMgr>("FarmTableMgr")
				.def("getFarmShopRow",			&FarmTableMgr::getFarmShopRow)
				.def("getFarmShopTableCount",	&FarmTableMgr::getFarmShopTableCount)
				.def("getFarmItemRow",			&FarmTableMgr::getFarmItemRow)
				.def("getSeedRow",				&FarmTableMgr::getSeedRow)
				.def("getOrnamentRow",			&FarmTableMgr::getOrnamentRow)
				.def("getFarmLevelRow",			&FarmTableMgr::getFarmLevelRow)
				.def("getCropStaNameRow",		&FarmTableMgr::getCropStaNameRow)
				.def("getFarmPropRow",			&FarmTableMgr::getFarmPropRow)
				.def("getFarmTaskRow",			&FarmTableMgr::getFarmTaskRow)
				.def("getFarmTaskImagesetCount",&FarmTableMgr::getFarmTaskImagesetCount)
				.def("getFarmTaskImageset",		&FarmTableMgr::getFarmTaskImageset)
				.def("getFarmTaskImageCount",	&FarmTableMgr::getFarmTaskImageCount)
				.def("getFarmTaskImage",		&FarmTableMgr::getFarmTaskImage)
			,

			class_<CropsCtrl>("CropsCtrl")
				.def("getCurStage",				&CropsCtrl::getCurStage)
				.def("getHealth",				&CropsCtrl::getHealth)
				.def("getRemain",				&CropsCtrl::getRemain)
				.def("getOutput",				&CropsCtrl::getOutput)
				.def("getRemainStime",			&CropsCtrl::getRemainStime)
				.def("getCropTpltID",			&CropsCtrl::getCropTpltID)
				.def("hasCrop",					&CropsCtrl::hasCrop)
				.def("getRemainTime",			&CropsCtrl::getRemainTime)
				.def("getPicked",				&CropsCtrl::getPicked)
			/**
			*/
		];

	return true;
}