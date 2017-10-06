#include "EQCommPCH.h"
#include "EQLogin.h"

#include "EQMsgManager.h"
#include "EQGameMgr.h"

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
#include "EQGameMgr.h"
//
#include "GameEventMgr.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

#include "CEGUI.h"
#include "CommFunc.h"
#include "ExceptionHandler.h"

//--------------------------------------------------------------------
template<> EQLogin* Ogre::Singleton<EQLogin>::ms_Singleton = 0;
//--------------------------------------------------------------------

BOOL EQLogin::LoginRequest()
{
	return TRUE;
}
//------------------------------------------------------------------------------
BOOL EQLogin::CreatPlayerRequest()
{		
	req_create_role reqRegist;
	const int MAX_COUNT = 8;
	MainPlayer * mainplayer = MainPlayer::getSingletonPtr();
	// 	GameObj* gameObj = (GameObj*)mainplayer;

	// 	Avatar * avatar = mainplayer->getAvatar();
	AvatarAdjuster* ava = EQGameMgr::getSingleton().getRTT()->mAvatar->getAdjuster();
	item it[MAX_COUNT];

	it[0].template_id = mainplayer->mCreateEquip.curHatEquip;
	it[1].template_id = mainplayer->mCreateEquip.curClothEquip;
	it[2].template_id = mainplayer->mCreateEquip.curGlassesEquip;
	it[3].template_id = mainplayer->mCreateEquip.curTrouserEquip;
	it[4].template_id = mainplayer->mCreateEquip.curBackEquip;
	it[5].template_id = mainplayer->mCreateEquip.curShoesEquip;
	it[6].template_id = mainplayer->mCreateEquip.curHandEquip;
	it[7].template_id = mainplayer->mCreateEquip.curTrinketEquip;

	for (int i = 0; i<MAX_COUNT;i++)
	{
		reqRegist.equips.push_back(it[i]);
	}

	reqRegist.basic_data.account    = mAccount;
	reqRegist.basic_data.username = mUsername;
	reqRegist.basic_data.sex = ava->getCurrentVal(ava->atSex);
	reqRegist.basic_data.skin_color = ava->getCurrentVal(ava->atSkin);
	reqRegist.basic_data.face = ava->getCurrentVal(ava->atFace);
	reqRegist.basic_data.beard = ava->getCurrentVal(ava->atBeard);
	reqRegist.basic_data.hair = ava->getCurrentVal(ava->atHairType);
	reqRegist.basic_data.hair_color = ava->getCurrentVal(ava->atHairColor);
	reqRegist.basic_data.online_time = 0.0f;

	return sGameMgr.getEQMsgManager()->Send(reqRegist);
}
//------------------------------------------------------------------------------
BOOL EQLogin::VersionRequest()
{
	GameEventMgr::getSingleton().fireEvent("VERSION_REQUEST_ING");
	/*连接服务器*/
	if (!sGameMgr.getEQMsgManager()->createNet((char*)mServerIP.c_str(), mServerPort))
		return false;

	req_ver_validate version;
	version.ver = PROTO_VER;
	return sGameMgr.getEQMsgManager()->Send(version);
}
//------------------------------------------------------------------------------
BOOL EQLogin::RolesRequest()
{
	req_get_roles roles;
	return sGameMgr.getEQMsgManager()->Send(roles);
}
//------------------------------------------------------------------------------
BOOL EQLogin::ReqEnterGame()
{
	return TRUE;
}


//------------------------------------------------------------------------------
void EQLogin::VersionReply()
{
	notify_ver_validate version;

	version.decode(sGameMgr.getEQMsgManager()->getRecebuff());

	if (version.result == ver_ok)
	{
		LoginRequest();
		GameEventMgr::getSingleton().fireEvent("VERSION_OK");
	}
	else if (version.result == ver_fail)
	{
		GameEventMgr::getSingleton().fireEvent("VERSION_FAILED");
	}
}
//------------------------------------------------------------------------------
void EQLogin::LoginReply()
{

}
//------------------------------------------------------------------------------
void EQLogin::RolesReply()
{

}
//------------------------------------------------------------------------------
void EQLogin::CreatPlayerReply()
{
	notify_create_role_result regist_result;

	regist_result.decode(sGameMgr.getEQMsgManager()->getRecebuff());

	if (regist_result.result == rr_ok)
	{
		// 创建角色成功，请求进入场景
		ReqEnterGame();
	}
	else if (regist_result.result == rr_fail)
	{
		// 创建角色失败
		GameEventMgr::getSingleton().fireEvent("ROLE_CREATED_FAILED");
	}
}
//------------------------------------------------------------------------------
void EQLogin::EnterSceneReply()
{
	notify_enter_scene_result scene;

	scene.decode(sGameMgr.getEQMsgManager()->getRecebuff());

	if (scene.result == sr_ok)
	{
		GameEventMgr::getSingleton().fireEvent("REQUEST_SCENE_OK");
	}
	else
	{
		GameEventMgr::getSingleton().fireEvent("REQUEST_SCENE_FAILED");
	}
}

bool EQLogin::handleMsg()
{
	switch(sGameMgr.getEQMsgManager()->getMsgtype())
	{
		case NetMsgType::msg_notify_ver_validate:				
			VersionReply();				
			break;
		case NetMsgType::msg_notify_login_result:				
			LoginReply();					
			break;
		case NetMsgType::msg_notify_get_roles_result:			
			RolesReply();					
			break;
		case NetMsgType::msg_notify_create_role_result:		    
			CreatPlayerReply();			
			break;
		case NetMsgType::msg_notify_enter_scene_result:		   
			EnterSceneReply();			
			break;

		default:
			return false;
	}

	return true;
}
