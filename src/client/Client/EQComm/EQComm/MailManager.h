/******************************************************************
* 作者: XiaoMing
* 时间: 2010-09-01
* 描述: 邮件管理
*******************************************************************/
#ifndef MAIL_MANAGER_H
#define MAIL_MANAGER_H

#include "MsgHandleRegist.h"

struct mail_info;

namespace CEGUI
{
	class AnimateBox;
	class EventArgs;
}

class ByteArray;


typedef unsigned __int64  uint64;
static const uint Mail_Item_Size = 6;
static const uint Mail_Count_PerPage = 6;
static const uint Max_Mail_Count = 120;

struct MailItem
{
public:
	MailItem(uint64 insId = 0, std::string effTime = "", int temId = -1, int count = 0)
		:mInstanceId(insId)
		,mEffectTime(effTime)
		,mTemplateId(temId)
		,mCount(count)
	{

	}

	void setItem(uint64 insId = 0, int temId = -1, int count = 0);

	void setItem(uint64 insId = 0, std::string effTime = "", int temId = -1, int count = 0);

	bool operator == (const uint64 insId) const;
public:
	uint64 mInstanceId;
	int mTemplateId;
	int mCount;
	std::string mEffectTime;	// 到期时间，格式：yyyy,mm,dd,hh,nn,ss,xxx（hezhr）
};

typedef std::vector<MailItem> MailItems;

struct Mail 
{
public:
	Mail()
		:id(0)
		,type(0)
		,read_status(0)
		,remain_time(0)
		,name("")
		,account("")
		,title("")
		,content("")
		,isReqContent(false)
	{
		items.reserve(Mail_Item_Size);
	}
	// 此函数用与std::find 若提供的mailId与邮件id
	// 相等，则认为存在此邮件
	bool operator == (const uint64 mailId) const;

	bool operator < (const Mail & cmpMail) const;

public:
	uint64 id;
	int type;
	int read_status;
	int remain_time;
	bool isReqContent; // 该邮件的内容是否请求过
	std::string name;
	std::string account;
	std::string title;
	std::string content;
	MailItems items;
};

typedef std::list<Mail> MailList;

class MailManager:public Ogre::Singleton<MailManager>, public MemberMsgHandleRegistHelper
{
	enum MailType
	{
		CurMail,	// 当前查看邮件
		SendMail,   // 发件箱邮件
	};

	enum MailState
	{// 与服务器的枚举必须相同。
	 // 由于enum.h中的内容更新频繁 故不使用	
		NIL = 0,
		NOT_READ = 1,
		READ_TITLE = 2,
		READ_CONTENT = 3
	};

	enum AddPosition
	{
		Front,	// 
		Back,   // 
	};
public:
	MailManager();
	~MailManager();
	
	/* 要通过返回的Mail改变邮件的阅读状态，暂时不
		不返回值设置为Const
	*/
	Mail* getMail(uint page, uint offset);
	//Mail& getMail(uint64 mailId);
	void addMail(const Mail & mail, AddPosition ps = Front);
	void addMail(const mail_info & mailInfo, AddPosition ps = Front);
	void deleteMail(uint64 mailId);
	void deleteMail(uint index);

	
	/** 设置发送邮件的基本信息
			@remarks
				此函数透露给LUA ，所以参数不用引用
	*/
	void sendMail(std::string acc, std::string title, std::string content) const;
	/** 设置你要发送的邮件的附件
			@remarks
				由仓库的索引获得物品的instanceId
			@params
				index 邮件附件栏索引
				storageIndex 仓库索引
	*/
	void setSendMailItem(uint index, uint storageIndex);

	/** 请求锁定和解锁仓库物品
	*/
	void reqLockStorageItem(uint64 insId) const;
	void reqUnlockStorageItem(uint64 insId) const;


	/* 取回发件箱中的物品
		@parms index 发件箱索引
			   storageIndex 背包索引（全局）
	*/
	void backSendItem(uint index, uint storageIndex);

	/** 获得发件箱附件栏物品的模板ID 数量
	*/
	int  getSendMailItemId(uint index) const;
	uint getSendMailItemCount(uint index) const;
	std::string getSendMailItemEffectTime(uint index);
	/*
		@return 存在，返回发件箱格子索引，否则返回-1 
	*/
	int  existItem(uint64 instanceId) const;
	void swapItem(int toIndex, int fromIndex);
	//更新物品栏（hezhr）
	void updateItems(void);

	void reqMailList();
	void reqKnowMailList(uint startIndex, uint endIndex);
	/* 获得当前页的最后一个索引
	*/
	uint getPageLastIndex(uint page) const;
	uint getTotalPage() const;
	//bool handleByMailMsg(UINT16 msgType, ByteArray& recBuff);

	void handle_send_mail_success(MsgPack &recBuff);
	void handle_mail_list(MsgPack &recBuff);
	void handle_mail_content(MsgPack &recBuff);
	void handle_delete_mail(MsgPack &recBuff);
	void handle_delete_mail_item(MsgPack &recBuff);
	void handle_add_mail(MsgPack &recBuff);
	void handle_mail_not_read(MsgPack &recBuff);
	void handle_reject_mail_success(MsgPack &recBuff);

	void reqMailContent(uint page, uint offset);
	MailItem* getCurMailItem(uint index);
	std::string getCurMailItemEffectTime(uint index);
	uint getCurMailItemCount() const;
	void reqGetMailItem(uint itemIndex) const;

	void reqDeleteMail() const;
	void reqReturnMail() const;

	int  getCurMailType() const {return mCurMail->type;}
	const Mail* getCurMail() const {return mCurMail;}

	const bool isAllReaded() const;
	const bool isAnimating() const;

	const bool isFull() const {return mMailList.size() > Max_Mail_Count;}

	/* 有邮件未阅读时 显示动画
	*/
	void addAnimation();
	void removeAnimation();
	
	bool sendMailHaveItem() const {return _haveItem(mSendMail);}
	bool curMailHaveItem() const {return _haveItem(*mCurMail);}
	void clearSendMail();
private:
	bool _haveItem(const Mail & mail) const;
	void _resetMail(Mail & mail);
	void _deleteCurMailItem(uint64 mailId, uint64 insId);
	void _sortMailList();
	bool clickNewMailPaoPao(const CEGUI::EventArgs& args);
public:
	bool mReqMailListFlag;
	uint mStartOffset; //第一封邮件在邮件列表的偏移
private:
	MailList				mMailList;
	Mail					mSendMail; // 要发送的邮件的附件
	Mail*					mCurMail;  // 当前阅读的邮件
	MailList::iterator		mCurMailIter; // 当前邮件在列表中的位置
	MailList::iterator		mCurPageFirstIter;
	CEGUI::AnimateBox*      mAnimateBox;
};
#endif