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
#include "HouseTable.h"
#include "LogicTable.h"
#include "TaskTable.h"
#include "DragItem.h"
#include "EQCamera.h"

using namespace luabind;
/**
注册GameItem及其相关用到的类
*/
bool CLuaManager::RegisterGameItemClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		*/
		class_<DragItem>("DragItem")
			.def("init",&DragItem::init)
			.def("drag",&DragItem::drag)
			.def("undo",&DragItem::undo)
			.def("isDraging",&DragItem::isDraging)
			//.def("getName",&DragItem::getName)
			.def("getIndex",&DragItem::getIndex)
			.def("getPacktype",&DragItem::getPacktype)
			.def("setVisible",&DragItem::setVisible)
			.def("isVisible",&DragItem::isVisible)
			.def("setSplitNum",&DragItem::setSplitNum)
			.def("getSplitNum",&DragItem::getSplitNum)
		,
		/**
		*/
		class_<ItemGrid>("ItemGrid")
		.def_readonly("mItem",&ItemGrid::mItem)
		.def_readonly("mOverlay",&ItemGrid::mOverlay)
		.def_readonly("mLock",&ItemGrid::mLock)

		,
		class_<ItemPack>("ItemPack")
			.enum_("PackType")
			[
				value("PT_WAREHOUSE",ItemPack::PT_WAREHOUSE),
				value("PT_BODY",ItemPack::PT_BODY),
				value("PT_SHOPCENTER",ItemPack::PT_SHOPCENTER),
				value("PT_SHOPCENTER_RECOMMEND",ItemPack::PT_SHOPCENTER_RECOMMEND),
				value("PT_FITMODE_SHOPCENTER",ItemPack::PT_FITMODE_SHOPCENTER),
				value("PT_FITMODE_SHOPPINGCART",ItemPack::PT_FITMODE_SHOPPINGCART),
				value("PT_NPCSHOP",ItemPack::PT_NPCSHOP),
				value("PT_TRADEPACK",ItemPack::PT_TRADEPACK),
				value("PT_OUTBOX",ItemPack::PT_OUTBOX),
				value("PT_INBOX",ItemPack::PT_INBOX),
				value("PT_STORAGE",ItemPack::PT_STORAGE),
				value("PT_IITSLOTS",ItemPack::PT_IITSLOTS),
				value("PT_FARMDEPOT", ItemPack::PT_FARMDEPOT)
			]
		,
		/**
		*/
		class_<ItemRow>("ItemRow")
			.def_readonly("id",&ItemRow::id)
			.def_readonly("name",&ItemRow::name)
			.def_readonly("type",&ItemRow::type)
			.def_readonly("icon",&ItemRow::icon)
			.def_readonly("imageset",&ItemRow::imageset)
			.def_readonly("tooltip",&ItemRow::tooltip)
			.def_readonly("overlap",&ItemRow::overlap)
			.def_readonly("bind",&ItemRow::bind)
			.def_readonly("subID",&ItemRow::subID)
			.def_readonly("sell_price",&ItemRow::sellPrice)
			.def_readonly("useDel",&ItemRow::useDel)
			.def_readonly("effectTimeID",&ItemRow::effectTimeID)
			.def_readonly("useID",&ItemRow::useID)
			.def_readonly("voiceid",&ItemRow::voiceid)
			.def_readonly("dvoiceid",&ItemRow::dvoiceid)
			.def_readonly("uvoiceid",&ItemRow::uvoiceid)
		,
		/**
		*/
		class_<ItemEffectTimeRow>("ItemEffectTimeRow")
		.def_readonly("id",&ItemEffectTimeRow::id)
		.def_readonly("type",&ItemEffectTimeRow::type)
		.def_readonly("effectTime",&ItemEffectTimeRow::effectTime)
		.def_readonly("tradeCutTime",&ItemEffectTimeRow::tradeCutTime)
		.def_readonly("delTime",&ItemEffectTimeRow::delTime)
		,
		/**
		*/
		class_<ItemEquipRow>("ItemEquipRow")
			.def_readonly("id",&ItemEquipRow::id)
			.def_readonly("name",&ItemEquipRow::name)
			.def_readonly("sex",&ItemEquipRow::sex)
			.def_readonly("equip_pos",&ItemEquipRow::equip_pos)
			.def_readonly("equip_placeholder",&ItemEquipRow::equip_placeholder)
			.def_readonly("equip_disp_id",&ItemEquipRow::equip_disp_id)
			.def_readonly("place",&ItemEquipRow::place)
			.def_readonly("action",&ItemEquipRow::action)
			.def_readonly("type1",&ItemEquipRow::type1)
			.def_readonly("type2",&ItemEquipRow::type2)
		,

		//
		class_<FashionSelectRow>("Fashion_Select")
			.def_readonly("id",&FashionSelectRow::id)
			.def_readonly("sex",&FashionSelectRow::sex)
			.def_readonly("sort1",&FashionSelectRow::sort1)
			.def_readonly("sort2",&FashionSelectRow::sort2)
			.def_readonly("dressId",&FashionSelectRow::dressId)
		,

		//2010-04-26
		class_<HouseCompRow>("HouseComp_Row")
			.def_readonly("ID",&HouseCompRow::ID)
			.def_readonly("DisID",&HouseCompRow::DisID)
			.def_readonly("loopID",&HouseCompRow::LoopID)
		,
		
		//2010-04-26
		class_<LoopMenuRow>("LoopMenu_Row")
			.def_readonly("id",&LoopMenuRow::id)
			.def_readonly("fun1",&LoopMenuRow::fun1)
			.def_readonly("fun2",&LoopMenuRow::fun2)
			.def_readonly("fun3",&LoopMenuRow::fun3)
			.def_readonly("fun4",&LoopMenuRow::fun4)
			.def_readonly("fun5",&LoopMenuRow::fun5)
			.def_readonly("fun6",&LoopMenuRow::fun6)
			.def_readonly("fun7",&LoopMenuRow::fun7)
			.def_readonly("hint",&LoopMenuRow::hint)
			.def_readonly("authority",&LoopMenuRow::authority)
		,

		class_<ShopCenterRow>("ShopCenterRow")
			.def_readonly("good_id",&ShopCenterRow::good_id)
			.def_readonly("item_id",&ShopCenterRow::item_id)
			.def_readonly("type",&ShopCenterRow::type)
			.def_readonly("sex",&ShopCenterRow::sex)
			.def_readonly("money_type",&ShopCenterRow::money_type)
			.def_readonly("price",&ShopCenterRow::price)
			.def_readonly("intro",&ShopCenterRow::intro)
		,

		class_<NpcShopMgrRow>("NpcShopMgrRow")
		.def_readonly("id",&NpcShopMgrRow::id)
		.def_readonly("page1",&NpcShopMgrRow::page1)
		.def_readonly("page2",&NpcShopMgrRow::page2)
		.def_readonly("page3",&NpcShopMgrRow::page3)
		.def_readonly("page4",&NpcShopMgrRow::page4)
		.def_readonly("page5",&NpcShopMgrRow::page5)
		.def_readonly("default_page",&NpcShopMgrRow::default_page)
		.def_readonly("shop_name",&NpcShopMgrRow::shop_name)
		,

		class_<HouseSalesRow>("HouseSalesRow")
		.def_readonly("indexId",		&HouseSalesRow::indexId)
		.def_readonly("level",			&HouseSalesRow::level)
		.def_readonly("name",			&HouseSalesRow::name)
		.def_readonly("price",			&HouseSalesRow::price)
		.def_readonly("recommend",		&HouseSalesRow::recommend)
		.def_readonly("type",			&HouseSalesRow::type)
		.def_readonly("descriptInfo",	&HouseSalesRow::descriptInfo)
		.def_readonly("houseId",		&HouseSalesRow::houseId)
		.def_readonly("payType",		&HouseSalesRow::payType)
		.def_readonly("cost_flower",	&HouseSalesRow::cost_flower)
		.def_readonly("need_decoration",&HouseSalesRow::need_decoration)
		,

		class_<DoServiceRow>("DoServiceRow")
		.def_readonly("id",				&DoServiceRow::id)
		.def_readonly("name",			&DoServiceRow::name)
		.def_readonly("image",			&DoServiceRow::image)
		,

		class_<WorkRow>("WorkRow")
		.def_readonly("id",				&WorkRow::id)
		.def_readonly("grade",			&WorkRow::grade)
		.def_readonly("money",			&WorkRow::money)
		.def_readonly("clean",			&WorkRow::clean)
		.def_readonly("health",			&WorkRow::health)
		.def_readonly("charm",			&WorkRow::charm)
		.def_readonly("active",			&WorkRow::active)
		.def_readonly("healthState",	&WorkRow::healthState)
		.def_readonly("propertyid",		&WorkRow::propertyid)
		.def_readonly("workImage",		&WorkRow::workImage)
		.def_readonly("typeImageSet",	&WorkRow::typeImageSet)
		.def_readonly("typeImage",		&WorkRow::typeImage)
		,

		class_<WorkEventRow>("WorkEventRow")
		.def_readonly("id",				&WorkEventRow::id)
		.def_readonly("image",			&WorkEventRow::image)
		,

		class_<PlayerPropertyRow>("PlayerPropertyRow")
		.def_readonly("id",				&PlayerPropertyRow::id)
		.def_readonly("clean",			&PlayerPropertyRow::clean)
		.def_readonly("health",			&PlayerPropertyRow::health)
		.def_readonly("charm",			&PlayerPropertyRow::charm)
		.def_readonly("active",			&PlayerPropertyRow::active)
		,

		class_<ServicePriceRow>("ServicePriceRow")
		.def_readonly("level",			&ServicePriceRow::level)
		.def_readonly("price",			&ServicePriceRow::price)
		,
		//2010-04-30
		class_<SysMsgRow>("SysMsgRow")
			.def_readonly("id",&SysMsgRow::id)
			.def_readonly("content",&SysMsgRow::content)
			.def_readonly("msgType",&SysMsgRow::type)
			.def("randContent",&SysMsgRow::randContent)
		,

		//
		class_<FurniInteractRow>("FurniInteractRow")
			.def_readonly("id",&FurniInteractRow::id)
			.def_readonly("imageset",&FurniInteractRow::imageset)
			.def_readonly("icon",&FurniInteractRow::icon)
			.def_readonly("tooltip",&FurniInteractRow::tooltip)
		,

		//
		class_<BigExpressionRow>("BigExpressionRow")
		.def_readonly("id",&BigExpressionRow::id)
		.def_readonly("transferred",&BigExpressionRow::transferred)
		.def_readonly("playtime",&BigExpressionRow::playtime)
		.def_readonly("firstf",&BigExpressionRow::firstf)
		.def_readonly("secondf",&BigExpressionRow::secondf)
		.def_readonly("thirdf",&BigExpressionRow::thirdf)
		.def_readonly("forthf",&BigExpressionRow::forthf)
		.def_readonly("fifthf",&BigExpressionRow::fifthf)
		.def_readonly("sixthf",&BigExpressionRow::sixthf)
		.def_readonly("seventhf",&BigExpressionRow::seventhf)
		.def_readonly("eightf",&BigExpressionRow::eightf)
		.def_readonly("ninthf",&BigExpressionRow::ninthf)
		.def_readonly("tenthf",&BigExpressionRow::tenthf)
		,

		//
		class_<HelpBtnShowPicRow>("HelpBtnShowPicRow")
		.def_readonly("id",&HelpBtnShowPicRow::id)
		.def_readonly("btn_name", &HelpBtnShowPicRow::btn_name)
		.def_readonly("picID", &HelpBtnShowPicRow::picID)
		,
		//
		class_<ShopCenterIconRow>("ShopCenterIconRow")
		.def_readonly("type",&ShopCenterIconRow::type)
		.def_readonly("imageset",&ShopCenterIconRow::imageset)
		.def_readonly("normal",&ShopCenterIconRow::normal)
		.def_readonly("push",&ShopCenterIconRow::push)
		.def_readonly("hover",&ShopCenterIconRow::hover)
		.def_readonly("tooltip",&ShopCenterIconRow::tooltip)
		,

		class_<TaskRow>("TaskRow")
		.def_readonly("id",&TaskRow::id)
		.def_readonly("title",&TaskRow::title)
		.def_readonly("intention",&TaskRow::intention)
		.def_readonly("item_need",&TaskRow::item_need)
		.def_readonly("amount_need",&TaskRow::amount_need)
		.def_readonly("describe",&TaskRow::describe)
		.def_readonly("item_choice",&TaskRow::item_choice)
		.def_readonly("amount_choice",&TaskRow::amount_choice)
		.def_readonly("item_reward",&TaskRow::item_reward)
		.def_readonly("amount_reward",&TaskRow::amount_reward)
		.def_readonly("game_reward",&TaskRow::game_reward)
		.def_readonly("eq_reward",&TaskRow::eq_reward)
		.def_readonly("is_show",&TaskRow::is_show)
		.def_readonly("is_give_up",&TaskRow::is_give_up)
		.def_readonly("sub_id",&TaskRow::sub_id)
		,

		class_<TaskGuideStepRow>("TaskGuideStepRow")
		.def_readonly("id",&TaskGuideStepRow::id)
		.def_readonly("stepNum",&TaskGuideStepRow::stepNum)
		.def_readonly("content",&TaskGuideStepRow::content)
		,

		class_<OpenUIRow>("OpenUIRow")
		.def_readonly("name",&OpenUIRow::name)
		.def_readonly("mutexSets",&OpenUIRow::mutexUISets)
		.def_readonly("relationSets",&OpenUIRow::relationUISets)
		,

		class_<GiftBoxRow>("GiftBoxRow")
		.def_readonly("id",&GiftBoxRow::id)
		.def_readonly("type",&GiftBoxRow::type)
		.def_readonly("eqGold",&GiftBoxRow::eqGold)
		.def_readonly("gameGold",&GiftBoxRow::gameGold)
		.def_readonly("modelId",&GiftBoxRow::modelId)
		.def_readonly("name",&GiftBoxRow::name)
		.def_readonly("imageset",&GiftBoxRow::imageset)
		.def_readonly("image",&GiftBoxRow::image)
		.def_readonly("desc",&GiftBoxRow::desc)
		,

		class_<GiftCardRow>("GiftCardRow")
		.def_readonly("id",&GiftCardRow::id)
		.def_readonly("type",&GiftCardRow::type)
		.def_readonly("eqGold",&GiftCardRow::eqGold)
		.def_readonly("gameGold",&GiftCardRow::gameGold)
		.def_readonly("name",&GiftCardRow::name)
		.def_readonly("imageset",&GiftCardRow::imageset)
		.def_readonly("image",&GiftCardRow::image)
		.def_readonly("desc",&GiftCardRow::desc)
		.def_readonly("urlAddress",&GiftCardRow::urlAddress)
		,

		class_<GotoRow>("GotoRow")
		.def_readonly("id",&GotoRow::id)
		.def_readonly("page",&GotoRow::page)
		.def_readonly("pos",&GotoRow::pos)
		.def_readonly("image",&GotoRow::image)
		.def_readonly("name",&GotoRow::name)
		.def_readonly("target",&GotoRow::target)
		,
		class_<MagicBoxRow>("MagicBoxRow")
		.def_readonly("id",&MagicBoxRow::id)
		.def_readonly("modelId",&MagicBoxRow::modelId)
		,
		// (add by hezhr)
		class_<GoodsDescribeRow>("GoodsDescribeRow")
		.def_readonly("id",&GoodsDescribeRow::id)
		.def_readonly("value",&GoodsDescribeRow::value)
		,
		// (add by hezhr)
		class_<PlayerGuideRow>("PlayerGuideRow")
		.def_readonly("id",&PlayerGuideRow::id)
		.def_readonly("row",&PlayerGuideRow::row)
		.def_readonly("col",&PlayerGuideRow::col)
		,
		// (add by hezhr)
		class_<NoviceTeachStepRow>("NoviceTeachStepRow")
		.def_readonly("step",&NoviceTeachStepRow::step)
		.def_readonly("content",&NoviceTeachStepRow::content)
		.def_readonly("preBtnStr",&NoviceTeachStepRow::preBtnstr)
		.def_readonly("nextBtnstr",&NoviceTeachStepRow::nextBtnstr)
		,
		class_<MutexParentIdxRow>("MutexParentIdxRow")
		.def_readonly("id",&MutexParentIdxRow::id)
		.def_readonly("parent",&MutexParentIdxRow::parent)
		.def_readonly("name",&MutexParentIdxRow::name)
		,

		class_<MutexUIRow>("MutexUIRow")
		.def_readonly("name",&MutexUIRow::name)
		.def_readonly("relation",&MutexUIRow::relation)
		.def_readonly("mutex",&MutexUIRow::mutex)
		,

		class_<VoiceRow>("VoiceRow")
		.def_readonly("id",&VoiceRow::id)
		.def_readonly("cls",&VoiceRow::cls)
		.def_readonly("ltype",&VoiceRow::ltype)
		.def_readonly("file",&VoiceRow::file)
		,

		class_<UIVoiceRow>("UIVoiceRow")
		.def_readonly("id",&UIVoiceRow::id)
		.def_readonly("hovervid",&UIVoiceRow::hovervid)
		.def_readonly("clickvid",&UIVoiceRow::clickvid)
		.def_readonly("name",&UIVoiceRow::name)
		,

		class_<SpecialVoiceRow>("SpecialVoiceRow")
		.def_readonly("id",&SpecialVoiceRow::id)
		.def_readonly("voiceid",&SpecialVoiceRow::voiceid)
		.def_readonly("name",&SpecialVoiceRow::name)
		,

		class_<DiseaseSpecialEventRow>("DiseaseSpecialEventRow")
		.def_readonly("id",&DiseaseSpecialEventRow::id)
		.def_readonly("pid",&DiseaseSpecialEventRow::pid)
		.def_readonly("images",&DiseaseSpecialEventRow::images)
		,

		class_<ChangeWallFloorMaterialRow>("ChangeWallFloorMaterialRow")
		.def_readonly("id",&ChangeWallFloorMaterialRow::id)
		.def_readonly("type",&ChangeWallFloorMaterialRow::type)
		.def_readonly("name",&ChangeWallFloorMaterialRow::name)
		.def_readonly("material",&ChangeWallFloorMaterialRow::material)
		,

		class_<MiniMapRow>("MiniMapRow")
		.def_readonly("scene_id",&MiniMapRow::scene_id)
		.def_readonly("row",&MiniMapRow::row)
		.def_readonly("col",&MiniMapRow::col)
		,

		class_<LoadingRow>("LoadingRow")
		.def_readonly("set",&LoadingRow::set)
		.def_readonly("image",&LoadingRow::image)
		,

		class_<Task>("Task")
		.def(constructor<>())
		.def("requestTask",&Task::requestTask)
		.def("reqGiveup",&Task::reqGiveup)
		.def("reqCompleteTask",&Task::reqCompleteTask)
		.def("existTask",&Task::existTask)
		.def("getTaskCount",&Task::getTaskCount)
		.def("getTaskIDByIndex",&Task::getTaskIDByIndex)
		.def("setSelTask",&Task::setSelTask)
		.def("getNeedCount",&Task::getNeedCount)
		.def("getNeedItem",&Task::getNeedItem)
		.def("getNeedAmount",&Task::getNeedAmount)
		.def("getChoiceCount",&Task::getChoiceCount)
		.def("getChoiceItem",&Task::getChoiceItem)
		.def("getChoiceAmount",&Task::getChoiceAmount)
		.def("getRewardCount",&Task::getRewardCount)
		.def("getRewardItem",&Task::getRewardItem)
		.def("getRewardAmount",&Task::getRewardAmount)
		.def("clearTaskList",&Task::clearTaskList)
		.def("initCeguiTree",&Task::initCeguiTree)
		.def("Task_addTaskGuideTitleTreeItem",&Task::createCeguiTreeItem)
		.def("removeTaskGuideTitleItm",&Task::removeTaskGuideTitleItm)
		.def("getTaskGuideTreeItemCount",	&Task::getTaskGuideTreeItemCount)
		.def("caculateTreeSize",	&Task::caculateTreeSize)
		.def("getTaskActionDesc",	&Task::getTaskActionDesc)
		,

		class_<ItemTableManager>("ItemTableManager")
			.def("getItemRow", &ItemTableManager::getItemRow)
			.def("getItemRowEx", &ItemTableManager::getItemRowEx)
			.def("getItemRowBySubId", &ItemTableManager::getItemRowBySubId)
			.def("getItemEffectTimeRow", &ItemTableManager::getItemEffectTimeRow)
			.def("getItemEquipRow",&ItemTableManager::getItemEquipRow)
			//
			.def("getFashionRow",&ItemTableManager::getFashionRow)
			.def("getFashionRowCount",&ItemTableManager::getFashionRowCount)
			.def("getSysMsgRow",&ItemTableManager::getSysMsgRow)
		,

		//2010-04-26
		class_<HouseTableManager>("HouseTableManager")
			.def("getHouseCompRow",&HouseTableManager::getHouseCompRow)
		,

		class_<LogicTableManager>("LogicTableManager")
			.def("getShopCenterRowByItemID",&LogicTableManager::getShopCenterRowByItemID)
			.def("getNpcShopRowByItemID",&LogicTableManager::getNpcShopRowByItemID)
			.def("getLoopMenuRow",&LogicTableManager::getLoopMenuRow)
			.def("getFurniInteractRow",&LogicTableManager::getFurniInteractRow)
			.def("getShopCenterIconRow",&LogicTableManager::getShopCenterIconRow)
			.def("getFitModeShopCenterIconRow",&LogicTableManager::getFitModeShopCenterIconRow)
			.def("getHouseSalesRow",&LogicTableManager::getHouseSalesRow)
			.def("getDoserviceRow",&LogicTableManager::getDoserviceRow)
			.def("getWorkRow",&LogicTableManager::getWorkRow)
			.def("getWorkEventRow",&LogicTableManager::getWorkEventRow)
			.def("getPlayerPropertyRow",&LogicTableManager::getPlayerPropertyRow)
			.def("getServicePriceRow",&LogicTableManager::getServicePriceRow)
			.def("getOpenUIRow",&LogicTableManager::getOpenUIRow)
			.def("getGiftBoxRow",&LogicTableManager::getGiftBoxRow)	
			.def("getGiftCardRow",&LogicTableManager::getGiftCardRow)
			.def("getGotoRows",&LogicTableManager::getGotoRows)
			.def("getGotoRow",&LogicTableManager::getGotoRow)
			.def("getGotoUserData",&LogicTableManager::getGotoUserData)
			.def("setGotoUserData",&LogicTableManager::setGotoUserData)
			.def("getMagicBoxRows",&LogicTableManager::getMagicBoxRows)
			.def("getMagicBoxRow",&LogicTableManager::getMagicBoxRow)
			.def("getGarbageRows",&LogicTableManager::getGarbageRows)
			.def("getGarbageRow",&LogicTableManager::getGarbageRow)
			.def("getGoodsDescribeRow",&LogicTableManager::getGoodsDescribeRow)
			.def("getPlayerGuideRow",&LogicTableManager::getPlayerGuideRow)
			.def("getPlayerGuideImageset",&LogicTableManager::getPlayerGuideImageset)
			.def("getPlayerGuideImage",&LogicTableManager::getPlayerGuideImage)
			.def("getNoviceTeachStepRow",&LogicTableManager::getNoviceTeachStepRow)
			.def("getMutexParentIdxRow",&LogicTableManager::getMutexParentIdxRow)
			.def("getMutexParentIdxRowN",&LogicTableManager::getMutexParentIdxRowN)
			.def("getMutexParentIdxRows",&LogicTableManager::getMutexParentIdxRows)
			.def("getMutexParentIdxTable",&LogicTableManager::getMutexParentIdxTable)
			.def("getMutexUIRow",&LogicTableManager::getMutexUIRow)
			.def("getMutexUIRows",&LogicTableManager::getMutexUIRows)
			.def("getMutexUITable",&LogicTableManager::getMutexUITable)
			.def("getNpcShopMgrRow",&LogicTableManager::getNpcShopMgrRow)
			.def("getVoiceTable",&LogicTableManager::getVoiceTable)
			.def("getVoiceRows",&LogicTableManager::getVoiceRows)
			.def("getVoiceRow",&LogicTableManager::getVoiceRow)
			.def("getUIVoiceTable",&LogicTableManager::getUIVoiceTable)
			.def("getUIVoiceRows",&LogicTableManager::getUIVoiceRows)
			.def("getUIVoiceRow",&LogicTableManager::getUIVoiceRow)
			.def("getUIVoiceRowByIdx",&LogicTableManager::getUIVoiceRowByIdx)
			.def("getUIVoiceRowByIdxStart",&LogicTableManager::getUIVoiceRowByIdxStart)
			.def("getUIVoiceRowByIdxNext",&LogicTableManager::getUIVoiceRowByIdxNext)
			.def("getSpecialVoiceTable",&LogicTableManager::getSpecialVoiceTable)
			.def("getSpecialVoiceRows",&LogicTableManager::getSpecialVoiceRows)
			.def("getSpecialVoiceRow",&LogicTableManager::getSpecialVoiceRow)
			.def("getDiseaseSpecialEventRows",&LogicTableManager::getDiseaseSpecialEventRows)
			.def("getDiseaseSpecialEventRow",&LogicTableManager::getDiseaseSpecialEventRow)
			.def("getChangeWallFloorMaterialRow",&LogicTableManager::getChangeWallFloorMaterialRow)
			.def("getMiniMapRow",&LogicTableManager::getMiniMapRow)
			.def("getMiniMapImageset",&LogicTableManager::getMiniMapImageset)
			.def("getMiniMapImage",&LogicTableManager::getMiniMapImage)
			.def("getRandomLoadingRow",&LogicTableManager::getRandomLoadingRow)
			.def("getRandomLoadingTips",&LogicTableManager::getRandomLoadingTips)
			.def("getBigExpressionRow",&LogicTableManager::getBigExpressionRow)
			.def("getBigExpressionTableCount",&LogicTableManager::getBigExpressionTableCount)
			.def("getHelpBtnShowPicRow",&LogicTableManager::getHelpBtnShowPicRow)
			.def("getHelpBtnShowPicTableCount",&LogicTableManager::getHelpBtnShowPicTableCount)

		,

		class_<TaskTableManager>("TaskTableManager")
		.def("getTaskRow",&TaskTableManager::getTaskRow)
		.def("getTaskGuideStepRow",&TaskTableManager::getTaskGuideStepRow)
		,

		class_<EQCamera>("EQCamera")
		.def("reset",				&EQCamera::reset)	
		.def("getPitchMin",			&EQCamera::getPitchMin)
		.def("getPitchMax",			&EQCamera::getPitchMax)
		.def("getYawMin",			&EQCamera::getYawMin)
		.def("getYawMax",			&EQCamera::getYawMax)
		.def("getZoomMin",			&EQCamera::getZoomMin)
		.def("getZoomMax",			&EQCamera::getZoomMax)
		.def("getLookAtMin",		&EQCamera::getLookAtMin)
		.def("getLookAtMax",		&EQCamera::getLookAtMax)
		.def("getPitch",			&EQCamera::getPitch)
		.def("getYaw",				&EQCamera::getYaw)
		.def("getZoom",				&EQCamera::getZoom)
		.def("getLookAtX",			&EQCamera::getLookAtX)
		.def("getLookAtY",			&EQCamera::getLookAtY)
		.def("getLookAtZ",			&EQCamera::getLookAtZ)
		.def("pitch",				&EQCamera::pitch)
		.def("yaw",					&EQCamera::yaw)
		.def("zoom",				&EQCamera::zoom)

		//.def("setPitchMin",			&EQCamera::setPitchMin)
		//.def("setPitchMax",			&EQCamera::setPitchMax)
		//.def("setYawMin",			&EQCamera::setYawMin)
		//.def("setYawMax",			&EQCamera::setYawMax)
		//.def("setZoomMin",			&EQCamera::setZoomMin)
		//.def("setZoomMax",			&EQCamera::setZoomMax)
		//.def("setLookAtMin",		&EQCamera::setLookAtMin)
		//.def("setLookAtMax",		&EQCamera::setLookAtMax)
		,

		luabind::def("getDragItem",&DragItem::getSingleton),
		luabind::def("getItemTableManager",&ItemTableManager::getSingleton),
		//2010-04-26
		luabind::def("getHouseTableManager",&HouseTableManager::getSingleton),
		luabind::def("getLogicTableManager",&LogicTableManager::getSingleton),
		luabind::def("getTaskTableManager",&TaskTableManager::getSingleton)
	];

	return true;
}