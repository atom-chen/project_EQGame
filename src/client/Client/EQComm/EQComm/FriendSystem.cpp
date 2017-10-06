#include "EQCommPCH.h"
#include "FriendSystem.h"
#include "NetMsgType.h"
#include "INetPacket.h"
#include "EQMsgManager.h"
#include "ObjMgr.h"
#include "Player.h"
#include "GameObj.h"
#include "SmallModules.h"
#include "EnumDef.h"
#include "ChatSystem.h"
#include "ObjMgr.h"
#include "CommFunc.h"
#include <math.h>
#include <algorithm>
#include "GUIManager.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "EQGameSceneMgr.h"
#include "EQGameMgr.h"
#include "tinyXml/tinyxml.h"
#include "tinyXml/tinystr.h"
//----------------------------------------------------
template<> FriendSystem* Ogre::Singleton<FriendSystem>::ms_Singleton = 0;
//----------------------------------------------------
FriendSystem::FriendSystem(void):mselectedItemName("")
								,mcurPage(1)
								,mTotalPage(1)
								,mSelectID(0)
								,mLastSelectID(0)
								,mLastSelBtn(NULL)
								,mLastSelectName("")
								,mSearchExMaxCnt(8)
								,mGMFriendSys(new GMFriendSys())
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_preview_house, NewMsgHandle(FriendSystem, handle_preview_fhouse));
	Regist(NetMsgType::msg_notify_friend_garbage, NewMsgHandle(FriendSystem, handle_friend_garbage));
	_initGMFriendInfo();
}
//----------------------------------------------------
void FriendSystem::_initGMFriendInfo()
{
	if (mGMFriendSys)
	{
			mGMFriendSys->loadGMFriendInfo();
	}
}
//----------------------------------------------------
FriendSystem::~FriendSystem(void)
{
	if (mfLWin != NULL)
	{
		mfLWin = NULL;
	}
	if (mGMFriendSys)
	{
		delete mGMFriendSys;
		mGMFriendSys = NULL;
	}
}
//----------------------------------------------------
BOOL FriendSystem::reqAddasFriend()
{
	std::string account;

	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		account = ((Player*)(obj))->getAccount();
	}
	return reqAsFriend(account);
}
//-----------------------------------------------------
BOOL FriendSystem::reqAsFriend(std::string reqacc)
{
	req_add_friend reqAddFriend;
	reqAddFriend.friend_account = reqacc;

	return sGameMgr.getEQMsgManager()->Send(reqAddFriend);
}
//-----------------------------------------------------
BOOL FriendSystem::reqRemoveFriend(std::string account)
{
	req_remove_friend reqDelFriend;
	reqDelFriend.friend_account = account;
	return sGameMgr.getEQMsgManager()->Send(reqDelFriend);
}
//----------------------------------------------------
BOOL FriendSystem::reqEnterFriendHouse(std::string friendAcc)
{
	//鏈夎繘鍏ユ埧闂翠汉鏁伴檺鍒?
	req_enter_player_house reqentFriHouse;
	if (!friendAcc.empty())
	{
		if (getFriendOnlineState(friendAcc) == ONLINE)//濂藉弸鍦ㄧ嚎鏃舵秷鎭�氱煡濂藉弸
		{

		}
		reqentFriHouse.account = friendAcc;
	}
	return sGameMgr.getEQMsgManager()->Send(reqentFriHouse);
}
//----------------------------------------------------
BOOL FriendSystem::reqEnterFriendHouseByLua()
{
	std::string account;

	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		account = ((Player*)(obj))->getAccount();
		reqEnterFriendHouse(account);
	}

	return true;
}
bool FriendSystem::reqHouseListFromCircleMenu()
{
	std::string account;

	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		account = ((Player*)(obj))->getAccount();

		SmallModules::getSingleton().reqHouseListByAcc(account.c_str());

		return true;
	}

	return false;
}
//-----------------------------------------------------
BOOL FriendSystem::reqFriendList()
{
	req_friend_list reqFriendLst;
	return sGameMgr.getEQMsgManager()->Send(reqFriendLst);
}
//-----------------------------------------------------
bool FriendSystem::reqInviteEnterHouse(std::string friendAcc)
{	
	// 	std::string invitefail = "閭�璇峰け璐ワ紒濂藉弸(" + friendAcc + ")涓嶅湪绾?";
	// 	if (getFriendOnlineState(friendAcc) == ONLINE)
	// 	{
	SmallModules::getSingleton().inviteOtherPlayer(ivt_enter_house,friendAcc);
	// 	}
	// 	else
	// 	{
	// 		LOGMSG(invitefail);
	// 		ChatSystem::getSingleton().putTextOnChatBox(GB2312ToUTF8(invitefail.c_str()));
	// 	}
	return true;
}

