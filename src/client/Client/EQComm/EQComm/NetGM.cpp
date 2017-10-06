#include "EQCommPCH.h"
#include "CEGUI.h"
#include "CEGUILua.h"
#include "EQMsgManager.h"
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
#include "SmallModules.h"
#include "FitmentInteractMgr.h"
#include "GameState.h"
#include "EQGameSceneMgr.h"
#include "ObjMgr.h"
#include "Timekeeper.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

//------------------------------------------------------------------------------
BOOL EQMsgManager::LogoutRequest()
{
	if(!sStateMgr.gaming())
		return FALSE;

	sStateMgr.setState(GS_LOGIN);
	//EQGameSceneMgr::getSingleton().changeScene(SCENE_NONE, 0);
	sObjMgr.AllPlayerGosNone();

	req_logout logout;

	BOOL ret = m_Socket.Send(logout);
	
	m_Socket.Destroy();

	return ret;
}
//------------------------------------------------------------------------------
BOOL EQMsgManager::GMRequest(int type, const char* par)
{
	if(!sStateMgr.gaming())
		return FALSE;

	if(!sMainPlayer.isGmEnabled())
		return FALSE;

	req_gm_command com;
	com.type = type;
	//com.params.resize(1);
	com.params.push_back(par);

	return m_Socket.Send(com);
}
//------------------------------------------------------------------------------
BOOL EQMsgManager::GMRequest(int type, const char* par, const char* par2)
{
	if(!sStateMgr.gaming())
		return FALSE;

	if(!sMainPlayer.isGmEnabled())
		return FALSE;

	req_gm_command com;
	com.type = type;
	//com.params.resize(1);
	com.params.push_back(par);
	com.params.push_back(par2);

	return m_Socket.Send(com);
}
//------------------------------------------------------------------------------
BOOL EQMsgManager::GMRequest(int type, const char* par, const char* par2, const char* par3)
{
	if(!sStateMgr.gaming())
		return FALSE;

	if(!sMainPlayer.isGmEnabled())
		return FALSE;

	req_gm_command com;
	com.type = type;
	//com.params.resize(1);
	com.params.push_back(par);
	com.params.push_back(par2);
	com.params.push_back(par3);

	return m_Socket.Send(com);
}
//------------------------------------------------------------------------------
BOOL EQMsgManager::GMRequest(int type, const char* par, const char* par2, const char* par3, const char * par4)
{
	if(!sStateMgr.gaming())
		return FALSE;

	if(!sMainPlayer.isGmEnabled())
		return FALSE;

	req_gm_command com;
	com.type = type;
	//com.params.resize(1);
	com.params.push_back(par);
	com.params.push_back(par2);
	com.params.push_back(par3);
	com.params.push_back(par4);

	return m_Socket.Send(com);
}
//------------------------------------------------------------------------------
bool EQMsgManager::handleByGM()
{
	switch (mMsgtype)
	{
	case NetMsgType::msg_notify_sys_time:
		sTimekeeper.recvServerTime();
		EQGameSceneMgr::getSingleton().initDayNightTimer();
		break;
	case NetMsgType::msg_notify_package:
		OnRecePlayerPackage(mRecebuff);
		break;
	case NetMsgType::msg_notify_update_coin:
		recUpdateCoin(mRecebuff);
		break;
	default:
		return false;
	}

	return true;
}