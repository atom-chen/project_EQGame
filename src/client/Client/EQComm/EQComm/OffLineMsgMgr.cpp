#include "EQCommPCH.h"
#include "OffLineMsgMgr.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "GameEventMgr.h"
#include "CommFunc.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

//------------------------------------------------------
OffLineMsgMgr::OffLineMsgMgr():mAnimateBox(NULL),mAnimateBox1(NULL)
{
	_init();
}
//------------------------------------------------------
OffLineMsgMgr::~OffLineMsgMgr()
{
	removeAnimation();
}
//------------------------------------------------------
void OffLineMsgMgr::_init()
{
	_subcribeEvent();
}
//------------------------------------------------------
void OffLineMsgMgr::_subcribeEvent()
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_offline_invites, NewMsgHandle(OffLineMsgMgr, notify_OffineInvite));
}
//------------------------------------------------------
void OffLineMsgMgr::notify_OffineInvite(MsgPack &recBuff)
{
	notify_offline_invites offline_invites;
	offline_invites.decode(recBuff);
	clearInviteList();
	size_t size = offline_invites.inv_list.size();
	for (size_t idx = 0; idx < size; idx++)
	{
		_fillRecordsToList(offline_invites.inv_list[idx]);
	}

	_fillRecordsToLuaTable();
}
//------------------------------------------------------
void OffLineMsgMgr::clearInviteRecords()
{
	luabind::call_function<void>(sLuaMgr.getL(),"FLofflineMsgMgr_clearInviteRecords");
}
//------------------------------------------------------
void OffLineMsgMgr::_notify()
{
	GameEventMgr::getSingleton().fireEvent("NOTIFY_OFFLINE_INVITE");
}
//------------------------------------------------------
void OffLineMsgMgr::_fillRecordsToList(offline_invite_record invite_record)
{
	mInvite_list.push_back(invite_record);
}
//------------------------------------------------------
void OffLineMsgMgr::clearInviteList()
{
	mInvite_list.clear();
}
//------------------------------------------------------
void OffLineMsgMgr::_fillRecordsToLuaTable()
{
	std::string invitor;
	std::string invitor_name;
	int			invitor_type(0);
	std::string date;
	clearInviteRecords();
	size_t size = mInvite_list.size();
	for (size_t idx = 0; idx < size; idx++)
	{
		invitor			= mInvite_list[idx].invitor;
		invitor_name	= mInvite_list[idx].invitor_name;
		invitor_type	= mInvite_list[idx].invite_type;
		date			= FormatDate( "yyyyÄêmmÔÂddÈÕ",	mInvite_list[idx].date.year,
														mInvite_list[idx].date.month,
														mInvite_list[idx].date.day);
		date = GB2312ToUTF8(date.c_str());
		luabind::call_function<void>(sLuaMgr.getL(),"FLofflineMsgMgr_addInviteData_fromCpp",invitor,invitor_name,invitor_type,date);
	}
	_notify();
}
//--------------------------------------------------------------------
void OffLineMsgMgr::addAnimation()
{
	_addMsgBtnAnimation();
	_addFriendListBtnAnimation();
}
//--------------------------------------------------------------------
void OffLineMsgMgr::removeAnimation()
{
	_removeMsgBtnAnimation();
	_removeFriendListBtnAnimation();
}
//--------------------------------------------------------------------
void OffLineMsgMgr::_addMsgBtnAnimation()
{
	if(NULL != mAnimateBox)
		return;

	CEGUI::Imageset *imgSet = CEGUI::ImagesetManager::getSingleton().getImageset("Button5");
	CEGUI::Image img1 = imgSet->getImage("NewImage371");
	CEGUI::Image img2 = imgSet->getImage("NewImage370");

	uint width  = static_cast<uint>(CEGUI::WindowManager::getSingleton().getWindow("freindlist/sys_msg")->getWidth().asAbsolute(800));
	uint height = static_cast<uint>(CEGUI::WindowManager::getSingleton().getWindow("freindlist/sys_msg")->getHeight().asAbsolute(600));
	CEGUI::Animate animate;
	animate.initAnimate(300, width, height);

	animate.addAnimateFrame(img1);
	animate.addAnimateFrame(img2);

	mAnimateBox = static_cast<CEGUI::AnimateBox*>(CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/AnimateBox", "OfflineMsgAnimation"));
	mAnimateBox->addAnimate(animate);
	mAnimateBox->setMousePassThroughEnabled(true);

	mAnimateBox->play(true);

	CEGUI::WindowManager::getSingleton().getWindow("freindlist/sys_msg")->addChildWindow(mAnimateBox);
}
//--------------------------------------------------------------------
void OffLineMsgMgr::_addFriendListBtnAnimation()
{
	if(NULL != mAnimateBox1)
		return;

	CEGUI::Imageset *imgSet = CEGUI::ImagesetManager::getSingleton().getImageset("Button5");
	CEGUI::Image img1 = imgSet->getImage("NewImage67");
	CEGUI::Image img2 = imgSet->getImage("NewImage68");

	uint width  = static_cast<uint>(CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Friendlist")->getWidth().asAbsolute(800));
	uint height = static_cast<uint>(CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Friendlist")->getHeight().asAbsolute(600));
	CEGUI::Animate animate;
	animate.initAnimate(300, width, height);

	animate.addAnimateFrame(img1);
	animate.addAnimateFrame(img2);

	mAnimateBox1 = static_cast<CEGUI::AnimateBox*>(CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/AnimateBox", "FriendListBtnAnimation"));
	mAnimateBox1->addAnimate(animate);
	mAnimateBox1->setMousePassThroughEnabled(true);

	mAnimateBox1->play(true);

	CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Friendlist")->addChildWindow(mAnimateBox1);
}
//--------------------------------------------------------------------
void OffLineMsgMgr::_removeMsgBtnAnimation()
{
	if(NULL != CEGUI::WindowManager::getSingletonPtr())
	{
		if (mAnimateBox != NULL)
		{
			CEGUI::WindowManager::getSingleton().getWindow("freindlist/sys_msg")->removeChildWindow(mAnimateBox);
			CEGUI::WindowManager::getSingleton().destroyWindow("OfflineMsgAnimation");
		}
	}

	if(NULL != mAnimateBox)
	{
		//delete mAnimateBox;
		mAnimateBox = NULL;
	}
}
//--------------------------------------------------------------------
void OffLineMsgMgr::_removeFriendListBtnAnimation()
{
	if(NULL != CEGUI::WindowManager::getSingletonPtr() && NULL != mAnimateBox1)
	{
		CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Friendlist")->removeChildWindow(mAnimateBox1);
		CEGUI::WindowManager::getSingleton().destroyWindow("FriendListBtnAnimation");
	}

	if(NULL != mAnimateBox1)
	{
		//delete mAnimateBox;
		mAnimateBox1 = NULL;
	}
}