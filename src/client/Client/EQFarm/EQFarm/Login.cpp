#include "EQFarmPCH.h"
#include "Login.h"

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
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "EQCamera.h"
#include "CommFunc.h"

BOOL Login::LoginRequest()
{
	/*登录协议*/
	req_login reqLogin;                    
	reqLogin.account = simple_xor(mAccount);
	reqLogin.password = simple_xor(mPassword);

	MainPlayer::getSingleton().setAccount(mAccount);
	sGameMgr.getMainCamera()->setFollowMe(false);

	return sMsgMgr.Send(reqLogin);                
}

BOOL Login::ReqEnterGame()
{
	req_enter_scene scene;

	/*return */sMsgMgr.Send(scene);

	req_enter_farm req;
	req.owner = sMainPlayer.getAccount();
	sGameMgr.getMainCamera()->setFollowMe(false);

	return sMsgMgr.Send(req);
}

void Login::LoginReply()
{
	notify_login_result loginResult;

	/*反序列号数据*/
	loginResult.decode(sMsgMgr.getRecebuff());   

	if (loginResult.result == lr_ok)
	{
		req_enter_farm req;
		req.owner = sMainPlayer.getAccount();

		sMsgMgr.Send(req);
		GameEventMgr::getSingleton().fireEvent("LOGIN_OK");
	}
	else
	{
		/*回调脚本函数，登录失败*/
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "LoginWindows_OnFail",loginResult.result);
	}
}

void Login::RolesReply()
{
	notify_get_roles_result roles;

	roles.decode(sMsgMgr.getRecebuff());

	if (roles.player.empty())
	{
		GameEventMgr::getSingleton().fireEvent("ROLE_LIST_RETURN_EMPTY");
	}
	else
	{
		ReqEnterGame();
	}
}


bool CMessageManager::handleLoginRepeat()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_repeat_login:
		{
			notify_repeat_login loginRepeat;

			loginRepeat.decode(sMsgMgr.getRecebuff());

			//关闭网络
			m_Socket.Destroy();

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnLoginRepeat", 1);
		}
		return true;
	}

	return false;
}