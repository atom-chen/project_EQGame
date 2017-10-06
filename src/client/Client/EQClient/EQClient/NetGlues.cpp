#include "EQClientPCH.h"
#include "CEGUI.h"
#include "CEGUILua.h"
#include "MessageManager.h"
#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "ObjMgr.h"
#include "Player.h"
#include "MainPlayer.h"
//
#include "Avatar.h"
#include "AvatarRTT.h"
#include "AvatarAdjuster.h"
#include "GameObj.h"
#include "GameMgr.h"
//
#include "GameEventMgr.h"
#include "SmallModules.h"
#include "FitmentInteractMgr.h"
#include "GameState.h"
#include "EQGameSceneMgr.h"
#include "ObjMgr.h"
#include "Timekeeper.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "SmallModules.h"

//------------------------------------------------------------------------------
BOOL CMessageManager::EnterOtherSceneRequest()
{
	req_enter_player_house scene;
	scene.account = "testuser";

	return m_Socket.Send(scene);
}
//------------------------------------------------------------------------------
BOOL CMessageManager::LeaveSceneRequest()
{

	return TRUE;
}
//------------------------------------------------------------------------------
void CMessageManager::PlayerDataReply(ByteArray& recBuff)
{
	notify_player_data data;

	data.decode(recBuff);

	Player* p = (Player*)MainPlayer::getSingletonPtr();

	p->setAccount(data.basic_data.account);
	p->setPlayerData(data.basic_data);
	//p->setSceneName(data.basic_data.scenename);
	Avatar* avatar = p->getAvatar();
	AvatarAdjuster* adj = avatar->getAdjuster();
	adj->doChange(AvatarAdjuster::atSex, data.basic_data.sex);

	p->updateModel();
	avatar = p->getAvatar();

	adj = avatar->getAdjuster();

	adj->doChange(AvatarAdjuster::atHairType, data.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace, data.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.basic_data.beard);

	//RTT更新
	AvatarRTT* rtt = GameMgr::getSingleton().getRTT();
	avatar = rtt->getAvatar();
	adj = avatar->getAdjuster();

	rtt->changeSex(data.basic_data.sex);
	//adj->doChange(AvatarAdjuster::atSex, data.basic_data.sex);
	//rtt->updateModel();
	avatar = rtt->getAvatar();
	adj = avatar->getAdjuster();

	adj->doChange(AvatarAdjuster::atHairType, data.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace, data.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.basic_data.beard);

	// 更新健康状况
	GameEventMgr::getSingleton().fireEvent("UPDATE_DISEASE");

	p->setState(GOS_STAND_IDLE);
}
//------------------------------------------------------------------------------
void CMessageManager::OtherPlayerDataReply(ByteArray& recBuff)
{
	if(sStateMgr.isFunnyFarm() || SmallModules::ifReviewHouseing == true)
		return;

	notify_other_player_data data;

	data.decode(recBuff);

	Player* p = (Player*) ObjMgr::getSingleton().reqestObj(ObjMgr::GOT_PLAYER);
	if(p->getAvatar() == NULL)
	{
		p->createAvatar(data.player.basic_data.sex);
	}

	p->setAccount(data.player.basic_data.account);
	p->setSceneName(data.player.scenename);
	p->setPlayerData(data.player.basic_data);

	Avatar* a = p->getAvatar();
	AvatarAdjuster* adj = a->getAdjuster();
	adj->doChange(AvatarAdjuster::atSex, data.player.basic_data.sex);

	p->updateModel();

	a = p->getAvatar();
	adj = a->getAdjuster();

	adj->doChange(AvatarAdjuster::atHairType, data.player.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.player.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace, data.player.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.player.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.player.basic_data.beard);

	p->setVisible(true);
	p->setHeadUpVisible(true);
	p->setState(GOS_STAND_IDLE);

	if(data.used_furni_inst_id)
	{
		FitmentInteractMgr::getSingleton().recBeginInteractRole(data.player.basic_data.account, 
																data.used_furni_inst_id, 
																data.used_furni_func_id, 
																data.used_furni_index);
	}
	else
	{
		Ogre::Vector3 currpos;
		currpos.x = data.curr_pos.x;
		currpos.y = data.curr_pos.y;
		currpos.z = data.curr_pos.z;

		Ogre::Vector3 tarpos;
		tarpos.x = data.dest_pos.x;
		tarpos.y = data.dest_pos.y;
		tarpos.z = data.dest_pos.z;

		p->setPos(currpos);


		Ogre::Vector3 dis = currpos - tarpos;
		if( dis.length() > GameObj::NORMAL_WALK_SPEED 
			* GameMgr::getSingleton().getTimElapsed())
		{
			p->moveTo(tarpos);
		}
	}
}
//---------------------------------------------------------------------
void CMessageManager::notifyPlayerBodyState(ByteArray& recBuff)
{
	if(sStateMgr.isFunnyFarm())
		return;

	notify_other_body_state data;
	data.decode(recBuff);
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(data.other_account);
	Player * player = isMainPlayer ? MainPlayer::getSingletonPtr():
		(Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.other_account);

	if (player)
	{
		if ((data.body_state == GOS_LIE_IDLE || data.body_state == GOS_SIT_IDLE) 
// 			&& player->getState() == GOS_ANIMATION 
			&& player->getActionState() == BodyActionMgr::AS_DANCE)
		{
			player->stopAnimation();
		}
		player->setState((GameObjState)data.body_state);
	}

}
//---------------------------------------------------------------------
void CMessageManager::PlayerPlayAnimation(ByteArray& recBuff)
{
	if(sStateMgr.isFunnyFarm())
		return;

	notify_play_animation data;
	data.decode(recBuff);
	bool loop = false;
	if (data.loop == 0)
	{
		loop = false;
	}
	else
	{
		loop = true;
	}
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(data.player_account);
	Player * player = isMainPlayer ? MainPlayer::getSingletonPtr():
		(Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.player_account);

	Player* target = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, data.target_account);

	if (player)
	{
		player->playAnimation(data.ani, player->getName(), target ? target->getName() : "",loop);
		// 		setActionState(mAction_state_map[ani]);
	}
}
//------------------------------------------------------------------------------
void CMessageManager::LeaveSceneReply(ByteArray& recBuff)
{
	notify_player_leave_scene scene;
	scene.decode(recBuff);

	GameObj* p;
	p = ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER,scene.account);

	if(p)
	{
		p->setState(GOS_NONE);
		p->setVisible(false);
	}
	else
	{
		LOGERR("找不到玩家: "+scene.account);
	}
}
//-----------------------------------------------------------------------------
void CMessageManager::notifyDayOrNight(ByteArray& recBuff)
{
	notify_day_or_night data;
	data.decode(mRecebuff);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "CreatePlayer_setFirstInflag",true);
}
//------------------------------------------------------------------------------
bool CMessageManager::handleByOthers()
{
	switch (mMsgtype)
	{
	case NetMsgType::msg_notify_day_or_night:
		notifyDayOrNight(mRecebuff);
		break;
	case NetMsgType::msg_notify_player_data:
		PlayerDataReply(mRecebuff);			
		break;
	case NetMsgType::msg_notify_other_player_data:		    
		OtherPlayerDataReply(mRecebuff);
		break;
	case NetMsgType::msg_notify_play_animation:
		PlayerPlayAnimation(mRecebuff);
		break;
	case NetMsgType::msg_notify_other_body_state:
		notifyPlayerBodyState(mRecebuff);
		break;
	case NetMsgType::msg_notify_player_leave_scene:        
		LeaveSceneReply(mRecebuff);          
		break;
	case NetMsgType::msg_notify_start_walk:                 
		StartWalkReply(mRecebuff);             
		break;
	case NetMsgType::msg_notify_stop_walk:                  
		StopWalkReply(mRecebuff);             
		break;
	case NetMsgType::msg_notify_house_data:
		receiveHousedata(mRecebuff);
		break;
	//case NetMsgType::msg_notify_package:
	//	OnRecePlayerPackage(mRecebuff);
	//	break;
	case NetMsgType::msg_notify_use_furniture_result:
		recUseFurniture(mRecebuff);
		break;
	case NetMsgType::msg_notify_use_mutli_furni:
		recUseMultiFurniture(mRecebuff);
		break;
	case NetMsgType::msg_notify_stop_using_furniture_result:
		recStopFurniture(mRecebuff);
		break;
	//case NetMsgType::msg_notify_update_coin:
	//	recUpdateCoin(mRecebuff);
	//	break;
	case NetMsgType::msg_notify_start_body_action:
		recPlayAction(mRecebuff);
		break;
	case NetMsgType::msg_notify_end_body_action:
		recEndBodyAction(mRecebuff);
		break;
	case NetMsgType::msg_notify_buy_furniture_list:
		recBuyFurniture(mRecebuff);
		break;
	case NetMsgType::msg_notify_player_depot:
		OnRecePlayerStorage(mRecebuff);
		break;
	case NetMsgType::msg_notify_eq_user_info:
		OnRecePlayerEQInfo(mRecebuff);
		break;
	case NetMsgType::msg_notify_eq_user_presence:
		OnRecePlayerEQSign(mRecebuff);
		break;
	case NetMsgType::msg_notify_total_flower_count:
		recFlowerCount(mRecebuff);
		break;
	case NetMsgType::msg_notify_house_decoration:
		recDecoration(mRecebuff);
		break;
	case NetMsgType::msg_notify_house_flower_change:
		recFlowerChange(mRecebuff);
		break;
	case NetMsgType::msg_notify_house_egg_change:
		recEggChange(mRecebuff);
		break;
	case NetMsgType::msg_notify_flower_logs:
		recFlowerLog(mRecebuff);
		break;
	case NetMsgType::msg_notify_egg_logs:
		recEggLog(mRecebuff);
		break;
	case NetMsgType::msg_notify_send_flower_success:
		recFlowerPlay(mRecebuff);
		break;
	case NetMsgType::msg_notify_send_egg_success:
		recEggPlay(mRecebuff);
		break;
	case NetMsgType::msg_notify_today_flower_egg_count:
		recTodayFlowerEggCount(mRecebuff);
		break;
	case NetMsgType::msg_notify_new_day_reset:
		recDailyReset(mRecebuff);
		break;
	default:
		return false;
	}

	return true;
}