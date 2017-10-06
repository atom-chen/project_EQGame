#include "EQCommPCH.h"
#include "MailManager.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "MainPlayer.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include <math.h>
#include "CEGUIEX/CEGUIAnimateBox.h"
#include "CEGUI.h"
#include "CEGUIImage.h"
#include "CEGUIImageset.h"
#include "ObjMgr.h"
#include "CommFunc.h"
#include "StringHandle.h"


//--------------------------------------------------------------------
template<> MailManager* Ogre::Singleton<MailManager>::ms_Singleton = 0;
//--------------------------------------------------------------------
MailManager::MailManager()
	:mReqMailListFlag(false)
	,mAnimateBox(NULL)
{
	mSendMail.items.resize(Mail_Item_Size);

	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_send_mail_success, NewMsgHandle(MailManager, handle_send_mail_success));
	Regist(NetMsgType::msg_notify_mail_list, NewMsgHandle(MailManager, handle_mail_list));
	Regist(NetMsgType::msg_notify_mail_content, NewMsgHandle(MailManager, handle_mail_content));
	Regist(NetMsgType::msg_notify_delete_mail, NewMsgHandle(MailManager, handle_delete_mail));
	Regist(NetMsgType::msg_notify_delete_mail_item, NewMsgHandle(MailManager, handle_delete_mail_item));
	Regist(NetMsgType::msg_notify_add_mail, NewMsgHandle(MailManager, handle_add_mail));
	Regist(NetMsgType::msg_notify_mail_not_read, NewMsgHandle(MailManager, handle_mail_not_read));
	Regist(NetMsgType::msg_notify_reject_mail_success, NewMsgHandle(MailManager, handle_reject_mail_success));
}
//--------------------------------------------------------------------
MailManager::~MailManager()
{
	removeAnimation();
}
//--------------------------------------------------------------------
//bool EQMsgManager::handleByMailMsg()
//{
//	return MailManager::getSingleton().handleByMailMsg(mMsgtype, mRecebuff);
//}
//--------------------------------------------------------------------
//bool MailManager::handleByMailMsg(UINT16 msgType, ByteArray& recBuff)
//{
//
//	switch(msgType)
//	{
//	case::NetMsgType::msg_notify_send_mail_success:
//		{
//				
//		}
//		break;
//	case::NetMsgType::msg_notify_mail_list:
//		{
//			
//		}
//		break;
//	case::NetMsgType::msg_notify_mail_content:
//		{
//					
//		}
//		break;
//	case::NetMsgType::msg_notify_delete_mail:
//		{
//			
//			
//		}
//		break;
//	case::NetMsgType::msg_notify_delete_mail_item:
//		{
//			
//		}
//		break;
//	case::NetMsgType::msg_notify_add_mail:
//		{
//			
//		}
//		break;
//	case::NetMsgType::msg_notify_mail_not_read:
//		{
//			
//		}
//		break;
//	case::NetMsgType::msg_notify_reject_mail_success:
//		{
//			
//		}
//		break;
//
//	default:return false;
//	}
//
//	return true;
//}

void MailManager::handle_send_mail_success(MsgPack &recBuff)
{
	notify_send_mail_success data;
	data.decode(recBuff);

	_resetMail(mSendMail);
	luabind::call_function<void>(sLuaMgr.getL(), "SuccessSendMail");
}

void MailManager::handle_mail_list(MsgPack &recBuff)
{
	Mail mail;
	notify_mail_list mailList;
	mailList.decode(recBuff);
	//MailItem item(0, "", -1, 0);

	mMailList.clear();
	luabind::call_function<void>(sLuaMgr.getL(), "ClearMailBox");

	uint size = mailList.mails.size();
	for(uint i = 0; i< size; i++)
	{
		addMail(mailList.mails[i], Back);
	}

	if(size > Max_Mail_Count)
		mStartOffset = size - Max_Mail_Count;
	else
		mStartOffset = 0;

	_sortMailList();
	luabind::call_function<void>(sLuaMgr.getL(), "UpdateMailPage");
}

void MailManager::handle_mail_content(MsgPack &recBuff)
{
	notify_mail_content content;
	content.decode(recBuff);

	mCurMail->content = content.content;
	mCurMail->read_status = READ_CONTENT; // 设置已读状态
	mCurMail->isReqContent = true;

	luabind::call_function<void>(sLuaMgr.getL(), "SetMail", mCurMail);
	luabind::call_function<void>(sLuaMgr.getL(), "ChangeMailIcon", mCurMail->type);
}

