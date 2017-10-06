#include "EQCommPCH.h"
#include "SmallModules.h"

#include "EQMsgManager.h"
#include "MainPlayer.h"
#include "NetMsgType.h"
#include "GameEventMgr.h"
#include "luabind/luabind/luabind.hpp"
#include "LogicTable.h"
#include "LuaManager.h"
#include "ObjMgr.h"
#include "EQGameMgr.h"
#include "OtherPlayerRTT.h"
#include "BeautyRTT.h"
#include "BeautyHeadRTT.h"
#include "EQCamera.h"
#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "HouseTable.h"
#include "time.h"
#include "Timekeeper.h"
#include "EnumDef.h"
#include "EQOgreSys.h"
#include "EQGameSceneMgr.h"
#include "GUIManager.h"
#include "OgreCEGUITexture.h"
#include "AvatarTable.h"
#include "AvatarRTT.h"
#include "DynamicUIMgr.h"
#include "GameState.h"

//--------------------------------------------------------------------
template<> SmallModules* Ogre::Singleton<SmallModules>::ms_Singleton = 0;
bool SmallModules::ifReviewHouseing = false;
//--------------------------------------------------------------------
SmallModules::SmallModules(void)
:mGetMoney(0),
mPutMoney(0),
mAnswerType(ANSWER_NONE),
mChangeActive(0),
mChangeCharm(0),
mChangeHealth(0),
mChangeClean(0),
mMoneyRate(100),
mCostMoney(0),
mServiceId(0),
mInitRttFlag(false),
mInitBeautyRTTFlag(false),
mScreenWidth(0),
mScreenHeight(0),
mImagesetFlag(0),
mWorkEventID(0),
mWorkMoney(0),
mEventMoney(0),
mEventItem(0),
mWorkId(0)
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_invitation, NewMsgHandle(SmallModules, handle_invitation));
	Regist(NetMsgType::msg_notify_cancel_invitation, NewMsgHandle(SmallModules, handle_cancel_invitation));
	Regist(NetMsgType::msg_notify_start_trade, NewMsgHandle(SmallModules, handle_start_trade));
	Regist(NetMsgType::msg_notify_update_trade_money, NewMsgHandle(SmallModules, handle_update_trade_money));
	Regist(NetMsgType::msg_notify_other_update_trade_money, NewMsgHandle(SmallModules, handle_other_update_trade_money));
	Regist(NetMsgType::msg_notify_other_put_trade_item, NewMsgHandle(SmallModules, handle_other_put_trade_item));
	Regist(NetMsgType::msg_notify_other_unput_trade_item, NewMsgHandle(SmallModules, handle_other_unput_trade_item));
	Regist(NetMsgType::msg_notify_put_trade_item, NewMsgHandle(SmallModules, handle_put_trade_item));
	Regist(NetMsgType::msg_notify_unput_trade_item, NewMsgHandle(SmallModules, handle_unput_trade_item));
	Regist(NetMsgType::msg_notify_swap_trade_item, NewMsgHandle(SmallModules, handle_swap_trade_item));
	Regist(NetMsgType::msg_notify_other_swap_trade_item, NewMsgHandle(SmallModules, handle_other_swap_trade_item));
	Regist(NetMsgType::msg_notify_sure_trade, NewMsgHandle(SmallModules, handle_sure_trade));
	Regist(NetMsgType::msg_notify_other_sure_trade, NewMsgHandle(SmallModules, handle_other_sure_trade));
	Regist(NetMsgType::msg_notify_cancel_trade, NewMsgHandle(SmallModules, handle_cancel_trade));
	Regist(NetMsgType::msg_notify_trade_success, NewMsgHandle(SmallModules, handle_trade_success));
	Regist(NetMsgType::msg_notify_player_health_status, NewMsgHandle(SmallModules, handle_player_health_status));
	Regist(NetMsgType::msg_notify_buy_house, NewMsgHandle(SmallModules, handle_buy_house));
	Regist(NetMsgType::msg_notify_domestic_result, NewMsgHandle(SmallModules, handle_domestic_result));
	Regist(NetMsgType::msg_notify_end_domestic, NewMsgHandle(SmallModules, handle_end_domestic));
	Regist(NetMsgType::msg_notify_start_domestic, NewMsgHandle(SmallModules, handle_start_domestic));
	Regist(NetMsgType::msg_notify_other_player_info, NewMsgHandle(SmallModules, handle_other_player_info));
	Regist(NetMsgType::msg_notify_house_basic_data, NewMsgHandle(SmallModules,handle_house_basic_data));
	Regist(NetMsgType::msg_notify_house_clean, NewMsgHandle(SmallModules,handle_house_clean));
	Regist(NetMsgType::msg_notify_buy_new_house_success, NewMsgHandle(SmallModules,handle_notify_buy_new_house_success));
	Regist(NetMsgType::msg_notify_clear_house_log, NewMsgHandle(SmallModules,handle_clear_house_log));
	Regist(NetMsgType::msg_notify_set_house_welcome_words, NewMsgHandle(SmallModules,handle_set_house_welcome_words));
	Regist(NetMsgType::msg_notify_set_house_permission, NewMsgHandle(SmallModules,handle_set_house_permission));
	Regist(NetMsgType::msg_notify_get_send_express_vec, NewMsgHandle(SmallModules,handle_get_send_express_vec));
	Regist(NetMsgType::msg_notify_get_send_box_vec, NewMsgHandle(SmallModules,handle_get_send_box_vec));
	Regist(NetMsgType::msg_notify_get_receive_box_vec, NewMsgHandle(SmallModules,handle_get_receive_box_vec));
	Regist(NetMsgType::msg_notify_get_viewed_box_vec, NewMsgHandle(SmallModules,handle_get_viewed_box_vec));
	Regist(NetMsgType::msg_notify_get_receive_express_vec, NewMsgHandle(SmallModules,handle_get_receive_express_vec));
	Regist(NetMsgType::msg_notify_get_viewed_express_vec, NewMsgHandle(SmallModules,handle_get_viewed_express_vec));
	Regist(NetMsgType::msg_notify_work_result, NewMsgHandle(SmallModules, handle_work_result));
	Regist(NetMsgType::msg_notify_end_work, NewMsgHandle(SmallModules, handle_end_work));
	Regist(NetMsgType::msg_notify_start_work, NewMsgHandle(SmallModules, handle_start_work));
	Regist(NetMsgType::msg_notify_start_change_looks, NewMsgHandle(SmallModules, handle_start_change_looks));
	Regist(NetMsgType::msg_notify_change_looks, NewMsgHandle(SmallModules, handle_change_looks));
	Regist(NetMsgType::msg_notify_end_change_looks, NewMsgHandle(SmallModules, handle_end_change_looks));
	Regist(NetMsgType::msg_notify_show_picture, NewMsgHandle(SmallModules, handle_show_picture));
	Regist(NetMsgType::msg_notify_house_list, NewMsgHandle(SmallModules, handle_house_list));
	Regist(NetMsgType::msg_notify_change_house, NewMsgHandle(SmallModules, handle_change_house));
	Regist(NetMsgType::msg_notify_wallow_time, NewMsgHandle(SmallModules, handle_wallow_time));
	Regist(NetMsgType::msg_notify_disease_special_event, NewMsgHandle(SmallModules, handle_disease_special_event));
	Regist(NetMsgType::msg_notify_express_vec, NewMsgHandle(SmallModules, handle_express_vec));
	Regist(NetMsgType::msg_notify_send_express, NewMsgHandle(SmallModules, handle_send_express));
	Regist(NetMsgType::msg_notify_end_preview_house, NewMsgHandle(SmallModules, handle_end_preview_house));
	Regist(NetMsgType::msg_notify_common_scene_state, NewMsgHandle(SmallModules, handle_notify_common_scene_state));
}
//--------------------------------------------------------------------
SmallModules::~SmallModules(void)
{
}
//--------------------------------------------------------------------
void SmallModules::init()
{
	mMyPack.resize(MAX_COUNT, TradePack());
	mOtherPack.resize(MAX_COUNT, TradePack());

}
//--------------------------------------------------------------------
void SmallModules::update()
{
	checkAnswerTime();

	if (sStateMgr.inMainGame())
	{
		// 透露给乐园lua的update函数，很囧，暂时写这里(2011/7/22 add by hezhr)
		luabind::call_function<void>(sLuaMgr.getL(), "Common_updateMainLua");
	}
	if (sStateMgr.isFunnyFarm())
	{
		// 透露给农场lua的update函数，很囧，暂时写这里(2011/7/22 add by hezhr)
		luabind::call_function<void>(sLuaMgr.getL(), "Common_updateFarmLua");
	}

	stime curTime = Timekeeper::getSingleton().getTimeNow();

	if(Timekeeper::getSingleton().calculElapsedSecs(mTimeCur, curTime) > 10)
	{
		if(mSendExpressList.size())
		{
			char iid[20];
			bool changed = false;
			for(int i = mSendExpressList.size() - 1; i >= 0; i --)
			{
				if(Timekeeper::getSingleton().calculElapsedSecs(mSendExpressList[i].send_date, curTime) >= 0)
				{
					iid[0] = 0;
					_ui64toa( mSendExpressList[i].inst_id, iid, 10);
					luabind::call_function<void>(sLuaMgr.getL(), "OnAddUnViewGift", iid);
					changed = true;

					req_send_express_by_subscribe data;
					data.inst_id = mSendExpressList[i].inst_id;
					sGameMgr.getEQMsgManager()->Send(data);

					mRecvExpressMap.insert(std::make_pair(mSendExpressList[i].inst_id, mSendExpressList[i]));
					mSendExpressList.erase(mSendExpressList.begin() + i);
				}
			}

			if(changed)
			{
				luabind::call_function<void>(sLuaMgr.getL(), "OnEndAddUnViewGift");
			}
		}

		mTimeCur = curTime;

		
	}
}

