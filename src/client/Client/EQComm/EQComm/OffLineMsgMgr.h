/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		29/4/2011	10:24
* 文件: 	OffLineMsgMgr.h
* 描述:		离线消息提醒和设置
******************
*********************************************************************/
#ifndef OffLineMsgMgr_H
#define OffLineMsgMgr_H

#include "EQMainPrerequisites.h"
#include "NetPacket.h"
#include "MsgHandleRegist.h"
#include "CEGUIEX/CEGUIAnimateBox.h"
#include "CEGUI.h"
#include "CEGUIImage.h"
#include "CEGUIImageset.h"

class OffLineMsgMgr : public MemberMsgHandleRegistHelper
{
public:
	/**construct
	*/
	OffLineMsgMgr();
	~OffLineMsgMgr();

	static OffLineMsgMgr& getSingleton()
	{
		static OffLineMsgMgr offline_msg_mgr;
		return offline_msg_mgr;
	}
public:
	typedef std::vector<offline_invite_record> OfflineInviteListType;
public:
	/**Interface
	*/

	/**
	*/
	void notify_OffineInvite(MsgPack &recBuff);

	/**
	*/
	void clearInviteRecords();
	/**
	*/
	void clearInviteList();
	/**
	*/
	void addAnimation();
	/**
	*/
	void removeAnimation();


protected:
private:
	void _init();
	void _subcribeEvent();
	void _notify();
	void _fillRecordsToList(offline_invite_record invite_record);
	void _fillRecordsToLuaTable();
	void _addMsgBtnAnimation();
	void _addFriendListBtnAnimation();
	void _removeMsgBtnAnimation();
	void _removeFriendListBtnAnimation();

private:
	OfflineInviteListType mInvite_list;
	CEGUI::AnimateBox*      mAnimateBox;
	CEGUI::AnimateBox*      mAnimateBox1;

};

#define  sOffineMsgMgr OffLineMsgMgr::getSingleton()
#endif