void MailManager::handle_delete_mail(MsgPack &recBuff)
{
	notify_delete_mail delMail;
	delMail.decode(recBuff);

	deleteMail(delMail.id);
}

void MailManager::handle_delete_mail_item(MsgPack &recBuff)
{
	notify_delete_mail_item data;
	data.decode(recBuff);

	_deleteCurMailItem(data.id, data.inst_id);

	luabind::call_function<void>(sLuaMgr.getL(), "UpdateMailItem");
}

void MailManager::handle_add_mail(MsgPack &recBuff)
{
	if(true == mReqMailListFlag)
	{
		notify_add_mail mail;
		mail.decode(recBuff);
		addMail(mail.mail_data);

		if(mMailList.size() > Max_Mail_Count)
			mStartOffset = mMailList.size() - Max_Mail_Count;
		else
			mStartOffset = 0;
	}
	addAnimation();
}

void MailManager::handle_mail_not_read(MsgPack &recBuff)
{
	addAnimation();
}

void MailManager::handle_reject_mail_success(MsgPack &recBuff)
{
	notify_reject_mail_success data;
	data.decode(recBuff);
	deleteMail(data.id);
}
//--------------------------------------------------------------------
void MailManager::setSendMailItem(uint index, uint storageIndex)
{
	assert(index >= 0 &&  index < Mail_Item_Size);

	// 需要传入的是对于当前页的偏移
	storageIndex = storageIndex % 30;

	uint64 insId =  MainPlayer::getSingleton().getItemInstanceID(storageIndex,  ItemPack::PT_WAREHOUSE);
	int templateId = MainPlayer::getSingleton().getItemID(storageIndex,  ItemPack::PT_WAREHOUSE);
	std::string effectTime = MainPlayer::getSingleton().getItemDelTime(templateId, ItemPack::PT_WAREHOUSE);
	const ItemGrid*	itemGrid = MainPlayer::getSingleton().getItemGird( storageIndex, ItemPack::PT_WAREHOUSE);

	int existIndex = existItem(insId);
	if(-1 != existIndex)
	{
		swapItem(index, existIndex);
	}
	else
	{	
		// 解锁原来物品
		if(0 != mSendMail.items[index].mInstanceId)
			reqUnlockStorageItem(mSendMail.items[index].mInstanceId);
		
		// 锁定新物品
		mSendMail.items[index].setItem(insId, effectTime, templateId, itemGrid->mOverlay );
		reqLockStorageItem(insId);
	}
}
//--------------------------------------------------------------------
void MailManager::sendMail(std::string acc, std::string title, std::string content) const
{
	req_send_mail req;
	req.recver_account = acc;
	req.recver_name    = "";
	std::string tmp_title;
	tmp_title = sStringHandle.stringFilter(const_cast<char *>(title.c_str()),0);
	req.title		   = tmp_title;
	// 控件自带一个回车 无法删除
	if("\n" == content)
		content = "";
	std::string output;
	output = sStringHandle.stringFilter(const_cast<char *>(content.c_str()),0);
	req.content		   = output;	

	for(uint i = 0; i < mSendMail.items.size(); i++)
	{
		if(0 != mSendMail.items[i].mInstanceId)
			req.items.push_back(mSendMail.items[i].mInstanceId);
	}

	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
int MailManager::getSendMailItemId(uint index ) const
{
	assert( index >= 0 && index < Mail_Item_Size);
	return mSendMail.items[index].mTemplateId;
}
//--------------------------------------------------------------------
uint MailManager::getSendMailItemCount(uint index ) const
{
	assert( index >= 0 && index < Mail_Item_Size);
	return mSendMail.items[index].mCount;
}
//--------------------------------------------------------------------
std::string MailManager::getSendMailItemEffectTime(uint index)
{
	assert( index >= 0 && index < Mail_Item_Size);
	return mSendMail.items[index].mEffectTime;
}
//--------------------------------------------------------------------
int MailManager::existItem(uint64 instanceId) const
{
	if(0 == instanceId)
		return -1;

	for(uint i = 0; i < mSendMail.items.size(); i++)
	{
		if (mSendMail.items[i].mInstanceId == instanceId)
			return i;
	}
	return -1;
}
//--------------------------------------------------------------------
void MailManager::swapItem(int toIndex, int fromIndex)
{
	MailItem temp = mSendMail.items[fromIndex];
	mSendMail.items[fromIndex] = mSendMail.items[toIndex];
	mSendMail.items[toIndex]   = temp;
}
//--------------------------------------------------------------------
void MailManager::updateItems(void)
{
	for (uint i=0; i<mSendMail.items.size(); ++i)
	{
		// 邮件物品表中的物品不在背包中
		if (false == MainPlayer::getSingleton().isInItemPack(mSendMail.items[i].mTemplateId))
		{
			mSendMail.items[i].setItem(0, "", -1, 0);
			luabind::call_function<void>(sLuaMgr.getL(), "MailClearItem", i+1);	// 物品时效已到，处理点击时的情况(hezhr)
		}
	}
}
//--------------------------------------------------------------------
void MailManager::reqMailList()
{
	if(false == mReqMailListFlag)
	{
		req_mail_list req;
		sGameMgr.getEQMsgManager()->Send(req);
		mReqMailListFlag = true;
	}
}
//--------------------------------------------------------------------
void MailManager::reqKnowMailList(uint startIndex, uint endIndex)
{
	if(0 == mMailList.size())
		return;

	req_know_new_mail_title req;

	assert(endIndex >= startIndex);

	MailList::iterator iter = mMailList.begin();

	startIndex += mStartOffset;
	endIndex   += mStartOffset;
	
	// list 不支持加法运算
	int setpCout = static_cast<int>(startIndex);
	while(setpCout-- > 0 )
	{
		iter++;
	}

	mCurPageFirstIter = iter;

	for(uint i = 0; i <= endIndex - startIndex; i ++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "SetMailGrid", (i+1), (*iter));

		if(NOT_READ == iter->read_status)
		{
			req.id_list.push_back(iter->id);
			iter->read_status = READ_TITLE;
		}
		iter++;
	}

	if(0 < req.id_list.size())
		sGameMgr.getEQMsgManager()->Send(req);
}

