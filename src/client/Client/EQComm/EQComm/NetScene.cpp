/*******************************************************************
* 作者: XiaoMing
* 时间: 2010-04-23
* 描述: 场景消息处理
********************************************************************/
#include "EQCommPCH.h"

#include "CEGUI.h"
#include "CEGUILua.h"

#include "LuaManager.h"
#include "EQMsgManager.h"
#include "GameEventMgr.h"

#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "Player.h"
#include "GameState.h"
#include "MainPlayer.h"
#include "ItemPack.h"
#include "NetMsgType.h"
#include "PlayerHouse.h"
#include "EQGameSceneMgr.h"
#include "ObjMgr.h"
#include "HouseTable.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "SmallMap.h"

#include "luabind/luabind/luabind.hpp"

//切换场景
//-------------------------------------------------------------------------
bool EQMsgManager::handleByScene()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_open_change_scene_guide:
		GameEventMgr::getSingleton().fireEvent("START_CHANGE_SCENE");
		break;

	case NetMsgType::msg_notify_close_change_scene_guide:
		GameEventMgr::getSingleton().fireEvent("END_CHANGE_SCENE");
		break;

	case NetMsgType::msg_notify_enter_common_scene:			
		{
			notify_enter_common_scene common;
			common.decode(mRecebuff);

			EQGameSceneMgr::getSingleton().changeScene(EQGameSceneMgr::SCENE_COMMON, common.scene_id, common.born_id);

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Goto_OnReqChangeScene",0);
			SmallMapManager::getSingleton().enterCommonScene(common.scene_id);	// (2011/5/10 add by hezhr)
		}
		break;

	case NetMsgType::msg_notify_finish_enter_scene:
		{
			EQGameSceneMgr::getSingleton().endLoading();
		}
		break;

	case NetMsgType::msg_notify_enter_player_house:
		{
			notify_enter_player_house house;
			house.decode(mRecebuff);

			const PlayerHouseRow* row = HouseTableManager::getSingleton().getPlayerHouseRow(house.house_tplt_id);
			assert(row);

			luabind::call_function<void>(sLuaMgr.getL(), "MainUI_saveHouseOwner", house.account);

			EQGameSceneMgr::getSingleton().changeScene(EQGameSceneMgr::SCENE_HOUSE, row->scene_id, house.born_id);

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Goto_OnReqChangeScene",1);

			const std::string gmHead = MainPlayer::getSingleton().getAccount() + "_gm_";
			bool isGMFriend = (house.account.find(gmHead) == 0);
			std::string houseOwner = house.account;
			if (isGMFriend)
				houseOwner = std::string(house.account.begin() + gmHead.size(), house.account.end());
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "SmallMap_setSceneName", houseOwner, 2);	// 设置场景名字(2011/9/30 add by hezhr)
		}
		break;

	case NetMsgType::msg_notify_open_all_light:
		{
			notify_open_all_light light;
			light.decode(mRecebuff);

			EQSceneManager::getSingleton().turnAllLight( light.turn_on!=0  );
		}

	default:
		return false;
	}

	return true;
}


