#include "EQCommPCH.h"

#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "NetPacket.h"
#include "ByteArray.h"
#include "NetMsgType.h"
#include "GameEventMgr.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "SmallModules.h"

//--------------------------------------------------------------------
void SmallModules::agreeInvite()
{
	endCheckAnswer();
	req_agree_invitation reqAgree;
	reqAgree.invitor = mInviterAcc;
	reqAgree.type = getInviteType();
	if (ifReviewHouseing)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "ReqEndPreviewHouse");
	}
	sGameMgr.getEQMsgManager()->Send((INetPacket&)reqAgree);
}
//--------------------------------------------------------------------
void SmallModules::disagreeInvite()
{
	endCheckAnswer();
	req_disagree_invitation reqDisagree;
	reqDisagree.invitor = mInviterAcc;
	reqDisagree.type = getInviteType();
	sGameMgr.getEQMsgManager()->Send((INetPacket&)reqDisagree);
}
//--------------------------------------------------------------------
void SmallModules::inviteOtherPlayer(const int & type, const std::string & inviteeAcc)
{
	req_invite_someone reqInvite;

	reqInvite.invitee = inviteeAcc;
	reqInvite.type    = type;

	sGameMgr.getEQMsgManager()->Send((INetPacket&)reqInvite);
}
//--------------------------------------------------------------------
//bool SmallModules::handleByInvite(UINT16 msgType, ByteArray &recBuff)
//{
//	notify_invitation inviteInfo;
//
//	switch(msgType)
//	{
//	// ���յ�����֪ͨ
//	case NetMsgType::msg_notify_invitation:
//		// ��ȡ�������ͺ�����������
//		inviteInfo.decode(recBuff);
//		mInvitee = inviteInfo.invitor_name;
//		mInviteType = inviteInfo.type;
//		mInviterAcc = inviteInfo.invitor;
//		// ���������
//		GameEventMgr::getSingleton().fireEvent("SHOW_INVITE_DIALOG");
//		// ��ʼ��⽻����Ӧ
//		startCheckAnswer(ANSWER_INVITE);
//
//		break;
//
//	// ���յ�ȡ������֪ͨ
//	case NetMsgType::msg_notify_cancel_invitation:
//		// ���������
//		GameEventMgr::getSingleton().fireEvent("UNSHOW_INVITE_DIALOG");
//		break;
//
//	default:
//		return false;
//	}
//
//	return true;
//}
void SmallModules::handle_invitation(MsgPack &recBuff)
{
	// ��ȡ�������ͺ�����������
	notify_invitation inviteInfo;
	inviteInfo.decode(recBuff);
	mInvitee = inviteInfo.invitor_name;
	mInviteType = inviteInfo.type;
	mInviterAcc = inviteInfo.invitor;
	// ���������
	GameEventMgr::getSingleton().fireEvent("SHOW_INVITE_DIALOG");
	// ��ʼ��⽻����Ӧ
	startCheckAnswer(ANSWER_INVITE);
}

void SmallModules::handle_cancel_invitation(MsgPack &recBuff)
{
	// ���������
	GameEventMgr::getSingleton().fireEvent("UNSHOW_INVITE_DIALOG");
}
//--------------------------------------------------------------------