bool FriendSystem::reqInviteSelFriendHouse()
{
	return reqEnterFriendHouse(mLastSelectName) ? true : false;
}
//-----------------------------------------------------
bool FriendSystem::reqInviteEnterHouseByLua()
{
	std::string account;

	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		account = ((Player*)(obj))->getAccount();
		reqInviteEnterHouse(account);
	}

	return true;
}
//--------------------------------------------------------
bool FriendSystem::reqKickPlayerByLua()
{
	std::string kickAcc;
	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		kickAcc = ((Player*)(obj))->getAccount();
		reqKickPlayer(kickAcc);
	}

	return true;
}
//--------------------------------------------------------
bool FriendSystem::reqKickPlayer(std::string acc)
{
	req_kick_guest kickPlayer;
	kickPlayer.target_player = acc;
	sGameMgr.getEQMsgManager()->Send(kickPlayer);
	return true;
}
//------------------------------------------------------
void FriendSystem::notifyAddFriend(ByteArray& recBuff)
{
	notify_add_friend notify_addfriend;
	notify_addfriend.decode(recBuff);
	//濂藉弸淇℃伅鍒楄〃
	mFriendvec.push_back(notify_addfriend.friend_infos);
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
}
//-------------------------------------------------------
void FriendSystem::notifyFriendList(ByteArray& recBuff)
{
	notify_friend_list recFriendList;
	recFriendList.decode(recBuff);
	//
	mFriendvec.clear();
	std::vector<friend_info>::iterator iter = recFriendList.friend_vec.begin();
	std::vector<friend_info>::iterator end = recFriendList.friend_vec.end();
	for ( ; iter != end; iter++)
	{
		if (iter->basic_data.account == mGMFriendSys->getGmFriendInfo().account)
		{
			mGMFriendInfo = *iter;
		}
		else
		{
			mFriendvec.push_back(*iter);
			// 初始（插入好友头像数据）(add by hezhr)
			insertFriendPhoto(iter->basic_data.account, "");
		}
	}
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
}
//---------------------------------------------------------
void FriendSystem::insertGmFriendInfo(friend_info& gm_friendinfo)
{
	removeFriendInfo(gm_friendinfo.basic_data.account);
	mFriendvec.insert(mFriendvec.begin(),gm_friendinfo);
}
//---------------------------------------------------------
bool FriendSystem::removeFriendInfo(std::string del_acc)
{
	std::vector<friend_info>::iterator searchBegin = mFriendvec.begin();
	while (searchBegin != mFriendvec.end())
	{
		if (del_acc == searchBegin->basic_data.account)
		{
			searchBegin= mFriendvec.erase(searchBegin);
			return true;
		}
		else
		{
			++searchBegin;
		}
	}
	return false;
}
//---------------------------------------------------------
void FriendSystem::notifyRemoveFriend(ByteArray& recBuff)
{
	notify_remove_friend notifyDelFriend;
	notifyDelFriend.decode(recBuff);
	removeFriendInfo(notifyDelFriend.friend_account);
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
}
//-----------------------------------------------------
void FriendSystem::notifyFriendOnline(ByteArray& recBuff)
{
	notify_friend_online notifyOnline;
	notifyOnline.decode(recBuff);
	std::string onLineFriendAcc = notifyOnline.friend_account;
	changeOnlineState(onLineFriendAcc, ONLINE);
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
}
//-------------------------------------------------------
void FriendSystem::notifyFriendOffline(ByteArray& recBuff)
{
	notify_friend_offline notifyOffline;
	notifyOffline.decode(recBuff);
	std::string offLineAccount = notifyOffline.friend_account;
	changeOnlineState(offLineAccount, OFFLINE);
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
}
//-------------------------------------------------------
bool FriendSystem::changeOnlineState(std::string account, int stateVal)
{
	std::vector<friend_info>::iterator searchBegin = mFriendvec.begin();
	std::vector<friend_info>::iterator searchEnd = mFriendvec.end();
	for ( ; searchBegin != searchEnd; searchBegin++)
	{
		if (account == searchBegin->basic_data.account)
		{
			searchBegin->is_online = stateVal;
		}
		else
		{
			continue;
		}
	}
	return true;
}
//------------------------------------------------------
int FriendSystem::getFriendOnlineState(std::string friendAcc)
{
	if (!friendAcc.empty())
	{
		std::vector<friend_info>::iterator searchBegin = mFriendvec.begin();
		std::vector<friend_info>::iterator searchEnd = mFriendvec.end();
		for ( ; searchBegin != searchEnd; searchBegin++)
		{
			if (friendAcc == searchBegin->basic_data.account)
			{
				return searchBegin->is_online;
			}
			else
			{
			}
		}
	}
	return 0;
}
//--------------------------------------------------------
const player_basic_data* FriendSystem::searchFriendBasicInfo(std::string acc)
{	
	std::vector<friend_info>::iterator searchBegin = mFriendvec.begin();
	std::vector<friend_info>::iterator searchEnd = mFriendvec.end();
	for ( ; searchBegin != searchEnd; searchBegin++)
	{
		if (acc == searchBegin->basic_data.account)
		{
			return &searchBegin->basic_data;
		}
		else
		{
			continue;
		}
	}
	return NULL;
}
//--------------------------------------------------------
const player_basic_data* FriendSystem::searchFriendBasicInfoByName(std::string name)
{
	std::vector<friend_info>::iterator searchBegin = mFriendvec.begin();
	std::vector<friend_info>::iterator searchEnd = mFriendvec.end();
	for ( ; searchBegin != searchEnd; searchBegin++)
	{
		if (name == searchBegin->basic_data.username)
		{
			return &searchBegin->basic_data;
		}
		else
		{
			continue;
		}
	}
	return NULL;
}
//------------------------------------------------------
void FriendSystem::showList()
{
	int Sz = mShowList.size();
	CEGUI::String listBaseName = "freindlist/acc";
	CEGUI::String listBaseName1 = "freindlist/cover";
	CEGUI::String picBaseName = "freindlist/pic";
	CEGUI::Window* list[10];
	CEGUI::Window* pic[10];
	CEGUI::Window* gbgPic[10];
	CEGUI::PushButton* pbtn[10];
	for (int i = 0; i < 10; i++)
	{
		CEGUI::String snum = Ogre::StringConverter::toString(i+1).c_str();

		list[i] = CEGUI::WindowManager::getSingleton().getWindow(listBaseName + snum);
		pbtn[i] = static_cast<CEGUI::PushButton*>(CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + snum));
		pic[i] = CEGUI::WindowManager::getSingleton().getWindow(picBaseName + snum);
		gbgPic[i] = CEGUI::WindowManager::getSingleton().getWindow(listBaseName + snum + "/laji" + snum);
	}
	for (int i = 0; i< 10; i++)
	{
		pbtn[i]->setID(i + 1);
		list[i]->setID(i);
		list[i]->setVisible(false);
		pic[i]->setVisible(false);
		gbgPic[i]->setVisible(false);
		pbtn[i]->setVisible(false);
		pbtn[i]->setProperty("WantsMultiClickEvents","True");
		pbtn[i]->removeEvent(CEGUI::Window::EventMouseEnters);
		pbtn[i]->removeEvent(CEGUI::PushButton::EventMouseClick);
		pbtn[i]->removeEvent(CEGUI::PushButton::EventMouseDoubleClick);
		pbtn[i]->subscribeEvent(CEGUI::Window::EventMouseEnters,CEGUI::Event::Subscriber(&FriendSystem::isHovering, this));
		pbtn[i]->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&FriendSystem::handleEventMouseClick, this));
		pbtn[i]->subscribeEvent(CEGUI::PushButton::EventMouseDoubleClick, CEGUI::Event::Subscriber(&FriendSystem::handleEventMouseDbClick, this));
	}
	if (Sz != 0)
	{
		for (int i = 0; i < Sz; i++)
		{
			if (mShowList[i].is_online == ONLINE)
			{
				list[i]->setProperty("TextColours","tl:FF04F9F9 tr:FF04F9F9 bl:FF04F9F9 br:FF04F9F9");
				pic[i]->setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
			}
			else
			{
				list[i]->setProperty("TextColours","tl:FF5C96FF tr:FF5C96FF bl:FF5C96FF br:FF5C96FF");
				pic[i]->setProperty("ImageColours","tl:FF7F7F7F tr:FF7F7F7F bl:FF7F7F7F br:FF7F7F7F");
			}
			CGUIManager::getSingleton().SetText(list[i]->getName().c_str(),mShowList[i].basic_data.username.c_str());
			//list[i]->setText((CEGUI::utf8*)mShowList[i].basic_data.username.c_str());
			if (mShowList[i].basic_data.account == getGMFriendAcc())
			{
				pic[i]->setProperty("Image", "set:Button6 image:NewImage164");
				list[i]->setProperty("TextColours","tl:FF04F9F9 tr:FF04F9F9 bl:FF04F9F9 br:FF04F9F9");
				pic[i]->setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
			}
			else
				pic[i]->setProperty("Image", "set:Button1 image:ImageButton41");
			list[i]->setVisible(true);
			pic[i]->setVisible(true);
			pbtn[i]->setVisible(true);
			// 设置好友头像(add by hezhr)
			setFriendPhoto(mShowList[i].basic_data.account, pic[i]->getName().c_str());

			if(m_friendExData.find(mShowList[i].basic_data.account) != m_friendExData.end())
			{
				if(m_friendExData[mShowList[i].basic_data.account].GarbageCnt > 0)
				{
					gbgPic[i]->setProperty("Image", "set:Button6 image:NewImage163");
					gbgPic[i]->setVisible(true);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i <10; i++)
		{
			list[i]->setText("");
		}
	}
	if (mIsSearchMode)
	{
		getTotalPage(mSearchExArr,10);
	}
	else
	{
		getTotalPage(mFriendvec,10);
	}
	setNumTxt();
	static int initFlag = 0;
	if (initFlag == 0)
	{
		registerEvent();
		initFlag = 1;
	}
}
//-------------------------------------------------------
bool FriendSystem::isHovering(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winEvtArgs = static_cast<const CEGUI::WindowEventArgs&>(args);
	CEGUI::Window* hoverWin = winEvtArgs.window;
	int winID = hoverWin->getID();
	CEGUI::String listBaseName = "freindlist/acc";
	CEGUI::Window* st = CEGUI::WindowManager::getSingleton().getWindow(listBaseName+Ogre::StringConverter::toString(winID).c_str());
	int stID = st->getID();
	mSelectID = stID;
	int size = mShowList.size();
	if (stID <= (size -1))
	{
		mselectedItem = mShowList[stID];
		mselectedItemName = mselectedItem.basic_data.account;
	}
	return true;
}
//-------------------------------------------------------
BOOL FriendSystem::popUpMgrWin()
{
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	CEGUI::Window* popFriendLstUI = CEGUI::WindowManager::getSingleton().getWindow("friendlist_manage");
	CEGUI::Window* gmfriend_manageUI = CEGUI::WindowManager::getSingleton().getWindow("gmfriend_manage");
	CEGUI::MouseCursor* mouseCur = CEGUI::MouseCursor::getSingletonPtr();
	CEGUI::Vector2 mousePos = mouseCur->getPosition();
	float screenW = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
	float screenH = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
	float mouseX = mousePos.d_x;
	float mouseY = mousePos.d_y;
	float popuiW = popFriendLstUI->getWidth().asAbsolute(screenW);
	float popuiH = popFriendLstUI->getHeight().d_offset;
	if (mouseX + popuiW > screenW)
	{
		mouseX = screenW - popuiW;
	}
	if (mouseY + popuiH > screenH)
	{
		mouseY = screenH - popuiH;
	}
	if(mselectedItemName != mGMFriendInfo.basic_data.account)
	{
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("gmfriend_manage");
		popFriendLstUI->setPosition(CEGUI::UVector2(CEGUI::UDim(0,mouseX),CEGUI::UDim(0, mouseY)));
		mainWin->addChildWindow(popFriendLstUI);
		registerButtonEvent();
	}
	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("friendlist_manage");
		gmfriend_manageUI->setPosition(CEGUI::UVector2(CEGUI::UDim(0,mouseX),CEGUI::UDim(0, mouseY)));
		mainWin->addChildWindow(gmfriend_manageUI);
		CEGUI::WindowManager::getSingleton().getWindow("gmfriend_manage/channel1")->removeEvent(CEGUI::PushButton::EventClicked);
		CEGUI::WindowManager::getSingleton().getWindow("gmfriend_manage/channel1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEnterGMFriendHouse, this));
	}
	return true;
}
//------------------------------------------------------
bool FriendSystem::handleEnterGMFriendHouse(const CEGUI::EventArgs& args)
{
	if (!mLastSelectName.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnClickToFriendHouseFromFriendList" );
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("gmfriend_manage");
	return true;
}
//------------------------------------------------------
bool EQMsgManager::handleByFriendSys()
{
	switch (mMsgtype)
	{
	case NetMsgType::msg_notify_add_friend:
		FriendSystem::getSingleton().notifyAddFriend(mRecebuff);
		break;
	case NetMsgType::msg_notify_friend_list:
		FriendSystem::getSingleton().notifyFriendList(mRecebuff);
		break;
	case NetMsgType::msg_notify_remove_friend:
		FriendSystem::getSingleton().notifyRemoveFriend(mRecebuff);
		break;
	case NetMsgType::msg_notify_friend_online:
		FriendSystem::getSingleton().notifyFriendOnline(mRecebuff);
		break;
	case NetMsgType::msg_notify_friend_offline:
		FriendSystem::getSingleton().notifyFriendOffline(mRecebuff);
		break;
	default:
		return false;
	}
	return true;
}
//-------------------------------------------------------
bool FriendSystem::handleRootKeyDown(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	CEGUI::Window* MainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	switch (keyArgs.scancode)
	{
	case CEGUI::Key::Escape:
		if (mfLWin)
		{
			MainWin->removeChildWindow(mfLWin);
		}
		break;
	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------
bool FriendSystem::handleEventMouseClick(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs& mosArgs = static_cast<const CEGUI::MouseEventArgs&>(args);
	CEGUI::MouseButton keyVal = mosArgs.button;
	setPushStaUI(args);
	switch (keyVal)
	{
	case CEGUI::RightButton:
		popUpMgrWin();
		break;
	case CEGUI::LeftButton:
		{
			CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("friendlist_manage");
			CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("gmfriend_manage");
			luabind::call_function<void>(sLuaMgr.getL(), "OnSelectFriendChanged", mselectedItem.basic_data.username + "(" + mselectedItem.basic_data.account + ")");
			break;
		}
	default:
		break;
	}
	return true;
}
//-------------------------------------------------------------------------
bool FriendSystem::handleEventMouseDbClick(const CEGUI::EventArgs& args)
{
	if (!mLastSelectName.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnClickToFriendHouseFromFriendList" );
	}
	return true;
}
//-------------------------------------------------------------------------
bool FriendSystem::handleEventRemveFriend(const CEGUI::EventArgs& args)
{
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	if (!mLastSelectName.empty())
	{
		reqRemoveFriend(mLastSelectName);
	}
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}
//--------------------------------------------------------------------------
bool FriendSystem::handleEventPensonalChat(const CEGUI::EventArgs& args)
{
	if(!mLastSelectName.empty())
	{
		ChatSystem::getSingleton().setTalktoAccount(mselectedItem.basic_data.account);
		ChatSystem::getSingleton().setTalktoName(mselectedItem.basic_data.username);

		ChatSystem::getSingleton().talkPickObj();
	}
	else
	{
		LOGERR("mselectedItemName is NULL");
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}
//------------------------------------------------------------------------
bool FriendSystem::handleEventVisitFriendHouse(const CEGUI::EventArgs& args)
{
	if (!mLastSelectName.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnClickToFriendHouseFromFriendList" );
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}

bool FriendSystem::reqHouseListFromFriendList()
{
	SmallModules::getSingleton().reqHouseListByAcc(mLastSelectName.c_str());

	return true;
}
//------------------------------------------------------------------------
bool FriendSystem::handleEventInviteEnterHouse(const CEGUI::EventArgs &args)
{
	if(!mLastSelectName.empty())
	{
		reqInviteEnterHouse(mLastSelectName);
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}
//---------------------------------------------------------------------------
bool FriendSystem::handleEventViewFriendInfo(const CEGUI::EventArgs& args)
{
	if(!mLastSelectName.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "PopOtherPlayerInfo" );
		req_other_player_info req;
		req.account = mLastSelectName;
		sGameMgr.getEQMsgManager()->Send(req);
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}
//---------------------------------------------------------------------------
bool FriendSystem::handleEventSendMail(const CEGUI::EventArgs& args)
{
	if(!mLastSelectName.empty())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OpenOutBox" );
		CGUIManager::getSingleton().SetText("mail/open_mailbox/Tab 2/receivemanedit",mLastSelectName.c_str());
		CGUIManager::getSingleton().SetText("mail/open_mailbox/Tab 2/contentsedit","");
		CGUIManager::getSingleton().SetText("mail/open_mailbox/Tab 2/titleedit","");
	}
	CEGUI::Window* mainWin = CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot");
	mainWin->removeChildWindow("friendlist_manage");
	return true;
}
//---------------------------------------------------------------------------
void FriendSystem::setShowList(int page,std::vector<friend_info> *friend_vec,int maxSize)
{
	int start = (page - 1)*maxSize;
	int size = friend_vec->size();
	int	temp = start + getPageCount(page,maxSize);
	int end = min(temp,size)-1;
	mShowList.clear();
	if (friend_vec->empty())
	{
		return;
	}
	std::vector<std::string> account_list;
	for ( int j = start; j <= end; j++)
	{
		mShowList.push_back((*friend_vec)[j]);
		if (needUpdateFriendPhoto((*friend_vec)[j].basic_data.account))	// 点击刷新按钮，需要刷新好友头像(add by hezhr)
		{	
			setNeedUpdateFriendPhoto((*friend_vec)[j].basic_data.account, false);
			account_list.push_back((*friend_vec)[j].basic_data.account);
		}
	}
	if (!account_list.empty())	// 向服务器申请好友头像数据(add by hezhr)
	{
		req_eq_users_info req;
		req.account_list = account_list;
		sGameMgr.getEQMsgManager()->Send(req);
	}
}
//---------------------------------------------------------------------------
int FriendSystem::getPageCount(int page, int maxSize)
{
	int size = mFriendvec.size();
	int s = page * maxSize;
	if (size >= s)
	{
		return maxSize;
	}
	else
		return size % maxSize;
}
//-------------------------------------------------------------------
int FriendSystem::getTotalPage(std::vector<friend_info> &friend_vec,int maxSize)
{
	double size = friend_vec.size();
	int totalPage = (int)ceil(size/maxSize);
	mTotalPage = totalPage;
	return totalPage;
}
//--------------------------------------------------------------------
bool FriendSystem::handleEventGoToFirstPage(const CEGUI::EventArgs& args)
{
	mcurPage = 1;
	std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 	if (mGMFriendInfo)
	{
		insertGmFriendInfo(mGMFriendInfo);
	}
	setShowList(mcurPage,&mFriendvec, 10);
	showList();
	setNumTxt();
	updateBtnStaUI();
	return true;
}
//--------------------------------------------------------------------
bool FriendSystem::handleEventRefrash(const CEGUI::EventArgs& args)
{
	std::vector<friend_info>::iterator iter = mFriendvec.begin();
	CEGUI::WindowManager::getSingleton().getWindow("freindlist/searchbox")->setText("");
	for (iter; mFriendvec.end()!=iter; ++iter)	// 设置好友头像需要刷新标识(add by hezhr)
	{
		setNeedUpdateFriendPhoto(iter->basic_data.account, true);
	}
	reqFriendList();
	return true;
}
//--------------------------------------------------------------------
bool FriendSystem:: nextPage(const CEGUI::EventArgs& args)
{	
	if (mcurPage>=mTotalPage)
	{
		return true;
	}
	else
	{
		mcurPage = mcurPage + 1;
	}
	if (mIsSearchMode == false)
	{
		std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 		if (mGMFriendInfo)
		{
			insertGmFriendInfo(mGMFriendInfo);
		}
		setShowList(mcurPage,&mFriendvec, 10);
	}
	else
	{
		std::sort(mSearchExArr.begin(),mSearchExArr.end(),friendSort());
		setShowList(mcurPage,&mSearchExArr, 10);
	}
	showList();
	setNumTxt();
	updateBtnStaUI();
	return true;
}
//---------------------------------------------------------------------------
bool FriendSystem::prePage(const CEGUI::EventArgs& args)
{
	if (mcurPage <= 1)
	{
		return true;
	}
	else
	{
		mcurPage = mcurPage - 1;
	}
	if (mIsSearchMode == false)
	{
		std::sort(mFriendvec.begin(),mFriendvec.end(),friendSort());
// 		if (mGMFriendInfo)
		{
			insertGmFriendInfo(mGMFriendInfo);
		}
		setShowList(mcurPage,&mFriendvec, 10);
	}
	else
	{
		std::sort(mSearchExArr.begin(),mSearchExArr.end(),friendSort());
		setShowList(mcurPage,&mSearchExArr, 10);
	}
	showList();
	setNumTxt();
	updateBtnStaUI();
	return true;
}
//------------------------------------------------------------------------
void FriendSystem::setNumTxt()
{
	std::string separator = "/";
	CEGUI::WindowManager::getSingleton().getWindow("freindlist/yeshu")->setText(Ogre::StringConverter::toString(mcurPage)
		+ separator
		+ Ogre::StringConverter::toString(mTotalPage));
}
//---------------------------------------------------------------------------
bool FriendSystem::registerEvent()
{
	CEGUI::Window* nextBtn = CEGUI::WindowManager::getSingleton().getWindow("freindlist/fenyeanniu1");
	/*CEGUI::PushButton*/CEGUI::Window* preBtn = /*static_cast<CEGUI::PushButton*>(*/CEGUI::WindowManager::getSingleton().getWindow("freindlist/fenyeanniu")/*)*/;
	//  	nextBtn->removeEvent(CEGUI::PushButton::EventClicked);
	//  	preBtn->removeEvent(CEGUI::PushButton::EventClicked);
	nextBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FriendSystem::nextPage, this));
	preBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FriendSystem::prePage, this));
	CEGUI::WindowManager::getSingleton().getWindow("freindlist/shouye")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventGoToFirstPage, this));
	CEGUI::WindowManager::getSingleton().getWindow("freindlist/shuaxin")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventRefrash, this));

	return true;
}
//--------------------------------------------------------------------------
bool FriendSystem::registerButtonEvent()
{
	CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
	winMgr->getWindow("friendlist_manage/whisper1")->removeEvent(CEGUI::PushButton::EventClicked);
	winMgr->getWindow("friendlist_manage/invute1")->removeEvent(CEGUI::PushButton::EventClicked);
	winMgr->getWindow("friendlist_manage/check1")->removeEvent(CEGUI::PushButton::EventClicked);
	winMgr->getWindow("friendlist_manage/mail1")->removeEvent(CEGUI::PushButton::EventClicked);
	winMgr->getWindow("friendlist_manage/unfeirend")->removeEvent(CEGUI::PushButton::EventClicked);
	winMgr->getWindow("friendlist_manage/channel1")->removeEvent(CEGUI::PushButton::EventClicked);

	winMgr->getWindow("friendlist_manage/unfeirend")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventRemveFriend, this));
	winMgr->getWindow("friendlist_manage/whisper1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventPensonalChat, this));
	winMgr->getWindow("friendlist_manage/invute1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventInviteEnterHouse, this));
	winMgr->getWindow("friendlist_manage/channel1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventVisitFriendHouse, this));
	winMgr->getWindow("friendlist_manage/check1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventViewFriendInfo, this));
	winMgr->getWindow("friendlist_manage/mail1")->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&FriendSystem::handleEventSendMail, this));

	return true;
}
//-------------------------------------------------------------------------
bool FriendSystem::setPushStaUI(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::WindowEventArgs&>(args);
	CEGUI::Window* pushWin = winArgs.window;
	CEGUI::ButtonBase* pbtn = static_cast<CEGUI::ButtonBase*>(pushWin);
	if (mLastSelBtn != NULL)
	{
		resetBtnSta();
	}
	mLastSelBtn = NULL;
	pushWin->setProperty("NormalImage", "set:window2 image:farm_freindlist2");
	pushWin->setProperty("HoverImage", "set:window2 image:farm_freindlist2");
	pushWin->setProperty("PushedImage", "set:window2 image:farm_freindlist2");
	mLastSelBtn = pushWin;
	mLastSelectID = mSelectID;
	mLastSelectName = mselectedItemName;
	return true;
}
//-------------------------------------------------------------------------
bool FriendSystem::resetBtnSta()
{
	if(mLastSelBtn != NULL)
	{
		mLastSelBtn->setProperty("NormalImage", "set:window2 image:");
		mLastSelBtn->setProperty("HoverImage", "set:window2 image:farm_freindlist2");
		mLastSelBtn->setProperty("PushedImage", "set:window2 image:farm_freindlist2");
	}
	return true;
}
//-------------------------------------------------------------------------
void FriendSystem::updateBtnStaUI()
{
	int sz = mShowList.size();
	// 	LOGERR(Ogre::StringConverter::toString(sz));
	// 	LOGERR(mLastSelectName);
	CEGUI::String listBaseName1 = "freindlist/cover";
	for (int i=0; i< sz; i++)
	{
		if (0 == mLastSelectName.compare(mShowList[i].basic_data.account))
		{
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("NormalImage", "set:window2 image:farm_freindlist2");
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("HoverImage", "set:window2 image:farm_freindlist2");
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("PushedImage", "set:window2 image:farm_freindlist2");
		}
		else
		{
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("NormalImage", "set:window2 image:");
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("HoverImage", "set:window2 image:farm_freindlist2");
			CEGUI::WindowManager::getSingleton().getWindow(listBaseName1 + Ogre::StringConverter::toString(i+1).c_str())->
				setProperty("PushedImage", "set:window2 image:farm_freindlist2");
		}
	}
}
//-------------------------------------------------------------------------
void FriendSystem::insertFriendPhoto(const std::string& account, const std::string& photoData)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			iter->savePhotoData(photoData);
			return;
		}
	}
	// 插入一个新账号
	FriendPhoto photo;
	photo.setAccount(account);
	photo.savePhotoData(photoData);
	m_friendPhotoVec.push_back(photo);
}
//-------------------------------------------------------------------------
void FriendSystem::removeFriendPhoto(const std::string& account)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			m_friendPhotoVec.erase(iter);
			return;
		}
	}
}
//-------------------------------------------------------------------------
std::string FriendSystem::getFriendPhotoName(const std::string& account)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			return iter->getPhotoName();
		}
	}
	return "NULL";
}
//-------------------------------------------------------------------------
bool FriendSystem::needUpdateFriendPhoto(const std::string& account)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			return iter->isNeedUpdate();
		}
	}
	return false;
}
//-------------------------------------------------------------------------
void FriendSystem::setNeedUpdateFriendPhoto(const std::string& account, bool update)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			iter->setNeedUpdate(update);
		}
	}
}
//-------------------------------------------------------------------------
void FriendSystem::setFriendPhoto(const std::string& account, const std::string& winname)
{
	std::vector<FriendPhoto>::iterator iter = m_friendPhotoVec.begin();
	for (iter; m_friendPhotoVec.end()!=iter; ++iter)
	{
		if (account == iter->getAccount())	// 存在该账号
		{
			iter->setWindowPhoto(winname);
		}
	}
}
//-------------------------------------------------------------------------
void FriendSystem::setCurListPhotos(void)
{
	CEGUI::String picBaseName = "freindlist/pic";
	CEGUI::Window* pic[10];
	for (int i=0; i<10; i++)
	{
		pic[i] = CEGUI::WindowManager::getSingleton().getWindow(picBaseName + Ogre::StringConverter::toString(i + 1).c_str());
	}
	for (int i=0; mShowList.size()!=i; i++)
	{
		setFriendPhoto(mShowList[i].basic_data.account, pic[i]->getName().c_str());
	}
}
//-------------------------------------------------------------------------
void FriendSystem::search_matchKey( const char *key ,int mode)
{	
	
	mSearchExArr.clear();

	if (strcmp(key,"") ==0)
	{
		mIsSearchMode = false;
// 		if (mGMFriendInfo)
		{
			insertGmFriendInfo(mGMFriendInfo);
		}
		setShowList(mcurPage,&mFriendvec, 10);
		showList();
		setNumTxt();
		updateBtnStaUI();
		return;
	}
	mIsSearchMode = true;
	mcurPage = 1;
	for(std::vector<friend_info>::iterator itr = mFriendvec.begin(); itr != mFriendvec.end(); ++itr)
	{
		if(-1 != itr->basic_data.username.find(key))
		{
			mSearchExArr.push_back((*itr));
		}
	}
	
	if (mode == 1)
	{
// 		GameEventMgr::getSingleton().fireEvent("UPDATE_SEARCHKEY");
		setShowList(1,&mSearchExArr, 10);
		showList();
		getTotalPage(mSearchExArr,10);
		setNumTxt();
		updateBtnStaUI();
	}
	
}
//-------------------------------------------------------------------------
void FriendSystem::searchEx( const char *key )
{
	UINT cnt = 0;

	mSearchExArr.clear();

	for(std::vector<friend_info>::iterator itr = mFriendvec.begin(); itr != mFriendvec.end(); ++itr)
	{
		if(-1 != itr->basic_data.username.find(key))
		{
			mSearchExArr.push_back((*itr));

			cnt ++;

			if((int)cnt >= mSearchExMaxCnt)
				break;
		}
	}

}
//---------------------------------------------------------------------------
std::string FriendSystem::getSearchExResult( UINT idx )
{
	assert(idx < getSearchExCnt());

	friend_info finfo = mSearchExArr[idx];

	return finfo.basic_data.username + "(" + finfo.basic_data.account + ")";
	//return finfo->basic_data.username;

}
//-------------------------------------------------------------------------
void FriendSystem::reqPreviewFHousedata(int idx, std::string &acc)
{
	req_preview_house req;
	req.inst_id = SmallModules::getSingleton().getFHouseListData(idx)->instance_id;
	req.account = acc;
	sGameMgr.getEQMsgManager()->Send(req);
	SmallModules::ifReviewHouseing = true;
}