//--------------------------------------------------------------------
uint MailManager::getTotalPage() const
{
	float size = static_cast<float>(mMailList.size());
	return std::min(static_cast<uint>(ceil(size/Mail_Count_PerPage)), Max_Mail_Count/Mail_Count_PerPage);
}
//--------------------------------------------------------------------
uint MailManager::getPageLastIndex(uint page) const
{
	uint totalPage = getTotalPage();

	assert(page > 0 && page <= totalPage);

	return min(page * Mail_Count_PerPage - 1, mMailList.size() - 1);
}
//--------------------------------------------------------------------
void delTabAndEnterChars(std::string &str)
{
	size_t size = str.length();

	while(size)
	{
		if('\t' == str[size - 1] || '\n' == str[size - 1])
			str.erase(size - 1, 1);
		else
			break;
	}
}

void MailManager::addMail(const mail_info &mailInfo, AddPosition ps)
{
	Mail mail;
	MailItem item(0, "", -1, 0);

	mail.account = mailInfo.sender_account;
	mail.id		 = mailInfo.id;
	mail.name	 = mailInfo.sender_name;
	mail.title	 = mailInfo.title;
	mail.type	 = mailInfo.type;
	mail.remain_time = mailInfo.remain_time;
	mail.read_status = mailInfo.read_status;

	delTabAndEnterChars(mail.name);
	delTabAndEnterChars(mail.title);

	for(uint j = 0; j < mailInfo.items.size(); j++)
	{
		item.mCount	    =  mailInfo.items[j].count;
		item.mInstanceId = mailInfo.items[j].item_data.instance_id;
		item.mTemplateId = mailInfo.items[j].item_data.template_id;
		int year = mailInfo.items[j].item_data.del_time.year;
		int month = mailInfo.items[j].item_data.del_time.month;
		int day = mailInfo.items[j].item_data.del_time.day;
		int hour = mailInfo.items[j].item_data.del_time.hour;
		int minute= mailInfo.items[j].item_data.del_time.minute;
		int second = mailInfo.items[j].item_data.del_time.second;
		item.mEffectTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
		mail.items.push_back(item);
	}

	if (Front == ps)
		mMailList.push_front(mail);
	else
		mMailList.push_back(mail);
}
//--------------------------------------------------------------------
Mail* MailManager::getMail(uint page, uint offset)
{
	assert(offset >= 0 && offset < Mail_Count_PerPage);

	if((page - 1) * Mail_Count_PerPage + offset > mMailList.size())
		return NULL;
	
	MailList::iterator iter = mCurPageFirstIter;

	while((--offset) > 0)
	{
		iter ++;
	}

	return  &(*iter);
}
//--------------------------------------------------------------------
void MailManager::reqMailContent(uint page, uint offset)
{
	mCurMail = getMail( page, offset);

	if(NULL == mCurMail)
		return;
	
	// 内容已经存在，不再请求
	if(READ_CONTENT == mCurMail->read_status && mCurMail->isReqContent)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "SetMail", mCurMail);
		return;
	}

	req_mail_content req;
	req.id = mCurMail->id;

	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
