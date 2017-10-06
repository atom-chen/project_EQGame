/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		19/4/2011	14:39
* �ļ�: 	ItemInteractSys.h
* ����:		EQ��԰��ũ������Ʒ����ϵͳ
******************
*********************************************************************/
#ifndef ItemInteractSys_H
#define ItemInteractSys_H

#include "EQMainPrerequisites.h"
#include "ItemPack.h"
#include "NetPacket.h"
#include "MsgHandleRegist.h"
/**������Ʒ��
*/
class InteractItem 
{
public:
	/**Construct
	*/
	InteractItem(){}
	InteractItem(int idx, std::string name, int count, bool interact);
	~InteractItem(){}

public:
	/**
	*/
	int getItemID();

	/**
	*/
	std::string getItemName();

	/**
	*/
	int getItemCount();

	/**
	*/
	bool isInteract();

private:
	int			mItemID;
	std::string mItemName;
	int			mCount;
	bool		mIsInteract;
};


/**
*/
#define DEFAULT_IITGRID_SIZE 30
/**������Ʒ����ϵͳ
*/
class ItemInteractSys
{
public:
	/**���캯������������
	*/
	ItemInteractSys();
	~ItemInteractSys(){}

	static ItemInteractSys& getSingleton()
	{
		static ItemInteractSys mIITSys;
		return mIITSys;
	}

public:
	/**
	*/


public:
	/**
	*/
	bool addItem(int idx, item& item_data,int count, int lock);

	/**
	*/
	void resetItemList(int size);
	/**
	*/
	void clearItemList();
	/**
	*/
	bool addmoney(int money);
	/**
	*/
	const Item& getItem();

	/**
	*/
	int getItemID(uint index);
	/**
	*/
	std::string getItemDelTime(uint index);

	/**
	*/
	std::string getItemInsIdStr(uint index);

	/**
	*/
	uint64 getItemInstanceID(uint index);
	/**
	*/
	std::string transformInsIdToStr(uint64 insID);
	/**
	*/
	uint64 transformInsIdToUi64(std::string insId_str);
	/**
	*/
	const ItemGrid* getItemGird(uint index);

	/**
	*/
	int getPackCurPage();
	/**
	*/
	int getPackPageCount();
	/**
	*/
	bool setPackCurPage(int page);

	/**
	*/
	int getmoney();
	/**
	*/
	void notifyDataUpdate();

	/**
	*/
	void setCurpage(int page);
	int getCurpage(){return mCurPage;}

protected:
private:
	void _init();
	void _setItemSize(uint size);

private:
	ItemPack mItemList;
	int      mMoney;
	uint		mItemSize;
	//
	int		mCurPage;

};

/**���ݷ��ͽ�����
*/
class IITDataConnect : public MemberMsgHandleRegistHelper
{
public:
	/**
	*/
	enum InteractSys_Type
	{
		IST_Farm = 1,
	};
	/**
	*/
	enum Notify_Active_Type
	{
		NOACTIVE,
		ACTIVE,
	};

public:
	/**
	*/
	void reqFarmisActive();
	/**
	*/
	void reqSwithItem(int sourceType,std::string insIdStr, int targetType);
	/**
	*/
	void reqMoveMoney(int money,int fromType,int toType);
	/**
	*/
	void rec_msg_FarmIsActive(MsgPack &recBuff);
	/**
	*/
	void rec_msg_FarmMoney(MsgPack &recBuff);
	/**
	*/
	void rec_msg_update_playercoin(MsgPack &recBuff);
	/**
	*/
	void send();
	/**
	*/
	void recv();

	/**
	*/
	bool notifyInteractItems(notify_package& pack,std::string acc);
private:
	void _notify();
	void regist_msgHandler();

public:
	/**
	*/
	IITDataConnect();
	~IITDataConnect(){}
	/**
	*/
	static IITDataConnect& getSingleton()
	{
		static IITDataConnect mIitDCon;
		return mIitDCon;
	}

};

#define sIITDataConnect IITDataConnect::getSingleton()
#define  sIITSys ItemInteractSys::getSingleton()
#endif