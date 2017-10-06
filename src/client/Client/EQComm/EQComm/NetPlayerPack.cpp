#include "EQCommPCH.h"

#include "LuaManager.h"
#include "EQMsgManager.h"

#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "ObjMgr.h"
#include "Player.h"
#include "GameState.h"
#include "MainPlayer.h"
#include "ItemPack.h"
#include "ScriptFunction.h"
#include "ItemTable.h"
#include "FitmentInteractMgr.h"
#include "EQGameSceneMgr.h"
#include "luabind/luabind/luabind.hpp"
#include "PlayerHouse.h"
#include "BodyActionMgr.h"
#include "MailManager.h"
#include "PlayerSearch.h"
//-------------------------------------------------------------------------
void EQMsgManager::OnRecePlayerPackage(ByteArray& recBuff)
{
	notify_package pack;

	pack.decode(recBuff);
	
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(pack.account);

	Player* player;

	if(isMainPlayer)
	{
		player = (Player*)MainPlayer::getSingletonPtr();
	}
	else
	{
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, pack.account);
	}

	if(player)
	{
		player->setPack(pack);
	}
	else
	{
		LOGERR("OnRecePlayerPackage 找不到这个玩家: "+pack.account);
	}
	if (!sStateMgr.isFunnyFarm())
	{
		luabind::call_function<void>(sLuaMgr.getL(), "handleEffTimeInMail");	// 处理邮件中物品的时效(hezhr)
		luabind::call_function<void>(sLuaMgr.getL(), "handleEffTimeWhenClicked");	// 物品时效已到，处理点击时的情况(hezhr)
	}
}
//-------------------------------------------------------------------------
bool EQMsgManager::handleByPack()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_around_change_dress:
		{
			notify_around_change_dress data;
			data.decode(mRecebuff);
			
			Player* p;
			if( ObjMgr::getSingleton().isMainPlayer(data.account) )
				p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_MAINPLAYER, data.account);
			else
				p = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.account);
			if(NULL == p)
			{
				LOGERR("msg_notify_around_puton_dress 找不到这个玩家： "+data.account);
				return true;
			}

			if (data.template_id == 0)
			{
				p->takeOff(posToSlot(data.pos));
			}
			else
			{
				const ItemEquipRow* row;
				row=ItemTableManager::getSingleton().getEquipRowByItem(data.template_id);
				p->takeOff(posToSlot(row->equip_pos));
				p->equipOn(posToSlot(row->equip_pos),row->equip_disp_id, row->equip_placeholder);
			}
		}
		break;

	default:
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
void EQMsgManager::recUseFurniture(ByteArray& recBuff)
{
	notify_use_furniture_result data;
	data.decode(recBuff);

	FitmentInteractMgr::getSingleton().recBeginInteractRole(data.account, 
															data.instance_id, 
															data.function_id, 
															data.position_index, 
															data.status);
	FitmentInteractMgr::getSingleton().recBeginInteractFurni(data.instance_id, 
															data.function_id, 
															data.position_index, 
															data.status);
}
//-------------------------------------------------------------------------
void EQMsgManager::recUseMultiFurniture(ByteArray& recBuff)
{
	notify_use_mutli_furni data;
	data.decode(recBuff);

	for(uint i=0; i<data.furni_vec.size(); i++)
	{
		FitmentInteractMgr::getSingleton().recBeginInteractRole(data.furni_vec[i].account, 
																data.furni_vec[i].instance_id, 
																data.furni_vec[i].function_id, 
																data.furni_vec[i].position_index, 
																data.furni_vec[i].status,
																true);
		FitmentInteractMgr::getSingleton().recBeginInteractFurni(data.furni_vec[i].instance_id, 
																data.furni_vec[i].function_id, 
																data.furni_vec[i].position_index, 
																data.furni_vec[i].status);
	}
}
//-------------------------------------------------------------------------
void EQMsgManager::recStopFurniture(ByteArray& recBuff)
{
	notify_stop_using_furniture_result data;
	data.decode(recBuff);

	FitmentInteractMgr::getSingleton().recEndInteract(data.account, data.instance_id, data.function_id, data.position_index, 
														Ogre::Vector3(data.pos.x, data.pos.y, data.pos.z), data.pre_status, data.status);
}
//-------------------------------------------------------------------------
void EQMsgManager::recUpdateCoin(ByteArray& recBuff)
{
	player_coin pack;

	pack.decode(recBuff);

	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(pack.account);

	Player* player;

	if(isMainPlayer)
	{
		player = (Player*)MainPlayer::getSingletonPtr();
	}
	else
	{
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, pack.account);
	}

	if(player)
	{
		player->setGold(pack);
	}
	else
	{
		LOGERR("recUpdateCoin 找不到这个玩家: "+pack.account);
	}
}
//-------------------------------------------------------------------------
void EQMsgManager::recEndBodyAction(ByteArray& recBuff)
{
	notify_end_body_action pack;
	pack.decode(recBuff);
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(pack.player_account);

	Player* player = NULL;

	if(isMainPlayer)
	{
		player = (Player*)MainPlayer::getSingletonPtr();
	}
	else
	{
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, pack.player_account);
	}
	if (player)
	{
		player->getBodyActionMgr()->notifyEndBodyAction(pack.player_account, pack.target_account);
	}

}
//-------------------------------------------------------------------------
void EQMsgManager::recPlayAction(ByteArray& recBuff)
{
	notify_start_body_action pack;
	pack.decode(recBuff);
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(pack.player_account);
	Player * player = isMainPlayer ? MainPlayer::getSingletonPtr():
		(Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, pack.player_account);
	if (player)
	{
		player->getBodyActionMgr()->notifyStartBodyAction(pack.action, pack.player_account, pack.target_account);
	}
}
//-------------------------------------------------------------------------
void EQMsgManager::recBuyFurniture(ByteArray& recBuff)
{
	MainPlayer::getSingleton().clearShoppingCart();

	notify_buy_furniture_list pack;
	pack.decode(recBuff);

	for(int i=0; i<(int)pack.furnitures.size(); i++)
		EQGameSceneMgr::getSingleton().getHouse()->addOneFurniture(&(pack.furnitures[i]));

	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_BuySucess");
}
//-------------------------------------------------------------------------
void EQMsgManager::OnRecePlayerStorage(ByteArray& recBuff)
{
	notify_player_depot storageData;
	storageData.decode(recBuff);

	MainPlayer::getSingleton().setStoragePack(storageData);
}
//-------------------------------------------------------------------------
void EQMsgManager::OnRecePlayerEQInfo(ByteArray& recBuff) 
{
	notify_eq_user_info eqInfo;
	eqInfo.decode(recBuff);

	if(ObjMgr::getSingleton().isMainPlayer(eqInfo.account))
	{
		luabind::call_function<void>(sLuaMgr.getL(), "MainUI_setUserHead", eqInfo.account, eqInfo.info);
	}
	else
	{
		PlayerSearch::getSingleton().addEQInfo(eqInfo.account,eqInfo.info.full_name,eqInfo.info.nick_name,eqInfo.info.region,eqInfo.info.city);
		luabind::call_function<void>(sLuaMgr.getL(), "SetEqInfo", eqInfo.account, eqInfo.info);
	}
}
//-------------------------------------------------------------------------
void EQMsgManager::OnRecePlayerEQSign(ByteArray& recBuff)
{
	notify_eq_user_presence signature;
	signature.decode(recBuff);

	// 目前只设置主玩家的个性签名，其他的暂时不显示。
	if(MainPlayer::getSingleton().getAccount() == signature.account)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "SetMainPlayerSignature", signature.presence);
	}
}
//-------------------------------------------------------------------------
void EQMsgManager::recFlowerCount(ByteArray& recBuff)
{
	notify_total_flower_count flower;
	flower.decode(recBuff);

	MainPlayer::getSingleton().setFlowerCount(flower.flower_count);
}//-------------------------------------------------------------------------
void EQMsgManager::recDecoration(ByteArray& recBuff)
{
	notify_house_decoration decoration;
	decoration.decode(recBuff);

	MainPlayer::getSingleton().setDecoration(decoration.decoration);
}
//-------------------------------------------------------------------------
void EQMsgManager::recFlowerChange(ByteArray& recBuff)
{
	notify_house_flower_change flower;
	flower.decode(recBuff);

	MainPlayer::getSingleton().setFlowerToday(flower);
}
//-------------------------------------------------------------------------
void EQMsgManager::recEggChange(ByteArray& recBuff)
{
	notify_house_egg_change egg;
	egg.decode(recBuff);

	MainPlayer::getSingleton().setEggToday(egg);
}
//-------------------------------------------------------------------------
void EQMsgManager::recFlowerLog(ByteArray& recBuff)
{
	notify_flower_logs flog;
	flog.decode(recBuff);

	MainPlayer::getSingleton().showFlowerLog(flog);
}
//-------------------------------------------------------------------------
void EQMsgManager::recEggLog(ByteArray& recBuff)
{
	notify_egg_logs elog;
	elog.decode(recBuff);

	MainPlayer::getSingleton().showEggLog(elog);
}
//-------------------------------------------------------------------------
void EQMsgManager::recFlowerPlay(ByteArray& recBuff)
{
	notify_send_flower_success info;
	info.decode(recBuff);

	MainPlayer::getSingleton().showFlowerAni(info);
}
//-------------------------------------------------------------------------
void EQMsgManager::recEggPlay(ByteArray& recBuff)
{
	notify_send_egg_success info;
	info.decode(recBuff);

	MainPlayer::getSingleton().showEggAni(info);
}//-------------------------------------------------------------------------
void EQMsgManager::recTodayFlowerEggCount(ByteArray& recBuff)
{
	notify_today_flower_egg_count info;
	info.decode(mRecebuff);

	MainPlayer::getSingleton().showFne(info);
}
//-------------------------------------------------------------------------
void EQMsgManager::recDailyReset(ByteArray& recBuff)
{
	notify_new_day_reset reset;
	reset.decode(recBuff);

	MainPlayer::getSingleton().resetDaily(reset);
}
//-------------------------------------------------------------------------