MailItem * MailManager::getCurMailItem(uint index)
{
	assert(index >= 0 && index < Mail_Item_Size);

	if(index >= mCurMail->items.size() || 0 == mCurMail->items[index].mInstanceId)
		return NULL;
	else
		return &(mCurMail->items[index]);
}
//--------------------------------------------------------------------
std::string MailManager::getCurMailItemEffectTime(uint index)
{
	assert(index >= 0 && index < Mail_Item_Size);

	if(index >= mCurMail->items.size() || 0 == mCurMail->items[index].mInstanceId)
		return "";
	else
		return mCurMail->items[index].mEffectTime;
}
//--------------------------------------------------------------------
uint MailManager::getCurMailItemCount() const
{
	return mCurMail->items.size();
}
//--------------------------------------------------------------------
void MailManager::_resetMail(Mail &mail)
{
	mail.id = 0;
	mail.type = 0;
	mail.read_status = 0;
	mail.remain_time = 0;
	mail.isReqContent = false;
	mail.name = "";
	mail.account = "";
	mail.title = "";
	mail.content = "";
	
	for(uint i = 0; i<mail.items.size(); i++)
	{
		mail.items[i].setItem(0, "", -1, 0);
	}
}
//--------------------------------------------------------------------
void MailManager::reqGetMailItem(uint itemIndex) const
{
	assert(itemIndex >= 0 && itemIndex < Mail_Item_Size);

	req_get_mail_item req;
	req.id = mCurMail->id;
	req.inst_id = mCurMail->items[itemIndex].mInstanceId;

	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void MailManager::reqDeleteMail() const
{
	req_delete_mail req;
	req.id = mCurMail->id;

	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void MailManager::reqReturnMail() const
{
	req_reject_mail req;
	req.id = mCurMail->id;

	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void MailManager::deleteMail(uint64 mailId)
{
	MailList::iterator iter = std::find(mMailList.begin(), mMailList.end(), mailId);
	if(iter != mMailList.end())
	{

		// 若删除的是当前邮件
		if(mCurMail && iter->id == mCurMail->id)
			luabind::call_function<void>(sLuaMgr.getL(), "CloseMail");

		// 删除的是该页的第一份邮件
		if(iter == mCurPageFirstIter)
		{
			luabind::call_function<void>(sLuaMgr.getL(), "SubCurPage");
			mCurMail = NULL;
		}

		mMailList.erase(iter);

		if(0 < mStartOffset)
			mStartOffset--;

		luabind::call_function<void>(sLuaMgr.getL(), "UpdateMailPage");
	}
}
//--------------------------------------------------------------------
bool Mail::operator ==(const uint64 mailId) const
{
	return id == mailId;
}
//--------------------------------------------------------------------
bool Mail::operator <(const Mail &cmpMail) const
{
	return remain_time > cmpMail.remain_time;
}
//--------------------------------------------------------------------
void MailItem::setItem(uint64 insId /* = 0 */, int temId /* = -1 */, int count /* = 0 */)
{
	mInstanceId = insId;
	mTemplateId = temId;
	mCount	    = count;
}
void MailItem::setItem(uint64 insId , std::string effTime, int temId , int count )
{
	mInstanceId = insId;
	mEffectTime = effTime;
	mTemplateId = temId;
	mCount		= count;
}
//--------------------------------------------------------------------
bool MailItem::operator == (const uint64 insId) const
{
	return mInstanceId == insId;
}
//--------------------------------------------------------------------
void MailManager::_deleteCurMailItem(uint64 mailId, uint64 insId)
{
	if(mailId != mCurMail->id)
	{
		mCurMailIter = std::find(mMailList.begin(), mMailList.end(), mailId);
		mCurMail = &(*mCurMailIter);
	}

	assert(mCurMail);

	MailItems::iterator iter = std::find(mCurMail->items.begin(), mCurMail->items.end(), insId);
	if (mCurMail->items.end() != iter)
	{
		iter->setItem(0, "", -1, 0);
		//mCurMail->items.erase(iter);
	}
	else
		return;
}
//--------------------------------------------------------------------
void MailManager::_sortMailList()
{
	mMailList.sort();
}
//--------------------------------------------------------------------
void MailManager::addAnimation()
{
	if(NULL != mAnimateBox)
		return;

	luabind::call_function<void>(sLuaMgr.getL(), "OnRecvNewMail");

	CEGUI::Imageset *imgSet = CEGUI::ImagesetManager::getSingleton().getImageset("Button5");
	CEGUI::Image img1 = imgSet->getImage("NewImage430");
	CEGUI::Image img2 = imgSet->getImage("NewImage430"/*"NewImage188"*/);

	float width  = img1.getWidth();
	float height = img1.getHeight();
	CEGUI::Animate animate;
	animate.initAnimate(1000, width, height);

	animate.addAnimateFrame(img1);
	animate.addAnimateFrame(img2);

	mAnimateBox = static_cast<CEGUI::AnimateBox*>(CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/AnimateBox", "MailAnimation"));
	mAnimateBox->addAnimate(animate);
	//mAnimateBox->setMousePassThroughEnabled(true);
	mAnimateBox->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&MailManager::clickNewMailPaoPao, this));
	mAnimateBox->setClippedByParent(false);
	mAnimateBox->play(true);
	CEGUI::UDim mail_btn_posx = CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Email")->getXPosition();
	CEGUI::UDim mail_btn_posy = CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Email")->getYPosition();
	mAnimateBox->setXPosition(mail_btn_posx - CEGUI::UDim(0,width) + CEGUI::UDim(0,30));
	mAnimateBox->setYPosition(mail_btn_posy - CEGUI::UDim(0,height) + CEGUI::UDim(0,10));
	CEGUI::WindowManager::getSingleton().getWindow("MainRoot/SImg_VMenubar")->addChildWindow(mAnimateBox);
}
//--------------------------------------------------------------------
bool MailManager::clickNewMailPaoPao( const CEGUI::EventArgs& args )
{
	luabind::call_function<void>(sLuaMgr.getL(), "Mail_clickNewMailPaoPao");
	return true;
}
//--------------------------------------------------------------------
void MailManager::removeAnimation()
{
	if(NULL != CEGUI::WindowManager::getSingletonPtr())
	{
		CEGUI::WindowManager::getSingleton().getWindow("MainRoot/HMenubar/Btn_Email")->removeChildWindow(mAnimateBox);
		CEGUI::WindowManager::getSingleton().destroyWindow("MailAnimation");
	}

	if(NULL != mAnimateBox)
	{
		//delete mAnimateBox;
		mAnimateBox = NULL;
	}
}
//--------------------------------------------------------------------
const bool MailManager::isAllReaded() const
{
	MailList::const_iterator iter = mMailList.begin();
	while(iter != mMailList.end()) 
	{
		if(NOT_READ == iter->read_status)
			return false;

		iter++;
	}

	return true;
}
//--------------------------------------------------------------------
const bool MailManager::isAnimating() const 
{
	return NULL != mAnimateBox;
}
//--------------------------------------------------------------------
void MailManager::backSendItem(uint index, uint storageIndex)
{
	assert( index >= 0 && index < Mail_Item_Size);
	reqUnlockStorageItem(mSendMail.items[index].mInstanceId);
	mSendMail.items[index].setItem(0, "", -1, 0);
}
//--------------------------------------------------------------------
bool MailManager::_haveItem(const Mail & mail) const
{
	for(uint i = 0; i < mail.items.size(); i++)
	{
		if (0 != mail.items[i].mInstanceId)
			return true;
	}

	return false;
}
//--------------------------------------------------------------------
void MailManager::reqLockStorageItem(uint64 insId) const
{
	assert(insId > 0);
	req_lock_bag_item req;
	req.item_inst_id = insId;
	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void MailManager::reqUnlockStorageItem(uint64 insId) const
{
	assert(insId > 0);
	req_unlock_bag_item req;
	req.item_inst_id = insId;
	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void MailManager::clearSendMail() 
{
	for(uint i = 0; i<mSendMail.items.size(); i++)
	{
		if(0 != mSendMail.items[i].mInstanceId)
			reqUnlockStorageItem(mSendMail.items[i].mInstanceId);
	}

	_resetMail(mSendMail);
}