void SmallModules::handle_player_health_status(MsgPack &recBuff)
{
 	notify_player_health_status pack;
 	pack.decode(recBuff);
 
 	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(pack.account);
 	Player* player;
 	if(isMainPlayer)
 	{
 		player = (Player*)MainPlayer::getSingletonPtr();
		if (pack.status != dt_normal) // 如果生病了
		{
			// 运行新手教学任务(生病)
			player->getTask().startNewerTeachTask(15012);		
		}
 	}
 	else
 	{
 		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, pack.account);
 	}
 
 	player->setHealtyState(pack.status);
 	//治愈粒子
 	if(pack.status == 1)
 		player->addParticle(1016);
 
 	if(isMainPlayer)
 		GameEventMgr::getSingleton().fireEvent("UPDATE_DISEASE");
}
//------------------------------------------------------------------------
// 房屋售卖模块
void SmallModules::saveHouseShopData()
{
	const HouseSalesTable ht = LogicTableManager::getSingleton().getHouseSalesTable();
	HouseSalesTable::const_iterator iter = ht.begin();
	const HouseSalesRow *r;
	for(uint i = 1; i <= ht.size(); i++)
	{
		r = &iter->second; 
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addHouseSales",r->indexId, r->type,r->level, r->recommend, r->price, r->cost_flower, i, ht.size());
		iter++;
	}
	// 通知表读取成功
}
//------------------------------------------------------------------------
void SmallModules::reqPreviewHouse(int houseSaleTableId)
{
	// 有售卖表索引ID获得房屋ID
	int houseTpltId = LogicTableManager::getSingleton().getHouseSalesRow(houseSaleTableId)->houseId;

	reqPreviewHouse1(houseTpltId);
}
void SmallModules::reqPreviewHouse1(int houseTpltId)
{
	EQGameSceneMgr::getSingleton().reqPreviewHousedata(houseTpltId);

	previewHouse(houseTpltId);
}
void SmallModules::previewHouse(int houseTpltId)
{
	// 再有模板ID获得场景ID
	const PlayerHouseRow * row = HouseTableManager::getSingleton().getPlayerHouseRow(houseTpltId);

	EQGameSceneMgr::getSingleton().changeScene(EQGameSceneMgr::SCENE_HOUSE_PREVIEW, row->scene_id, row->previewBorn);

	luabind::call_function<void>(sLuaMgr.getL(), "PreviewHouseUI");
}
//------------------------------------------------------------------------
void SmallModules::reqEndPreviewHouse()
{
	EQGameSceneMgr::getSingleton().reqReseeScene();
	//sGameMgr.getMainCamera()->resetCamMovSpeed();
	//sGameMgr.getMainCamera()->setFollowMe(true);
}
void SmallModules::handle_end_preview_house(MsgPack &recBuff)
{
	sGameMgr.getMainCamera()->resetCamMovSpeed();
	sGameMgr.getMainCamera()->setFollowMe(true);
	luabind::call_function<void>(sLuaMgr.getL(), "OnEndPreviewHouse");
	ifReviewHouseing = false;
}
//------------------------------------------------------------------------
void SmallModules::reqBuyHouse(int id)
{
	req_buy_house req;
	req.id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
void SmallModules::reqBuyHouseAdd(int id)
{
	req_buy_house_add req;
	req.id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
void SmallModules::reqBuyHouseReplace(int id, int type)
{
	req_buy_house_replace req;
	req.id = id;
	req.type = type;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
//bool SmallModules::handleByHouseSales(UINT16 msgType, ByteArray &recBuff)
//{
//	switch(msgType)
//	{
//	case NetMsgType::msg_notify_buy_house:
//		{
//			notify_buy_house data;
//			data.decode(recBuff);
//			int result = data.result;
//			break;
//		}
//		break;
//	default: return false;
//	}
//	return true;
//}
void SmallModules::handle_notify_buy_new_house_success(MsgPack &recBuff)
{
	notify_buy_new_house_success data;
	data.decode(recBuff);

	// 运行新手教学任务(第一次成功购买房屋之后)
	MainPlayer::getSingleton().getTask().startNewerTeachTask(15008);	
}

void SmallModules::handle_buy_house(MsgPack &recBuff)
{
	notify_buy_house data;
	data.decode(recBuff);
	
	luabind::call_function<void>(sLuaMgr.getL(), "OnHandleBuyHouse", data.result);
}
//------------------------------------------------------------------------
// 家政模块
void SmallModules::reqDomesticService(int id)
{
	req_domestic_service reqDoService;
	reqDoService.id = id;

	sGameMgr.getEQMsgManager()->Send(reqDoService);
}
//------------------------------------------------------------------------
void SmallModules::reqStartDomestic()
{
	req_start_domestic req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
//bool SmallModules::handleByDoService(UINT16 msgType, ByteArray& recBuff)
//{
//	notify_domestic_result data;
//	notify_end_domestic endDoService;
//
//	switch(msgType)
//	{
//	case NetMsgType::msg_notify_domestic_result:
//		{
//			data.decode(recBuff);
//
//			mServiceId			= data.id;
//			mCostMoney			= data.cost_money;
//			mChangeActive	    = data.change_active;
//			mChangeCharm		= data.change_charm;
//			mChangeHealth		= data.change_health;
//			mChangeClean		= data.change_clean;
//			mMoneyRate			= data.money_rate;   
//			GameEventMgr::getSingleton().fireEvent("POP_HOME_SERVICE");
//			setBlur(true);
//			return true;
//		}
//		break;
//	case NetMsgType::msg_notify_end_domestic:
//		{
//			setBlur(false);
//		}
//		break;
//	case NetMsgType::msg_notify_start_domestic:
//		{
//			notify_start_domestic data;
//			data.decode(recBuff);
//			luabind::call_function<void>(sLuaMgr.getL(), "AddHouseServiceTip");
//		}
//		break;
//	default:
//		return false;
//	}
//	return true;
//}
void SmallModules::handle_domestic_result(MsgPack &recBuff)
{
	notify_domestic_result data;
	data.decode(recBuff);

	mServiceId			= data.id;
	mCostMoney			= data.cost_money;
	mChangeHouseClean   = data.change_house_clean;
	mMoneyRate			= data.money_rate;   
	GameEventMgr::getSingleton().fireEvent("POP_HOME_SERVICE");
	setBlur(true);
}

void SmallModules::handle_end_domestic(MsgPack &recBuff)
{
	setBlur(false);
	luabind::call_function<void>(sLuaMgr.getL(), "CloseAddressBook");
	luabind::call_function<void>(sLuaMgr.getL(), "EndHouseService");
}

void SmallModules::handle_start_domestic(MsgPack &recBuff)
{
	notify_start_domestic data;
	data.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "AddHouseServiceTip");
}
//------------------------------------------------------------------------
void SmallModules::changeValue()
{
	notify_player_property_changed prop;
	prop.active_value = mChangeActive;
	prop.player_charm = mChangeCharm;
	prop.player_clean = mChangeClean;
	prop.player_health = mChangeHealth;
	MainPlayer::getSingleton().getPropertyModule().changeProper(prop);

	// 清洁度
	DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_FURNICLEAN, mChangeHouseClean);

	// 金币
	if(mCostMoney != 0)
	{
		//
		//MainPlayer::getSingleton().getGameGold() = MainPlayer::getSingleton().getGameGold() + mCostMoney;
		//CEGUI::Window* win = DynamicUIMgr::getSingleton().createNumberPaoPao(MainPlayer::getSingletonPtr(), ENPPT_GOLD, mCostMoney);
	}
}
//------------------------------------------------------------------------
void SmallModules::reqEndService(int id)
{
	req_end_domestic req;
	req.id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::saveDoServiceData()
{
	const DomesticServiceTable t = LogicTableManager::getSingleton().getDoServiceTable();
	DomesticServiceTable::const_iterator iter = t.begin();
	const DoServiceRow * r = NULL;

	for(uint i = 1; i <= t.size(); i++)
	{
		r = &iter->second; 
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addServices",r->id, i, t.size());
		iter++;
	}

}
//------------------------------------------------------------------------
void SmallModules::reqOtherPlayer(GetPlayerType type)
{
	req_other_player_info req;
	switch(type)
	{
	case PICKED:
		{
			GameObj* obj = ObjMgr::getSingleton().getPickObj();

			if(ObjMgr::GOT_PLAYER == obj->getType())
			{	
				req.account = ((Player*)obj)->getAccount();
				sGameMgr.getEQMsgManager()->Send(req);
			}
		}
		return;
	}
}
void SmallModules::otherPlayerRttRotate(int degree,int axes)
{
	EQGameMgr::getSingleton().getOtherPlayerRTT()->rotate(degree, axes);
}
//------------------------------------------------------------------------
// 通知其他玩家数据
//bool SmallModules::handleOtherPlayer(UINT16 msgType, ByteArray& recBuff)
//{
//	if(msgType == NetMsgType::msg_notify_other_player_info)
//	{
//		notify_other_player_info data;
//		data.decode(recBuff);
//
//		Player * p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.property.account);
//
//		if( NULL == p)
//		{
//			const uint MAX_COUNT = 8;
//			int equipArray[MAX_COUNT];
//
//			for (size_t i = 0; i < data.body.size(); i++)
//			{
//				equipArray[i] = data.body[i].item_data.template_id;
//				luabind::call_function<void>(sLuaMgr.getL(), "lua_getEquipParam",i, data.body[i].item_data.template_id);
//			}
//			int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
//			if(sex != data.player.basic_data.sex)
//			{
//				EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(data.player.basic_data.sex);
//				EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
//			}
//			EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip1(equipArray);
//			EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));
//
//			OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
//			Avatar*avatar = rtt->getAvatar();
//			AvatarAdjuster* adj = avatar->getAdjuster();
//
//			adj->doChange(AvatarAdjuster::atHairType, data.player.basic_data.hair);
//			adj->doChange(AvatarAdjuster::atHairColor, data.player.basic_data.hair_color);
//			adj->doChange(AvatarAdjuster::atFace,data.player.basic_data.face);
//			adj->doChange(AvatarAdjuster::atSkin, data.player.basic_data.skin_color);
//			adj->doChange(AvatarAdjuster::atBeard, data.player.basic_data.beard);
//			luabind::call_function<void>(sLuaMgr.getL(), "lua_ShowOffLinePlayerInfo",data.property,data.player.basic_data, 2);
//		}
//
//		if(NULL != p)
//		{
//			p->getProperty() = data.property; 
//			p->setPlayerData(data.player.basic_data);
//
//			int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
//			if(sex != p->getSex())
//			{
//				EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(p->getSex());
//				EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
//			}
//			EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip(p);
//			EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));
//
//			OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
//			Avatar*avatar = rtt->getAvatar();
//			AvatarAdjuster* adj = avatar->getAdjuster();
//
//			adj->doChange(AvatarAdjuster::atHairType, p->getPlayerData()->hair);
//			adj->doChange(AvatarAdjuster::atHairColor, p->getPlayerData()->hair_color);
//			adj->doChange(AvatarAdjuster::atFace, p->getPlayerData()->face);
//			adj->doChange(AvatarAdjuster::atSkin, p->getPlayerData()->skin_color);
//			adj->doChange(AvatarAdjuster::atBeard, p->getPlayerData()->beard);
//
//			luabind::call_function<void>(sLuaMgr.getL(), "ShowPlayerInfo", p, 2);
//		}
//		return true;
//	}
//	else
//		return false;
//}

void SmallModules::handle_other_player_info(MsgPack &recBuff)
{
	notify_other_player_info data;
	data.decode(recBuff);

	Player * p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.property.account);

	if( NULL == p)
	{
		const uint MAX_COUNT = 8;
		int equipArray[MAX_COUNT];

		for (size_t i = 0; i < data.body.size(); i++)
		{
			equipArray[i] = data.body[i].item_data.template_id;
			luabind::call_function<void>(sLuaMgr.getL(), "lua_getEquipParam",i, data.body[i].item_data.template_id);
		}
		int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
		if(sex != data.player.basic_data.sex)
		{
			EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(data.player.basic_data.sex);
			EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
		}
		EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip1(equipArray);
		EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));

		OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
		Avatar*avatar = rtt->getAvatar();
		AvatarAdjuster* adj = avatar->getAdjuster();

		adj->doChange(AvatarAdjuster::atHairType, data.player.basic_data.hair);
		adj->doChange(AvatarAdjuster::atHairColor, data.player.basic_data.hair_color);
		adj->doChange(AvatarAdjuster::atFace,data.player.basic_data.face);
		adj->doChange(AvatarAdjuster::atSkin, data.player.basic_data.skin_color);
		adj->doChange(AvatarAdjuster::atBeard, data.player.basic_data.beard);
		luabind::call_function<void>(sLuaMgr.getL(), "lua_ShowOffLinePlayerInfo",data.property,data.player.basic_data, 2);
	}

	if(NULL != p)
	{
		p->getProperty() = data.property; 
		p->setPlayerData(data.player.basic_data);

		int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
		if(sex != p->getSex())
		{
			EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(p->getSex());
			EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
		}
		EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip(p);
		EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));

		OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
		Avatar*avatar = rtt->getAvatar();
		AvatarAdjuster* adj = avatar->getAdjuster();

		adj->doChange(AvatarAdjuster::atHairType, p->getPlayerData()->hair);
		adj->doChange(AvatarAdjuster::atHairColor, p->getPlayerData()->hair_color);
		adj->doChange(AvatarAdjuster::atFace, p->getPlayerData()->face);
		adj->doChange(AvatarAdjuster::atSkin, p->getPlayerData()->skin_color);
		adj->doChange(AvatarAdjuster::atBeard, p->getPlayerData()->beard);

		luabind::call_function<void>(sLuaMgr.getL(), "ShowPlayerInfo", p, 2);
	}
}
//------------------------------------------------------------------------
// 初始化其他玩家的RTT
void SmallModules::initOtherPlayerRTT()
{
	if (false == mInitRttFlag)
	{
		// RTT处理
		EQGameMgr::getSingleton().getOtherPlayerRTT()->startRTT(500, 50000, 500);
		EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->getOgreCam()->setAspectRatio(Ogre::Real((float)98/232));
		EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);

		mInitRttFlag = true;
	}
}
//------------------------------------------------------------------------
// 初始化美容院的RTT
void SmallModules::initBeautyRTT(void)
{
	if (false == mInitBeautyRTTFlag)
	{
		mScreenWidth = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
		mScreenHeight = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
		// 美容院人物胸部至头部的预览
		float w = CEGUI::WindowManager::getSingleton().getWindow("Root/beauty/background/head")->getWidth().asAbsolute(mScreenWidth);
		float h = CEGUI::WindowManager::getSingleton().getWindow("Root/beauty/background/head")->getHeight().asAbsolute(mScreenHeight);
		EQGameMgr::getSingleton().getBeautyRTT()->startRTT(0, 60000, 500);
		EQGameMgr::getSingleton().getBeautyRTT()->getRTTCamera()->getOgreCam()->setAspectRatio(Ogre::Real((float)w/h));
		mInitBeautyRTTFlag = true;
	}
}
//------------------------------------------------------------------------
// 美容院请求开始消息
void SmallModules::reqStartBeauty(void)
{
	req_start_change_looks req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
// 美容院请求退出消息
void SmallModules::reqCancelBeauty(void)
{
	req_cancel_change_looks req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
// 美容院请求结束消息
void SmallModules::reqEndBeauty(void)
{
	req_end_change_looks req;
	AvatarAdjuster *adj = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster();
	req.new_hair = adj->getCurrentVal(AvatarAdjuster::atHairType);
	req.new_hair_color = adj->getCurrentVal(AvatarAdjuster::atHairColor);
	req.new_face = adj->getCurrentVal(AvatarAdjuster::atFace);
	req.new_skin_color = adj->getCurrentVal(AvatarAdjuster::atSkin);
	req.new_beard = adj->getCurrentVal(AvatarAdjuster::atBeard);
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
// 处理美容院消息
//bool SmallModules::handleBeauty(UINT16 msgType, ByteArray& recBuf)
//{
//	switch (msgType)
//	{
//	case NetMsgType::msg_notify_start_change_looks:
//		{
//			Player *p = (Player*)MainPlayer::getSingletonPtr();
//			// 处理胸部到头部RTT
//			EQGameMgr::getSingleton().getBeautyRTT()->changeSex(p->getSex());
//			if (1 == p->getSex())	// 男
//			{
//				EQGameMgr::getSingleton().getBeautyRTT()->rttScale(4*0.96, 4*0.96, 4*0.96);
//				EQGameMgr::getSingleton().getBeautyRTT()->getRTTCamera()->lookAt(Ogre::Vector3(5, 60610, 493));
//			}
//			else if (2 == p->getSex())	//	女
//			{
//				EQGameMgr::getSingleton().getBeautyRTT()->rttScale(4*1.05, 4*1.05, 4*1.05);
//				EQGameMgr::getSingleton().getBeautyRTT()->getRTTCamera()->lookAt(Ogre::Vector3(0, 60610, 500));
//			}
//			BeautyRTT *rtt = EQGameMgr::getSingleton().getBeautyRTT();
//			AvatarAdjuster *adj = rtt->getAvatar()->getAdjuster();
//			adj->doChange(AvatarAdjuster::atHairType, p->getPlayerData()->hair);
//			adj->doChange(AvatarAdjuster::atHairColor, p->getPlayerData()->hair_color);
//			adj->doChange(AvatarAdjuster::atFace, p->getPlayerData()->face);
//			adj->doChange(AvatarAdjuster::atSkin, p->getPlayerData()->skin_color);
//			adj->doChange(AvatarAdjuster::atBeard, p->getPlayerData()->beard);
//			notify_start_change_looks recvData;
//			recvData.decode(recBuf);
//			luabind::call_function<void>(sLuaMgr.getL(), "StartBeauty", recvData.type);
//		}
//		break;
//	case NetMsgType::msg_notify_change_looks:
//		{
//			notify_change_looks recvData;
//			recvData.decode(recBuf);
//			Player *p = (Player*)MainPlayer::getSingletonPtr();
//			if (p->getAccount() != recvData.account)
//			{
//				p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, recvData.account);
//			}
//			if (NULL == p)	// 暂时这样处理（主要为应对在房屋预览场景中，没有找到其他玩家的数据，就返回，囧）
//				break;
//			Avatar *avatar = p->getAvatar();
//			AvatarAdjuster *adj = avatar->getAdjuster();
//			adj->doChange(AvatarAdjuster::atHairType, recvData.new_hair);
//			adj->doChange(AvatarAdjuster::atHairColor, recvData.new_hair_color);
//			adj->doChange(AvatarAdjuster::atFace, recvData.new_face);
//			adj->doChange(AvatarAdjuster::atSkin, recvData.new_skin_color);
//			adj->doChange(AvatarAdjuster::atBeard, recvData.new_beard);
//			p->getPlayerData()->hair = recvData.new_hair;
//			p->getPlayerData()->hair_color = recvData.new_hair_color;
//			p->getPlayerData()->face = recvData.new_face;
//			p->getPlayerData()->skin_color = recvData.new_skin_color;
//			p->getPlayerData()->beard = recvData.new_beard;
//			p->updateModel();
//
//			// RTT也要更新
//			AvatarRTT *rtt = EQGameMgr::getSingleton().getRTT();
//			avatar = rtt->getAvatar();
//			adj = avatar->getAdjuster();
//			adj->doChange(AvatarAdjuster::atHairType, recvData.new_hair);
//			adj->doChange(AvatarAdjuster::atHairColor, recvData.new_hair_color);
//			adj->doChange(AvatarAdjuster::atFace, recvData.new_face);
//			adj->doChange(AvatarAdjuster::atSkin, recvData.new_skin_color);
//			adj->doChange(AvatarAdjuster::atBeard, recvData.new_beard);
//		}
//		break;
//	case NetMsgType::msg_notify_end_change_looks:
//		{
//			luabind::call_function<void>(sLuaMgr.getL(), "CancelBeauty");
//		}
//		break;
//	default:
//		return false;
//	}
//	return true;
//}
void SmallModules::handle_start_change_looks(MsgPack &recBuff)
{
	Player *p = (Player*)MainPlayer::getSingletonPtr();
	// 处理胸部到头部RTT
	EQGameMgr::getSingleton().getBeautyRTT()->changeSex(p->getSex());
	if (1 == p->getSex())	// 男
	{
		EQGameMgr::getSingleton().getBeautyRTT()->rttScale(4*0.96, 4*0.96, 4*0.96);
		EQGameMgr::getSingleton().getBeautyRTT()->getRTTCamera()->lookAt(Ogre::Vector3(5, 60610, 493));
	}
	else if (2 == p->getSex())	//	女
	{
		EQGameMgr::getSingleton().getBeautyRTT()->rttScale(4*1.05, 4*1.05, 4*1.05);
		EQGameMgr::getSingleton().getBeautyRTT()->getRTTCamera()->lookAt(Ogre::Vector3(0, 60610, 500));
	}
	BeautyRTT *rtt = EQGameMgr::getSingleton().getBeautyRTT();
	AvatarAdjuster *adj = rtt->getAvatar()->getAdjuster();
	adj->doChange(AvatarAdjuster::atHairType, p->getPlayerData()->hair);
	adj->doChange(AvatarAdjuster::atHairColor, p->getPlayerData()->hair_color);
	adj->doChange(AvatarAdjuster::atFace, p->getPlayerData()->face);
	adj->doChange(AvatarAdjuster::atSkin, p->getPlayerData()->skin_color);
	adj->doChange(AvatarAdjuster::atBeard, p->getPlayerData()->beard);
	notify_start_change_looks recvData;
	recvData.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "StartBeauty", recvData.type);
}
void SmallModules::handle_change_looks(MsgPack &recBuff)
{
	notify_change_looks recvData;
	recvData.decode(recBuff);
	Player *p = (Player*)MainPlayer::getSingletonPtr();
	if (p->getAccount() != recvData.account)
	{
		p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, recvData.account);
	}
	if (NULL == p)	// 暂时这样处理（主要为应对在房屋预览场景中，没有找到其他玩家的数据，就返回，囧）
		return;
	Avatar *avatar = p->getAvatar();
	AvatarAdjuster *adj = avatar->getAdjuster();
	adj->doChange(AvatarAdjuster::atHairType, recvData.new_hair);
	adj->doChange(AvatarAdjuster::atHairColor, recvData.new_hair_color);
	adj->doChange(AvatarAdjuster::atFace, recvData.new_face);
	adj->doChange(AvatarAdjuster::atSkin, recvData.new_skin_color);
	adj->doChange(AvatarAdjuster::atBeard, recvData.new_beard);
	p->getPlayerData()->hair = recvData.new_hair;
	p->getPlayerData()->hair_color = recvData.new_hair_color;
	p->getPlayerData()->face = recvData.new_face;
	p->getPlayerData()->skin_color = recvData.new_skin_color;
	p->getPlayerData()->beard = recvData.new_beard;
	p->updateModel();

	// RTT也要更新
	AvatarRTT *rtt = EQGameMgr::getSingleton().getRTT();
	avatar = rtt->getAvatar();
	adj = avatar->getAdjuster();
	adj->doChange(AvatarAdjuster::atHairType, recvData.new_hair);
	adj->doChange(AvatarAdjuster::atHairColor, recvData.new_hair_color);
	adj->doChange(AvatarAdjuster::atFace, recvData.new_face);
	adj->doChange(AvatarAdjuster::atSkin, recvData.new_skin_color);
	adj->doChange(AvatarAdjuster::atBeard, recvData.new_beard);
}
void SmallModules::handle_end_change_looks(MsgPack &recBuff)
{
	luabind::call_function<void>(sLuaMgr.getL(), "CancelBeauty");
}
//------------------------------------------------------------------------
// 美容院RTT旋转
void SmallModules::beautyRTTRotate(int degree,int axes)
{
	EQGameMgr::getSingleton().getBeautyRTT()->rotate(degree, axes);
}
//------------------------------------------------------------------------
// 改变美容院RTT的外观
void SmallModules::beautyRTTChangeLooks(BeautyAdjustType adjustType, int val)
{
	AvatarAdjuster *adj = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster();

	switch (adjustType)
	{
	case BAT_HAIR:		//	发型
		adj->doChange(AvatarAdjuster::atHairType, val);
		break;
	case BAT_HAIRCOLOR:	//	发色
		adj->doChange(AvatarAdjuster::atHairColor, val);
		break;
	case BAT_FACE:		//	脸型
		adj->doChange(AvatarAdjuster::atFace, val);
		break;
	case BAT_SKINCOLOR:	//	肤色
		adj->doChange(AvatarAdjuster::atSkin, val);
		break;
	case BAT_BEARD:		//	胡子
		adj->doChange(AvatarAdjuster::atBeard, val);
		break;
	}
}
//------------------------------------------------------------------------
// 获取美容院RTT各部位的值
int SmallModules::getSectionVal(int type, BeautyAdjustType adjustType)
{
	Player *p = (Player*)MainPlayer::getSingletonPtr();
	AvatarAdjuster *adj = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster();
	int val;
	if (1 == type)	//	玩家
	{
		switch (adjustType)
		{
		case BAT_HAIR:		//	发型
			val = p->getPlayerData()->hair;
			break;
		case BAT_HAIRCOLOR:	//	发色
			val = p->getPlayerData()->hair_color;
			break;
		case BAT_FACE:		//	脸型
			val = p->getPlayerData()->face;
			break;
		case BAT_SKINCOLOR:	//	肤色
			val = p->getPlayerData()->skin_color;
			break;
		case BAT_BEARD:		//	胡子
			val = p->getPlayerData()->beard;
			break;
		}
	} 
	else if (2 == type)	//	美容院RTT
	{	
		switch (adjustType)
		{
		case BAT_HAIR:		//	发型
			val = adj->getCurrentVal(AvatarAdjuster::atHairType);
			break;
		case BAT_HAIRCOLOR:	//	发色
			val = adj->getCurrentVal(AvatarAdjuster::atHairColor);
			break;
		case BAT_FACE:		//	脸型
			val = adj->getCurrentVal(AvatarAdjuster::atFace);
			break;
		case BAT_SKINCOLOR:	//	肤色
			val = adj->getCurrentVal(AvatarAdjuster::atSkin);
			break;
		case BAT_BEARD:		//	胡子
			val = adj->getCurrentVal(AvatarAdjuster::atBeard);
			break;
		}
	}
	return val;
}
//------------------------------------------------------------------------
// 设置窗口的图片
void SmallModules::setBeautyChooseUI(std::string winname, BeautyAdjustType adjustType, int val)
{
	float width = CEGUI::WindowManager::getSingleton().getWindow(winname)->getWidth().asAbsolute(mScreenWidth);
	float height = CEGUI::WindowManager::getSingleton().getWindow(winname)->getHeight().asAbsolute(mScreenHeight);
	int sex = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster()->getSex();
	AvatarAdjuster *adj = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster();
	// 初始化（不变）
	BeautyHeadRTT *beautyHead = new BeautyHeadRTT;
	beautyHead->startRTT(0, 70500, 500);
	beautyHead->getRTTCamera()->getOgreCam()->setAspectRatio(Ogre::Real((float)width/height));
	beautyHead->rotate(20, 2);	// 转20度
	if (beautyHead->getAvatar()->getAdjuster()->getSex() != sex)
	{
		beautyHead->changeSex(sex);
		beautyHead->rotate(20, 2);
	}
	if (1 == sex)	// 男
	{
		beautyHead->rttScale(5*0.96, 5*0.96, 5*0.96);
	}
	else if (2 == sex)	//	女
	{
		beautyHead->rttScale(5*1.05, 5*1.05, 5*1.05);
	}
	beautyHead->getRTTCamera()->lookAt(Ogre::Vector3(0, 71295, 500));
	AvatarAdjuster *headAdj = beautyHead->getAvatar()->getAdjuster();
	headAdj->doChange(AvatarAdjuster::atHairType, adj->getCurrentVal(AvatarAdjuster::atHairType));
	headAdj->doChange(AvatarAdjuster::atHairColor, adj->getCurrentVal(AvatarAdjuster::atHairColor));
	headAdj->doChange(AvatarAdjuster::atFace, adj->getCurrentVal(AvatarAdjuster::atFace));
	headAdj->doChange(AvatarAdjuster::atSkin, adj->getCurrentVal(AvatarAdjuster::atSkin));
	headAdj->doChange(AvatarAdjuster::atBeard, adj->getCurrentVal(AvatarAdjuster::atBeard));
	//	改变人物形象
	switch (adjustType)
	{
	case BAT_HAIR:		//	发型
		headAdj->doChange(AvatarAdjuster::atHairType, val);
		break;
	case BAT_HAIRCOLOR:	//	发色
		headAdj->doChange(AvatarAdjuster::atHairColor, val);
		break;
	case BAT_FACE:		//	脸型
		headAdj->doChange(AvatarAdjuster::atFace, val);
		break;
	case BAT_SKINCOLOR:	//	肤色
		headAdj->doChange(AvatarAdjuster::atSkin, val);
		break;
	case BAT_BEARD:		//	胡子
		headAdj->doChange(AvatarAdjuster::atBeard, val);
		break;
	}
	//	更新渲染RTT纹理
	beautyHead->getOgreRenderTexture()->update(true);
	//	创建texture
	CEGUI::Texture *tex = CEGUI::System::getSingleton().getRenderer()->createTexture(96);
	CEGUI::OgreCEGUITexture *ogreTex = (CEGUI::OgreCEGUITexture*)tex;
	ogreTex->setOgreTexture(beautyHead->getOgreTexturePtr());
	delete beautyHead;
	// 创建imageset
	char buf[28] = "";
	if (8 == mImagesetFlag)
	{
		mImagesetFlag = 0;
	}
	_snprintf_s(buf, sizeof(buf), "%d", mImagesetFlag);
	++mImagesetFlag;
	std::string imagesetBaseName = "BeautyHeadRttImageset_";
	std::string imagesetName = imagesetBaseName + buf;
	CEGUI::Imageset *imageset =  CEGUI::ImagesetManager::getSingleton().createImageset(imagesetName, tex);
	int w = tex->getWidth(), h = tex->getHeight();
	imageset->defineImage("image", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(w, h), CEGUI::Point(0.0f,0.0f));
	// 粘贴图片
	CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	win->setProperty("Image", CEGUI::PropertyHelper::imageToString(&imageset->getImage("image")));
}
//------------------------------------------------------------------------
// 清除动态创建的imageset和window
void SmallModules::clearBeautyChooseUI(void)
{
	std::string imagesetBaseName = "BeautyHeadRttImageset_";
	std::string winBaseName = "beanty_parts_list/backgroundimg/part";
	for (int i=0; i<8; i++)
	{
		char buf[28] = "";
		_snprintf_s(buf, sizeof(buf), "%d", i+1);
		std::string winName = winBaseName + buf;
		CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winName);
		win->setProperty("Image", win->getPropertyDefault("Image"));
		_snprintf_s(buf, sizeof(buf), "%d", i);
		std::string imagesetName = imagesetBaseName + buf;
		if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imagesetName))
		{
			CEGUI::ImagesetManager::getSingleton().destroyImageset(imagesetName);
		}
	}
}
//------------------------------------------------------------------------
// 保存不同美容部位的类型的数量
void SmallModules::saveBeautySectionTypes(BeautySectionType sectionType, int adjustType, BeautyMoneyType moneyType)
{
	// 取某种类型所有部件
	RoleSectionRows roleRows;
	int roleRaceID = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getRoleRaceInfo()->id;
	int color;
	switch (sectionType)
	{
	case BST_HEAD:	//	头部
		{
			color = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster()->getCurrentVal(AvatarAdjuster::atSkin);
			switch (moneyType)
			{
			case BMT_EQ:
				mFaceVec_EQ.clear();
				break;
			case BMT_GOLD:
				mFaceVec_GOLD.clear();
				break;
			}
		}
		break;
	case BST_HAIR:	//	头发
		{
			color = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster()->getCurrentVal(AvatarAdjuster::atHairColor);
			switch (moneyType)
			{
			case BMT_EQ:
				mHairVec_EQ.clear();
				break;
			case BMT_GOLD:
				mHairVec_GOLD.clear();
				break;
			}
		}
		break;
	case BST_BEARD:	//	胡子
		{
			color = EQGameMgr::getSingleton().getBeautyRTT()->getAvatar()->getAdjuster()->getCurrentVal(AvatarAdjuster::atHairColor);
			switch (moneyType)
			{
			case BMT_EQ:
				mBeardVec_EQ.clear();
				break;
			case BMT_GOLD:
				mBeardVec_GOLD.clear();
				break;
			}
		}
		break;
	}

	AvatarTableManager::getSingleton().getRoleSectionRows(roleRaceID, sectionType, color, roleRows, true, false);

	// 区分两种收费类型和三种不同部位
	for (size_t i=0; i<roleRows.size(); ++i)
	{
		const ChangeLooksRow *looksRows = AvatarTableManager::getSingleton().getChangeLooksRow(roleRows[i]->gender, adjustType, i+1);
		if (NULL == looksRows)
			return;

		switch (looksRows->money_type)
		{
		case BMT_EQ:
			{
				switch (sectionType)
				{
				case BST_HEAD:
					mFaceVec_EQ.push_back(i+1);
					break;
				case BST_HAIR:
					mHairVec_EQ.push_back(i+1);
					break;
				case BST_BEARD:
					mBeardVec_EQ.push_back(i+1);
					break;
				}
			}
			break;
		case BMT_GOLD:
			{
				switch (sectionType)
				{
				case BST_HEAD:
					mFaceVec_GOLD.push_back(i+1);
					break;
				case BST_HAIR:
					mHairVec_GOLD.push_back(i+1);
					break;
				case BST_BEARD:
					mBeardVec_GOLD.push_back(i+1);
					break;
				}
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
//	保存发色和肤色的数量
void SmallModules::saveBeautyColors(int sex, BeautySectionType sectionType, BeautyMoneyType moneyType)
{
	BeautyAdjustType adjustType = BAT_NONE;
	AvatarTableManager::TColors colVec;
	switch (sectionType)
	{
	case BST_HEAD:	//	头部
		{
			switch (moneyType)
			{
			case BMT_EQ:
				mSkinColorVec_EQ.clear();
				break;
			case BMT_GOLD:
				mSkinColorVec_GOLD.clear();
				break;
			}
			colVec = AvatarTableManager::getSingleton().getSkinColors(sectionType);
			adjustType = BAT_SKINCOLOR;
		}
		break;
	case BST_HAIR:
	case BST_BEARD:	// 头发，胡子
		{
			switch (moneyType)
			{
			case BMT_EQ:
				mHairColorVec_EQ.clear();
				break;
			case BMT_GOLD:
				mHairColorVec_GOLD.clear();
				break;
			}
			colVec = AvatarTableManager::getSingleton().getHairColors(sectionType);
			adjustType = BAT_HAIRCOLOR;
		}
		break;
	}

	for (size_t i=0; i<colVec.size(); ++i)
	{
		const ChangeLooksRow *looksRows = AvatarTableManager::getSingleton().getChangeLooksRow(sex, adjustType, i+1);
		if (NULL == looksRows)
			return;

		switch (looksRows->money_type)
		{
		case BMT_EQ:	//	EQ点
			{
				switch (sectionType)
				{
				case BST_HEAD:
					mSkinColorVec_EQ.push_back(i+1);
					break;
				case BST_HAIR:
				case BST_BEARD:
					mHairColorVec_EQ.push_back(i+1);
					break;
				}
			}
			break;
		case BMT_GOLD:	//	游戏币
			{
				switch (sectionType)
				{
				case BST_HEAD:
					mSkinColorVec_GOLD.push_back(i+1);
					break;
				case BST_HAIR:
				case BST_BEARD:
					mHairColorVec_GOLD.push_back(i+1);
					break;
				}
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
int SmallModules::getBeautyTypes(BeautyAdjustType adjustType, BeautyMoneyType moneyType)
{
	int dataCount = 0;
	switch (moneyType)
	{
	case BMT_EQ:	//	EQ点
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				dataCount = mHairVec_EQ.size();
				break;
			case BAT_HAIRCOLOR:
				dataCount = mHairColorVec_EQ.size();
				break;
			case BAT_FACE:
				dataCount = mFaceVec_EQ.size();
				break;
			case BAT_SKINCOLOR:
				dataCount = mSkinColorVec_EQ.size();
				break;
			case BAT_BEARD:
				dataCount = mBeardVec_EQ.size();
				break;
			}
		}
		break;
	case BMT_GOLD:	//	游戏币
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				dataCount = mHairVec_GOLD.size();
				break;
			case BAT_HAIRCOLOR:
				dataCount = mHairColorVec_GOLD.size();
				break;
			case BAT_FACE:
				dataCount = mFaceVec_GOLD.size();
				break;
			case BAT_SKINCOLOR:
				dataCount = mSkinColorVec_GOLD.size();
				break;
			case BAT_BEARD:
				dataCount = mBeardVec_GOLD.size();
				break;
			}
		}
		break;
	}
	return dataCount;
}
//------------------------------------------------------------------------
// 插入当前数据
void SmallModules::insertCurVal(BeautyAdjustType adjustType, BeautyMoneyType moneyType, int val)
{
	switch (moneyType)
	{
	case BMT_EQ:	//	EQ点
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				_insertVal(mHairVec_EQ, val);
				break;
			case BAT_HAIRCOLOR:
				_insertVal(mHairColorVec_EQ, val);
				break;
			case BAT_FACE:
				_insertVal(mFaceVec_EQ, val);
				break;
			case BAT_SKINCOLOR:
				_insertVal(mSkinColorVec_EQ, val);
				break;
			case BAT_BEARD:
				_insertVal(mBeardVec_EQ, val);
				break;
			}
		}
		break;
	case BMT_GOLD:	//	游戏币
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				_insertVal(mHairVec_GOLD, val);
				break;
			case BAT_HAIRCOLOR:
				_insertVal(mHairColorVec_GOLD, val);
				break;
			case BAT_FACE:
				_insertVal(mFaceVec_GOLD, val);
				break;
			case BAT_SKINCOLOR:
				_insertVal(mSkinColorVec_GOLD, val);
				break;
			case BAT_BEARD:
				_insertVal(mBeardVec_GOLD, val);
				break;
			}
		}
		break;
	}
}
//------------------------------------------------------------------------
void SmallModules::_insertVal(std::vector<int>& vec, int val)
{
	for (size_t i=0; i<vec.size(); ++i)
	{
		if (val == vec[i])
			return;
	}
	vec.push_back(val);
}
//------------------------------------------------------------------------
// 清除美容部位数据
void SmallModules::clearBeautySectionTypes(void)
{
	mHairVec_EQ.clear();
	mHairVec_GOLD.clear();
	mHairColorVec_EQ.clear();
	mHairColorVec_GOLD.clear();
	mFaceVec_EQ.clear();
	mFaceVec_GOLD.clear();
	mSkinColorVec_EQ.clear();
	mSkinColorVec_GOLD.clear();
	mBeardVec_EQ.clear();
	mBeardVec_GOLD.clear();
}
//------------------------------------------------------------------------
// 获取页数
int SmallModules::getBeautyPages(int total, int maxCount)
{
	int pages = 0;
	// 计算页数
	if (total/maxCount < 1)
	{
		pages = 1;
	}
	else
	{
		if (total%maxCount == 0)
			pages = total/maxCount;
		else
			pages = total/maxCount + 1;
	}
	return pages;
}
//------------------------------------------------------------------------
// 获取不同美容部位的不同类型的值
int SmallModules::getBeautySectionVal(BeautyAdjustType adjustType, int index, BeautyMoneyType moneyType)
{
	int val = 0;
	switch (moneyType)
	{
	case BMT_EQ:	//	EQ点
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				val = mHairVec_EQ[index];
				break;
			case BAT_HAIRCOLOR:
				val = mHairColorVec_EQ[index];
				break;
			case BAT_FACE:
				val = mFaceVec_EQ[index];
				break;
			case BAT_SKINCOLOR:
				val = mSkinColorVec_EQ[index];
				break;
			case BAT_BEARD:
				val = mBeardVec_EQ[index];
				break;
			}
		}
		break;
	case BMT_GOLD:	//	游戏币
		{
			switch (adjustType)
			{
			case BAT_HAIR:
				val = mHairVec_GOLD[index];
				break;
			case BAT_HAIRCOLOR:
				val = mHairColorVec_GOLD[index];
				break;
			case BAT_FACE:
				val = mFaceVec_GOLD[index];
				break;
			case BAT_SKINCOLOR:
				val = mSkinColorVec_GOLD[index];
				break;
			case BAT_BEARD:
				val = mBeardVec_GOLD[index];
				break;
			}
		}
		break;
	}
	return val;
}
//------------------------------------------------------------------------
// 获取相关类型的费用
int SmallModules::getBeautySectionMoney(int sex, BeautyAdjustType adjustType, int val)
{
	const ChangeLooksRow *looksRows = AvatarTableManager::getSingleton().getChangeLooksRow(sex, adjustType, val);
	if (NULL == looksRows)
		return -1;
	return looksRows->money;
}
//------------------------------------------------------------------------
//	处理show图消息
//bool SmallModules::hangdePlayerGuide(UINT16 msgType, ByteArray& recBuf)
//{
//	if (NetMsgType::msg_notify_show_picture == msgType)
//	{
//		notify_show_picture recvData;
//		recvData.decode(recBuf);
//		luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_ShowPicture", recvData.id);
//		return true;
//	}
//	return false;
//}
void SmallModules::handle_show_picture(MsgPack &recBuff)
{
	notify_show_picture recvData;
	recvData.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_ShowPicture", recvData.id);
}
//------------------------------------------------------------------------
std::string SmallModules::getTimeString(stime time)
{
	return getTimeString(time.year, time.month, time.day, time.hour, time.minute, time.second);
}
//------------------------------------------------------------------------
std::string SmallModules::getTimeString(int y, int m, int d, int h, int min, int s)
{
	time_t curTime = time(NULL);	
	time_t rawtime;
	struct tm * target_time;
	char buyTime[20];

	time ( &rawtime );
	target_time = localtime( &rawtime );
	// 今天所走过的秒数
	int totalTime = target_time->tm_hour * 60 * 60 + target_time->tm_min * 60 + target_time->tm_sec;

	target_time->tm_year = y - 1900;
	target_time->tm_mon= m - 1;     
	target_time->tm_mday = d ;  
	target_time->tm_hour = h ;  
	target_time->tm_min = min ; 
	target_time->tm_sec =  s ;  
	rawtime = mktime (target_time); 

	int64 dif = static_cast<int64>(difftime(curTime, rawtime));
	std::string result;

	if( totalTime <= dif && 60 * 60 * 24 + totalTime >= dif )
	{
		sprintf(buyTime, "昨天 %02d:%02d", h, min);

	}
	else if(totalTime >= dif && 0 <= dif)
	{
		sprintf(buyTime, "今天 %02d:%02d", h, min);
	}
	else
	{
		sprintf(buyTime, "%04d-%02d-%02d %02d:%02d", y, m, d, h, min);
	}
	return result = buyTime;
}
//------------------------------------------------------------------------
void SmallModules::saveWorks()
{
	WorksTable t = LogicTableManager::getSingleton().getWorksTable();
	WorksTable::const_iterator r = t.begin();

	for(uint i = 0; i < t.size(); i++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "SaveWorksTable", r->second.id, i, t.size());
		r++;
	}
}
//------------------------------------------------------------------------
void SmallModules::reqStartWork() const
{
	req_start_work req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::reqWork(uint id) const
{
	req_work req;
	req.id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::reqEndWork(uint id) const
{
	req_end_work req;
	req.id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
//bool SmallModules::handleByWorks(UINT16 msgType, ByteArray &recBuff)
//{
//	notify_work_result workResult;
//	notify_end_work endWork;
//
//	switch (msgType)
//	{
//	case NetMsgType::msg_notify_work_result:
//		{
//			workResult.decode(recBuff);
//			mWorkId				= workResult.id;
//			mWorkEventID		= workResult.event_id;
//			mWorkMoney			= workResult.work_money;
//			mChangeActive		= workResult.change_active;
//			mChangeHouseClean	= workResult.change_house_clean;
//			mChangeCharm		= workResult.change_charm;
//			mChangeHealth		= workResult.change_health;
//			mChangeClean		= workResult.change_clean;
//			mEventMoney			= workResult.reward_money;
//			mEventItem			= workResult.reward_item;
//			GameEventMgr::getSingleton().fireEvent("POP_WORK_UI");
//			setBlur(true);
//		}
//		break;
//	case NetMsgType::msg_notify_end_work:
//		{
//			endWork.decode(recBuff);
//			luabind::call_function<void>(sLuaMgr.getL(), "CloseWorkUI");
//			setBlur(false);
//		}
//		break;
//	case NetMsgType::msg_notify_start_work:
//		{
//			luabind::call_function<void>(sLuaMgr.getL(), "PopWorkChooseUI");		
//		}
//		break;
//	default:return false;
//	}
//	return true;
//}

void SmallModules::handle_work_result(MsgPack &recBuff)
{
	notify_work_result workResult;
	workResult.decode(recBuff);
	mWorkId				= workResult.id;
	mWorkEventID		= workResult.event_id;
	mWorkMoney			= workResult.work_money;
	mChangeActive		= workResult.change_active;
	mChangeCharm		= workResult.change_charm;
	mChangeHealth		= workResult.change_health;
	mChangeClean		= workResult.change_clean;
	mEventMoney			= workResult.reward_money;
	mEventItem			= workResult.reward_item;
	GameEventMgr::getSingleton().fireEvent("POP_WORK_UI");
	setBlur(true);
}

void SmallModules::handle_end_work(MsgPack &recBuff)
{
	notify_end_work endWork;
	endWork.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "CloseWorkUI");
	setBlur(false);
}

void SmallModules::handle_start_work(MsgPack &recBuff)
{
	luabind::call_function<void>(sLuaMgr.getL(), "PopWorkChooseUI");
}
//------------------------------------------------------------------------
void SmallModules::setBlur(bool state) const
{
	EQOgreSys::getSingleton().setGaussianBlur(state);
}

size_t SmallModules::getHouseSize()
{
	size_t cnt = 0;

	for(size_t i = 0; i < mHouseDataList.size(); i ++)
	{
		if(0 == mHouseDataList[i].empty)
			cnt ++;
	}

	return cnt;

}

size_t SmallModules::getFHouseSize()
{
	size_t cnt = 0;

	for(size_t i = 0; i < mFHouseDataList.size(); i ++)
	{
		if(0 == mFHouseDataList[i].empty)
			cnt ++;
	}

	return cnt;

}
//------------------------------------------------------------------------
void SmallModules::handle_wallow_time(MsgPack &recBuff)
{
	notify_wallow_time data;
	data.decode(recBuff);

	luabind::call_function<void>(sLuaMgr.getL(), "WallowTime_handle", data.wallow_seconds);	// 防沉迷处理
}
//------------------------------------------------------------------------
void SmallModules::handle_disease_special_event(MsgPack &recBuff)
{
	notify_disease_special_event data;
	data.decode(recBuff);

	luabind::call_function<void>(sLuaMgr.getL(), "OnDiseaseSpecialEvent", data.special_event_id);
}

void SmallModules::handle_express_vec( MsgPack &recBuff )
{
	notify_express_vec data;
	data.decode(recBuff);

	mSendExpressList.clear();

	for(std::vector<gift_express>::iterator itr = data.send_express_vec.begin(); itr != data.send_express_vec.end(); ++itr)
	{
		mSendExpressList.push_back(*itr);
	}

	bool hasExpressGift = false;
	char iid[20];
	luabind::call_function<void>(sLuaMgr.getL(), "OnStartAddUnViewGift");
	for(std::vector<gift_express>::iterator itr = data.receive_express_vec.begin(); itr != data.receive_express_vec.end(); ++itr)
	{
		iid[0] = 0;
		_ui64toa( itr->inst_id, iid, 10);
		luabind::call_function<void>(sLuaMgr.getL(), "OnAddUnViewGift", iid);

		mRecvExpressMap.insert(std::make_pair(itr->inst_id, *itr));
		if (itr->inst_id > 0)
			hasExpressGift = true;
	}
	luabind::call_function<void>(sLuaMgr.getL(), "OnEndAddUnViewGift");

	mTimeCur = Timekeeper::getSingleton().getTimeNow();
	if (hasExpressGift)
	{
		// 运行新手教学任务(第一次收到快递礼物)
		MainPlayer::getSingleton().getTask().startNewerTeachTask(15003);
	}
}

void SmallModules::handle_send_express( MsgPack &recBuff )
{
	notify_send_express data;
	data.decode(recBuff);
	// 运行新手教学任务(第一次收到快递礼物)
	MainPlayer::getSingleton().getTask().startNewerTeachTask(15003);

		
	mSendExpressList.push_back(data.send_express);
}
//--------------------------------------------
void SmallModules::handle_notify_common_scene_state(MsgPack &recBuff)
{
	notify_common_scene_state data;
	data.decode(recBuff);
	CEGUI::Combobox* comboBox = dynamic_cast<CEGUI::Combobox*>(CEGUI::WindowManager::getSingleton().getWindow("Goto/Windows/Tab/Tab1/"));
	comboBox->resetList();
	comboBox->getEditbox()->setText("");
	size_t list_sz = data.scene_list.size();
	if (!list_sz)
	{
		return;
	}
	for (size_t i = 0; i < list_sz; ++i)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Goto_setSceneState",	i,
																			data.scene_list[i].scene_id,
																			data.scene_list[i].copy_id,
																			data.scene_list[i].copy_name,
																			data.scene_list[i].status);
	}
	luabind::call_function<void>(sLuaMgr.getL(), "Goto_setDefSelLine");
}
//----
bool SmallModules::OnLineListSelectionAccepted(const CEGUI::EventArgs& args)
{
// 	luabind::call_function<void>(sLuaMgr.getL(), "OnLineListSelectionAccepted",args);
	return true;
}