void FriendSystem::reqPreviewFHousedataByFriendList(int idx)
{
	reqPreviewFHousedata(idx, mLastSelectName);
}

void FriendSystem::reqPreviewFHousedataByCircleMenu(int idx)
{
	std::string account;

	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		account = ((Player*)(obj))->getAccount();

		reqPreviewFHousedata(idx, account);
	}
}

void FriendSystem::handle_preview_fhouse(MsgPack &recBuff)
{
	notify_preview_house data;
	data.decode(recBuff);

	// disable by hjx 目前已由互斥填表控制
	//停止走路
	//MainPlayer::getSingletonPtr()->stopWalk(true);

	//需在recHousedata前做，否则该函数会清空所有收到的数据
	SmallModules::getSingleton().previewHouse(data.house.template_id);
	EQGameSceneMgr::getSingleton().recHousedata(mLastSelectName, data.house);

}

void FriendSystem::handle_friend_garbage( MsgPack &recBuff )
{
	notify_friend_garbage data;
	data.decode(recBuff);

	if(m_friendExData.find(data.friend_account) == m_friendExData.end())
	{
		FriendExtendData fed;
		fed.GarbageCnt = data.n;
		fed.MagicBoxCnt = 0;

		m_friendExData.insert(std::make_pair(data.friend_account, fed));
	}
	else
	{
		m_friendExData[data.friend_account].GarbageCnt = data.n;
	}
}
//------------------------------------------------------------------
GMFriendSys::GMFriendSys()
{

}
//------------------------------------------------------------------
void GMFriendSys::loadGMFriendInfo()
{
	TiXmlDocument doc;
	doc.LoadFile("../Media/Data/gm_friend_tplt.xml");
	TiXmlElement* root = doc.RootElement()->FirstChildElement();
	m_gmfriend.account = root->FirstChildElement("account")->GetText();
	m_gmfriend.nick = root->FirstChildElement("name")->GetText();
	m_gmfriend.house_id = Ogre::StringConverter::parseInt(root->FirstChildElement("house_id")->GetText());
}