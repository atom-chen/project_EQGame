#include "EQCommPCH.h"
#include "MsgSystem.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "NetMsgType.h"
#include "INetPacket.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "GameState.h"
#include "ItemTable.h"
#include "ChatSystem.h"
//----------------------------------------------------
template<> MsgSystem* Ogre::Singleton<MsgSystem>::ms_Singleton = 0;
//--------------------------------------------------------
MsgSystem::MsgSystem():mcreateMsgBoxTime(0)
{

}
//--------------------------------------------------------
bool EQMsgManager::handleByMsgSys()
{
	switch (mMsgtype)
	{
	case NetMsgType::msg_notify_sys_msg:
		sSysMsg.showServerMsg();
		break;
	default:
		return false;
	}
	return true;
}
//--------------------------------------------------------
void MsgSystem::showServerMsg()
{
	notify_sys_msg msgData;
	msgData.decode(sGameMgr.getEQMsgManager()->getRecebuff());

	std::vector<std::string> recparams;

	int	msgParamsSize = msgData.params.size();

	for (int i = 0; i < msgParamsSize; i++)
	{
		recparams.push_back(msgData.params[i]);

	}
	uint paramsSize = recparams.size();
	CLuaManager::getSingleton().CallFunction("getParamsSize",paramsSize);

	std::vector<std::string>::iterator iter = recparams.begin();
	std::vector<std::string>::iterator end = recparams.end();

	for ( ; iter != end; iter++)
	{
		CLuaManager::getSingleton().CallFunction("getSysMsgParams",(*iter));
	}
	CLuaManager::getSingleton().CallFunction("OnReceiveHandle",
		NetMsgType::msg_notify_sys_msg, 
		&msgData);
}
//-----------------------------------------------------------------
void MsgSystem::update()
{
	if (!sStateMgr.gaming())
		return ;

	DWORD now = GetTickCount();
	if (now - mcreateMsgBoxTime > 3000)
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "MsgSys_removeStyle5Box");
	}
}
//--------------------------------------------------------
void MsgSystem::setCreateMsgBoxTime()
{
	mcreateMsgBoxTime = GetTickCount();
}
//--------------------------------------------------------
void MsgSystem::showClientMsg(MSG_TYPE type, uint id)
{
	const SysMsgRow* row = ItemTableManager::getSingleton().getSysMsgRow(id);
	if(!row)
		return;

	showClientMsg(type, row->content);	
}
//--------------------------------------------------------
void MsgSystem::showClientMsg(MSG_TYPE type, std::string msg)
{
	if(type & MSG_CHATBOX)
	{
		ChatSystem::getSingleton().putTextOnChatBox(msg);
	}

	if(type & MSG_MSGBOX)
	{
		//luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "ShowMsgbox", msg);
	}
}