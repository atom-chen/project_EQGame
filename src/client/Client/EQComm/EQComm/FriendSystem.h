/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		13/5/2010	15:32
* 文件: 	FriendSystem.h
* 描述:		好友系统
******************
*********************************************************************/
#ifndef FRIENDSYSTEM_H
#define FRIENDSYSTEM_H

#include "GameSocket.h"
#include "NetPacket.h"
#include <Ogre.h>
#include "CEGUI.h"
#include "FriendPhoto.h"
#include "MsgHandleRegist.h"

enum friendOnlineState
{
	ONLINE = 1,
	OFFLINE,
};

enum winID
{
	TREEID = 1,
	EDITBOXID,
};
//好友额外数据
struct FriendExtendData
{
	int GarbageCnt;
	int MagicBoxCnt;

	FriendExtendData(void)
	{
		GarbageCnt = 0;
		MagicBoxCnt = 0;
	}
};
typedef stdext::hash_map<std::string, FriendExtendData> FriendExtendDataMap;

/**好友排序
*/
struct friendSort 
{
	bool operator()(const friend_info& f1, const friend_info& f2)
	{
		if (f1.is_online == f2.is_online)
		{
			return f1.basic_data.username < f2.basic_data.username;
		}
		else
		{
			return f1.is_online < f2.is_online;
		}
	}
};
/**
*/
//GM好友
struct GMFriend 
{
	GMFriend()
		: house_id(0)
	{}
	std::string account;
	std::string nick;
	std::string full_name;
	int			house_id;
};

/**
*/
class GMFriendSys
{
public:
	GMFriendSys();
	~GMFriendSys(){};

public:
	void loadGMFriendInfo();
	GMFriend& getGmFriendInfo(){return m_gmfriend;}
protected:
private:
	GMFriend m_gmfriend;
};

/**好友系统
*/
class FriendSystem : public Ogre::Singleton<FriendSystem>, public MemberMsgHandleRegistHelper
{
public:
	FriendSystem(void);
	virtual ~FriendSystem(void);

	/**向服务端申请加为好友
	*/
	BOOL reqAddasFriend();
	/**
	*/
	BOOL reqAsFriend(std::string reqacc);

	/**	请求好友列表
	*/
	BOOL reqFriendList();

	/** 请求删除好友
	*/
	BOOL reqRemoveFriend(std::string );

	/**
	*/
	bool removeFriendInfo(std::string del_acc);

	/**请求进入好友家
	*/
	BOOL reqEnterFriendHouse(std::string friendAcc);

	/**
	*/
	BOOL reqEnterFriendHouseByLua();

	/**
	*/
	bool reqInviteEnterHouse(std::string friendAcc);
	bool reqInviteSelFriendHouse();

	bool reqHouseListFromFriendList();
	bool reqHouseListFromCircleMenu();

	/**
	*/
	bool reqInviteEnterHouseByLua();

	/**踢人
	*/
	bool reqKickPlayer(std::string acc);

	/**
	*/
	bool reqKickPlayerByLua();

	/**
	*/
	bool changeOnlineState(std::string account, int stateVal);


	/**接收返回加为好友消息
	*/
	void notifyAddFriend(ByteArray& recBuff);

	/**接收返回好友列表消息
	*/
	void notifyFriendList(ByteArray& recBuff);

	/**在表头插入GM好友信息
	*/
	void insertGmFriendInfo(friend_info& gm_friendinfo);

	/**	接受删除好友信息
	*/
	void notifyRemoveFriend(ByteArray& recBuff);

	/**
	*/
	void notifyFriendOnline(ByteArray& recBuff);

	/**
	*/
	void notifyFriendOffline(ByteArray& recBuff);

	/**
	*/
	void showList();

	/**
	*/
	BOOL popUpMgrWin();

	/**
	*/
	int getFriendOnlineState(std::string friendAcc);

	/**
	*/
	const std::string getFriendAccount(){return mFriendAccount;}

	/**查找返回好友信息
	*/
	const player_basic_data* searchFriendBasicInfo(std::string acc);

	/**查找返回好友信息（根据好友名字）(hezhr)
	*/
	const player_basic_data* searchFriendBasicInfoByName(std::string name);

	/**查找返回好友名字
	*/
	const std::string getFriendName(std::string acc)
	{
		const player_basic_data* data = searchFriendBasicInfo(acc);
		if (data)
			return data->username;
		return "";
	}

