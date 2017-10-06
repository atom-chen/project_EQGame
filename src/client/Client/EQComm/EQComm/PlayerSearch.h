/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		31/3/2011	14:31
* 文件: 	PlayerSearch.h
* 描述:		玩家搜索相关
******************
*********************************************************************/
#ifndef PlayerSearch_H
#define PlayerSearch_H

#include "EQMainPrerequisites.h"
#include "MsgHandleRegist.h"
#include "NetPacket.h"
#include "PlayerSearchMode.h"

//前置声明
class PlayerSearchOutput;

class SearchPlayerResult
{
public:
	/**所属地区结构
	*/
	typedef struct  
	{
		std::string mprovince;
		std::string mcity;
	} AreaType;

public:
	SearchPlayerResult(){}
	/**
	*/
	SearchPlayerResult(std::string acc);
	/**
	*/
	~SearchPlayerResult(){}

public:
	/**
	*/
	std::string getNickname(){return mNick_name;}
	/**
	*/
	std::string getAccount(){return mAccount;}
	/**
	*/
	AreaType getArea(){return mArea;}
	/**
	*/
	std::string getProvince(){return mArea.mprovince;}
	/**
	*/
	std::string getCity(){return mArea.mcity;}
	/**
	*/
	void setNickname(std::string nick){mNick_name = nick;}
	/**
	*/
	void setAccount(std::string acc){mAccount = acc;}
	/**
	*/
	void setArea(std::string province,std::string city){mArea.mprovince = province;mArea.mcity = city;}
protected:

private:
	std::string	mNick_name;
	std::string	mAccount;
	AreaType	mArea;
};

/**
*/
class PlayerSearch : public MemberMsgHandleRegistHelper
{
public:
	typedef std::vector<SearchPlayerResult> PlayerList;

	enum
	{
		BY_NICK,
		BY_MAIL,
	};

public:
	/**request
	*/
	void req_search(std::string search_key, int mode);

	/**
	*/
	void searchway(PlayerSearchModeBase* psmb_ptr, std::string search_key);

	/**recieve
	*/
	void rec_msg_searchResult(MsgPack &recBuff);

	/**save
	*/
	void save_searchResult(std::string acc);

	/**
	*/
	void addEQInfo(std::string eq_acc, std::string full_name,std::string nick,std::string eq_province,std::string eq_city);
	/**
	*/
	void clearPlayerList(){mPlayer_list.clear();}
	/**
	*/
	bool isSearchEmpty(){return mPlayer_list.empty();}
	/**
	*/
	void updateSearchResult(PlayerList& playerList);

	/**
	*/
	void subscribe(PlayerSearchOutput* subscriber);

	/**
	*/
	void desubscribe(PlayerSearchOutput* subscriber);

protected:

private:
	/**register handler
	*/
	void regist_msgHandle();

private:
	std::list<PlayerSearchOutput*> msubscribers;
	PlayerList mPlayer_list;


public:
	PlayerSearch();
	~PlayerSearch();

	static PlayerSearch& getSingleton()
	{
		static PlayerSearch ps;
		return ps;
	}
	
};

#endif