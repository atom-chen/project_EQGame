#include "EQClientPCH.h"
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

#include "CEGUI.h"
#include "CommFunc.h"
#include "ExceptionHandler.h"



BOOL Login::LoginRequest()
{
	/*��¼Э��*/
	req_login reqLogin;                    
	reqLogin.account = simple_xor(mAccount);
	reqLogin.password = simple_xor(mPassword);

	MainPlayer::getSingleton().setAccount(mAccount);
	EQErrorInfoEx::instance().account = mAccount;

	return sMsgMgr.Send(reqLogin);
}

BOOL Login::ReqEnterGame()
{
	req_enter_scene scene;

	return sMsgMgr.Send(scene);

	/*req_enter_farm req;
	req.owner = sMainPlayer.getAccount();

	return sMsgMgr.Send(req);*/
}

void Login::LoginReply()
{
	notify_login_result loginResult;

	/*�����к�����*/
	loginResult.decode(sMsgMgr.getRecebuff());   

	if (loginResult.result == lr_ok)
	{
		/*�ص��ű���������¼�ɹ�*/
		GameEventMgr::getSingleton().fireEvent("LOGIN_OK");
	}
	else
	{
		/*�ص��ű���������¼ʧ��*/
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "LoginWindows_OnFail",loginResult.result);
	}
}
//------------------------------------------------------------------------------
void Login::RolesReply()
{
	notify_get_roles_result roles;

	roles.decode(sMsgMgr.getRecebuff());

	if (roles.player.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "CreatePlayer_OnCreate", roles.nick_name);
// 		GameEventMgr::getSingleton().fireEvent("ROLE_LIST_RETURN_EMPTY");
// 		//�����ǳ�
// 		CEGUI::String nickName((CEGUI::utf8*)(roles.nick_name.c_str()));
// 		CEGUI::WindowManager::getSingleton().getWindow("CreateCharacter/BackL/EditPlayerName")->setText( nickName );
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

			//�ر�����
			m_Socket.Destroy();

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnLoginRepeat", 0);
		}
		return true;
	}

	return false;
}

