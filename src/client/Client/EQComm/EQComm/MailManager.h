/******************************************************************
* ����: XiaoMing
* ʱ��: 2010-09-01
* ����: �ʼ�����
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
	std::string mEffectTime;	// ����ʱ�䣬��ʽ��yyyy,mm,dd,hh,nn,ss,xxx��hezhr��
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
	// �˺�������std::find ���ṩ��mailId���ʼ�id
	// ��ȣ�����Ϊ���ڴ��ʼ�
	bool operator == (const uint64 mailId) const;

	bool operator < (const Mail & cmpMail) const;

public:
	uint64 id;
	int type;
	int read_status;
	int remain_time;
	bool isReqContent; // ���ʼ��������Ƿ������
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
		CurMail,	// ��ǰ�鿴�ʼ�
		SendMail,   // �������ʼ�
	};

	enum MailState
	{// ���������ö�ٱ�����ͬ��
	 // ����enum.h�е����ݸ���Ƶ�� �ʲ�ʹ��	
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
	
	/* Ҫͨ�����ص�Mail�ı��ʼ����Ķ�״̬����ʱ��
		������ֵ����ΪConst
	*/
	Mail* getMail(uint page, uint offset);
	//Mail& getMail(uint64 mailId);
	void addMail(const Mail & mail, AddPosition ps = Front);
	void addMail(const mail_info & mailInfo, AddPosition ps = Front);
	void deleteMail(uint64 mailId);
	void deleteMail(uint index);

	
	/** ���÷����ʼ��Ļ�����Ϣ
			@remarks
				�˺���͸¶��LUA �����Բ�����������
	*/
	void sendMail(std::string acc, std::string title, std::string content) const;
	/** ������Ҫ���͵��ʼ��ĸ���
			@remarks
				�ɲֿ�����������Ʒ��instanceId
			@params
				index �ʼ�����������
				storageIndex �ֿ�����
	*/
	void setSendMailItem(uint index, uint storageIndex);

	/** ���������ͽ����ֿ���Ʒ
	*/
	void reqLockStorageItem(uint64 insId) const;
	void reqUnlockStorageItem(uint64 insId) const;


	/* ȡ�ط������е���Ʒ
		@parms index ����������
			   storageIndex ����������ȫ�֣�
	*/
	void backSendItem(uint index, uint storageIndex);

	/** ��÷����丽������Ʒ��ģ��ID ����
	*/
	int  getSendMailItemId(uint index) const;
	uint getSendMailItemCount(uint index) const;
	std::string getSendMailItemEffectTime(uint index);
	/*
		@return ���ڣ����ط�����������������򷵻�-1 
	*/
	int  existItem(uint64 instanceId) const;
	void swapItem(int toIndex, int fromIndex);
	//������Ʒ����hezhr��
	void updateItems(void);

	void reqMailList();
	void reqKnowMailList(uint startIndex, uint endIndex);
	/* ��õ�ǰҳ�����һ������
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

	/* ���ʼ�δ�Ķ�ʱ ��ʾ����
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
	uint mStartOffset; //��һ���ʼ����ʼ��б��ƫ��
private:
	MailList				mMailList;
	Mail					mSendMail; // Ҫ���͵��ʼ��ĸ���
	Mail*					mCurMail;  // ��ǰ�Ķ����ʼ�
	MailList::iterator		mCurMailIter; // ��ǰ�ʼ����б��е�λ��
	MailList::iterator		mCurPageFirstIter;
	CEGUI::AnimateBox*      mAnimateBox;
};
#endif