//房屋相关消息
//-------------------------------------------------------------------------
bool EQMsgManager::handleByHouse()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_start_buy_house:
		GameEventMgr::getSingleton().fireEvent("START_BUY_HOUSE");
		break;
	case NetMsgType::msg_notify_end_buy_house:
		GameEventMgr::getSingleton().fireEvent("END_BUY_HOUSE");
		break;
	case NetMsgType::msg_house_furniture:
		break;
	case NetMsgType::msg_house_component_tex:
		break;
	case NetMsgType::msg_house_component_mesh:
		break;
	case NetMsgType::msg_notify_house_data://34
		receiveHousedata(mRecebuff);
		break;
	case NetMsgType::msg_notify_start_edit_house:
		{
			notify_start_edit_house data;
			data.decode(mRecebuff);

			sEventMgr.fireEvent("START_FIT_EDIT");
			//CLuaManager::getSingleton().CallFunction("OnReceiveHandle",
			//	NetMsgType::msg_notify_start_edit_house, 
			//	&data);

			PlayerHouse * house = EQGameSceneMgr::getSingleton().getHouse();
			// 人物回到出生点
			MainPlayer::getSingleton().setPos(house->getBornPos()); 
			// 因为坐标点以客户端为主，需要通知服务端
			MainPlayer::getSingleton().syncPosToServer();

			// 隐藏马桶刷
			MainPlayer::getSingleton().hideTargetPath();

			sGameMgr.getMainCamera()->setFollowMe(false);
		}
		break;
	case NetMsgType::msg_notify_end_edit_house:
		{
			notify_end_edit_house data;
			data.decode(mRecebuff);

			sEventMgr.fireEvent("END_FIT_EDIT");
			//CLuaManager::getSingleton().CallFunction("OnReceiveHandle",
			//	NetMsgType::msg_notify_end_edit_house, 
			//	&data);
			sGameMgr.getMainCamera()->setFollowMe(true);
		}
		break;
	//case NetMsgType::msg_notify_house_editing:
	//	{
	//		notify_house_editing data;
	//		data.decode(recBuff);

	//		//CLuaManager::getSingleton().CallFunction("OnReceiveHandle",
	//		//	NetMsgType::msg_notify_end_edit_house, 
	//		//	&data);
	//	}
	//	break;

	case NetMsgType::msg_notify_add_furniture:
		{
			notify_add_furniture data;
			data.decode(mRecebuff);

			house_furniture fur;
			fur.face		= data.face;
			fur.instance_id = data.instance_id;
			fur.x			= data.x;
			fur.z			= data.z;
			fur.floor		= data.floor;
			fur.height		= data.height;
			fur.template_id = data.template_id;
			fur.del_time	= data.del_time;

			PlayerHouse* house;
			house = EQGameSceneMgr::getSingleton().getHouse();
			house->addOneFurniture(&fur);

		}
		break;

	case NetMsgType::msg_notify_delete_furniture:
		{
			notify_delete_furniture data;
			data.decode(mRecebuff);

			uint64 instance_id = data.instance_id;


			PlayerHouse* house;
			house = EQGameSceneMgr::getSingleton().getHouse();
			house->recoveryOneFurniture(instance_id);

		}
		break;
	case NetMsgType::msg_notify_replace_room_tex:
		{
			room_tex data;
			data.decode(mRecebuff);

			EQGameSceneMgr::getSingleton().getHouse()->changeRoomMaterial(data.floor_id, data.room_id, data.type, data.tex);
		}
		break;

	default:
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
void EQMsgManager::receiveHousedata(ByteArray& recBuff)
{
	notify_house_data house;
	house.decode(recBuff);

	EQGameSceneMgr::getSingleton().recHousedata(house.data.owner, house.data);
}
//-------------------------------------------------------------------------
BOOL EQMsgManager::recoveryReq(int id)
{
	req_recovery_furniture recovery;

	recovery.furniture_id = id;

	return m_Socket.Send(recovery);
}
//-------------------------------------------------------------------------
BOOL EQMsgManager::placedReq(INetPacket& place)
{
	return m_Socket.Send(place);
}

//-------------------------------------------------------------------------
void PlayerHouse::reqRecovery(house_furniture* fit)
{
	if (NULL != fit)
	{
		sGameMgr.getEQMsgManager()->recoveryReq(fit->instance_id);
	}
	else
	{
		LOGERR("reqRecovery() arg fit got NULL");
	}
}
//-------------------------------------------------------------------------
void PlayerHouse::reqPlace(req_placed_furniture& fit)
{
	sGameMgr.getEQMsgManager()->Send((INetPacket&)fit);//placedReq(placed);
	GameEventMgr::getSingleton().fireEvent(game_event[FIT_NEW_ADDED]);

}
//-------------------------------------------------------------------------
void PlayerHouse::reqMove(req_move_furniture& fit)
{
	sGameMgr.getEQMsgManager()->Send((INetPacket&)fit);//placedReq(placed);
	GameEventMgr::getSingleton().fireEvent(game_event[FIT_NEW_ADDED]);
}
//-------------------------------------------------------------------------