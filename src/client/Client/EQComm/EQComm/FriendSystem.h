/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		13/5/2010	15:32
* �ļ�: 	FriendSystem.h
* ����:		����ϵͳ
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
//���Ѷ�������
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

/**��������
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
//GM����
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

/**����ϵͳ
*/
class FriendSystem : public Ogre::Singleton<FriendSystem>, public MemberMsgHandleRegistHelper
{
public:
	FriendSystem(void);
	virtual ~FriendSystem(void);

	/**�����������Ϊ����
	*/
	BOOL reqAddasFriend();
	/**
	*/
	BOOL reqAsFriend(std::string reqacc);

	/**	��������б�
	*/
	BOOL reqFriendList();

	/** ����ɾ������
	*/
	BOOL reqRemoveFriend(std::string );

	/**
	*/
	bool removeFriendInfo(std::string del_acc);

	/**���������Ѽ�
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

	/**����
	*/
	bool reqKickPlayer(std::string acc);

	/**
	*/
	bool reqKickPlayerByLua();

	/**
	*/
	bool changeOnlineState(std::string account, int stateVal);


	/**���շ��ؼ�Ϊ������Ϣ
	*/
	void notifyAddFriend(ByteArray& recBuff);

	/**���շ��غ����б���Ϣ
	*/
	void notifyFriendList(ByteArray& recBuff);

	/**�ڱ�ͷ����GM������Ϣ
	*/
	void insertGmFriendInfo(friend_info& gm_friendinfo);

	/**	����ɾ��������Ϣ
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

	/**���ҷ��غ�����Ϣ
	*/
	const player_basic_data* searchFriendBasicInfo(std::string acc);

	/**���ҷ��غ�����Ϣ�����ݺ������֣�(hezhr)
	*/
	const player_basic_data* searchFriendBasicInfoByName(std::string name);

	/**���ҷ��غ�������
	*/
	const std::string getFriendName(std::string acc)
	{
		const player_basic_data* data = searchFriendBasicInfo(acc);
		if (data)
			return data->username;
		return "";
	}

	/**���ҷ��غ����˺ţ����ݺ������֣�(hezhr)
	*/
	const std::string getFriendAccount(std::string name)
	{
		const player_basic_data* data = searchFriendBasicInfoByName(name);
		if (data)
			return data->account;
		return "";
	}

	/**�������ͷ��(add by hezhr)
	*/
	void insertFriendPhoto(const std::string& account, const std::string& photoData="");

	/**�Ƴ�����ͷ��(add by hezhr)
	*/
	void removeFriendPhoto(const std::string& account);

	/**��ȡ����ͷ������(add by hezhr)
	*/
	std::string getFriendPhotoName(const std::string& account);

	/**����ͷ���Ƿ���Ҫ����(add by hezhr)
	*/
	bool needUpdateFriendPhoto(const std::string& account);

	/**���ú���ͷ����Ҫ����(add by hezhr)
	*/
	void setNeedUpdateFriendPhoto(const std::string& account, bool update=true);

	/*���ú���ͷ��(add by hezhr)
	*/
	void setFriendPhoto(const std::string& account, const std::string& winname);

	/**���õ�ǰ�б�ĺ���ͷ��(add by hezhr)
	*/
	void setCurListPhotos(void);

	//����Ԥ�����ѷ���
	void reqPreviewFHousedata(int idx, std::string &acc);
	void reqPreviewFHousedataByFriendList(int idx);
	void reqPreviewFHousedataByCircleMenu(int idx);

	void handle_preview_fhouse(MsgPack &recBuff);
	void handle_friend_garbage(MsgPack &recBuff);

public:
	/**���ָ��ҳ������
	*/
	void setShowList(int page,std::vector<friend_info> *friend_vec,int maxSize);

	/**���ÿһҳ���������
	*/
	int getPageCount(int page, int maxSize);

	/**���ҳ��
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

	/**�����б�����
	*/
	//bool friendSort(const friend_info& f1, const friend_info& f2);

	/**ע���������¼�
	*/
	bool registerEvent();

	/**ע�ᵯ���˵���ť�¼�
	*/
	bool registerButtonEvent();

	private:
		void _initGMFriendInfo();
private:
	std::vector<friend_info> mSearchExArr;
	int mSearchExMaxCnt;

	std::string		 mFriendAccount;
	CEGUI::Window*	 mfLWin;
	//ѡ�еĺ��ѵ���Ϣ
	friend_info mselectedItem;
	//��������
	std::vector<friend_info> mFriendvec;
	//��ʾ�����б�
	std::vector<friend_info> mShowList;
	//����ͷ������(add by hezhr)
	std::vector<FriendPhoto> m_friendPhotoVec;
	//���Ѷ�������
	FriendExtendDataMap m_friendExData;
	//GM������Ϣ
	friend_info		mGMFriendInfo;
	//
	GMFriendSys*	mGMFriendSys;
	//�����б�ҳ��
	int mTotalPage;
	//��ǰҳ��
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