	/**查找返回好友账号（根据好友名字）(hezhr)
	*/
	const std::string getFriendAccount(std::string name)
	{
		const player_basic_data* data = searchFriendBasicInfoByName(name);
		if (data)
			return data->account;
		return "";
	}

	/**插入好友头像(add by hezhr)
	*/
	void insertFriendPhoto(const std::string& account, const std::string& photoData="");

	/**移除好友头像(add by hezhr)
	*/
	void removeFriendPhoto(const std::string& account);

	/**获取好友头像名字(add by hezhr)
	*/
	std::string getFriendPhotoName(const std::string& account);

	/**好友头像是否需要更新(add by hezhr)
	*/
	bool needUpdateFriendPhoto(const std::string& account);

	/**设置好友头像需要更新(add by hezhr)
	*/
	void setNeedUpdateFriendPhoto(const std::string& account, bool update=true);

	/*设置好友头像(add by hezhr)
	*/
	void setFriendPhoto(const std::string& account, const std::string& winname);

	/**设置当前列表的好友头像(add by hezhr)
	*/
	void setCurListPhotos(void);

	//请求预览好友房屋
	void reqPreviewFHousedata(int idx, std::string &acc);
	void reqPreviewFHousedataByFriendList(int idx);
	void reqPreviewFHousedataByCircleMenu(int idx);

	void handle_preview_fhouse(MsgPack &recBuff);
	void handle_friend_garbage(MsgPack &recBuff);

public:
	/**获得指定页的数据
	*/
	void setShowList(int page,std::vector<friend_info> *friend_vec,int maxSize);

	/**获得每一页的最大条数
	*/
	int getPageCount(int page, int maxSize);

	/**获得页数
	*/
	int getTotalPage(std::vector<friend_info> &friend_vec,int maxSize);

	/**
	*/
	bool setPushStaUI(const CEGUI::EventArgs& args);

	void searchEx( const char *key );
	void search_matchKey(const char *key, int mode );

	std::string getSearchExResult(UINT idx);
	UINT getSearchExCnt() { return mSearchExArr.size(); }
	int getSearchExMaxCnt() { return mSearchExMaxCnt; }
	void setSearchExMaxCnt(int maxCnt) { mSearchExMaxCnt = maxCnt; }
	std::string getGMFriendAcc(){return mGMFriendInfo.basic_data.account;}

protected:
	/**
	*/
	bool isHovering(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleRootKeyDown(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventMouseClick(const CEGUI::EventArgs& args);
	/**
	*/
	bool handleEventMouseDbClick(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventRemveFriend(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventPensonalChat(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventVisitFriendHouse(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventInviteEnterHouse(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventViewFriendInfo(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventSendMail(const CEGUI::EventArgs& args);

	/**
	*/
	bool handleEventGoToFirstPage(const CEGUI::EventArgs& args);
	/**
	*/
	bool handleEventRefrash(const CEGUI::EventArgs& args);
	/**
	*/
	bool handleEnterGMFriendHouse(const CEGUI::EventArgs& args);
	/**
	*/
	bool nextPage(const CEGUI::EventArgs& args);

	/**
	*/
	bool prePage(const CEGUI::EventArgs& args);
	/**
	*/
	void setNumTxt();
	/**
	*/
	bool resetBtnSta();
	/**
	*/
	void updateBtnStaUI();

	/**好友列表排序
	*/
	//bool friendSort(const friend_info& f1, const friend_info& f2);

	/**注册好友面板事件
	*/
	bool registerEvent();

	/**注册弹出菜单按钮事件
	*/
	bool registerButtonEvent();

	private:
		void _initGMFriendInfo();
private:
	std::vector<friend_info> mSearchExArr;
	int mSearchExMaxCnt;

	std::string		 mFriendAccount;
	CEGUI::Window*	 mfLWin;
	//选中的好友的信息
	friend_info mselectedItem;
	//好友数组
	std::vector<friend_info> mFriendvec;
	//显示好友列表
	std::vector<friend_info> mShowList;
	//好友头像数组(add by hezhr)
	std::vector<FriendPhoto> m_friendPhotoVec;
	//好友额外数据
	FriendExtendDataMap m_friendExData;
	//GM好友信息
	friend_info		mGMFriendInfo;
	//
	GMFriendSys*	mGMFriendSys;
	//好友列表页数
	int mTotalPage;
	//当前页数
	int mcurPage;

	int mSelectID;
	int mLastSelectID;
	CEGUI::Window*	 mLastSelBtn;

	std::string mselectedItemName;
	std::string mLastSelectName;
	//
	bool mIsSearchMode;
};
#endif