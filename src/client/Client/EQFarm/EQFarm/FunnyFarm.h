/*******************************************************************************
* 作者: 麦_Mike
* 时间: 2010-06-11
* 描述: 欢乐农场
*******************************************************************************/
#ifndef FUNNYFARM_H
#define FUNNYFARM_H

#include "FarmObjEvent.h"

#include "gamestate.h"
#include "FarmDecoration.h"
#include "CEGUI.h"
#include "CEGUIEx/CEGUIAnimate.h"
#include "CEGUIEx/CEGUIAnimateBox.h"

class FarmScene;
class FarmPlantUnit::PickObjectUnit;

using namespace std;

/**鼠标动画样式
*/
enum FarmMouseAnimate
{
	MA_PUTBUG = 1,
	MA_WATER,
	MA_PICKBUG,
	MA_ONEKEYHARVEST,
	MA_WEED,
	MA_STEAL,
	MA_HARVEST,
	MA_TILL,
	MA_PUTGRASS,
	MA_USE_PROP,
};

enum FarmCursorState
{
	CS_FARM_NORMAL,
	CS_FARM_MOVE,
	CS_FARM_TILL,			// 铲除枯萎作物
	CS_FARM_WATER,			// 浇水
	CS_FARM_WEED,			// 除草
	CS_FARM_PICKBUG,		// 除虫
	CS_FARM_HARVEST,		// 采摘
	CS_FARM_ONEKEYHARVEST,	// 一键采摘
	CS_FARM_SEEDING,		// 种植
	CS_FARM_PUTBUG,			// 放虫
	CS_FARM_PUTGRASS,		// 放草
	CS_FARM_STEAL,			// 偷
	CS_FARM_USE_PROP,
};

//本人农场数据
struct SelfFarmInfo 
{
	SelfFarmInfo()
		:mSaccount("")
		,mSusername("")
		,mSlev(0)
		,mSexp(0)
		,mfarmState(5)
	{
	}
	string mSaccount;
	string mSusername;
	int	mSlev;
	//farm_pet mSpet;
	int mSexp;
	farm_pet_food	mSpetFood;
	farm_decoration mSdeco;
	int mfarmState;

};

//一键采摘信息
struct AllPickInfo 
{
	AllPickInfo()
		:index(0)
		,dexp(0)
	{
	}
	int index;
	int dexp;
	farm_crop_data cropData;
};

//采摘奖励信息
struct PickAwards 
{
	PickAwards()
		:awardIdx(0)
		,money(0)
		,count(0)
	{
	}

	int awardIdx;
	int money;
	int count;
};

//采摘被狗咬的信息
struct AllHintDatas
{
	AllHintDatas()
		:msg_id(0)
		,hintCount(0)
		,loseGold(0)
	{
	}
	
	int msg_id;
	int hintCount;
	int loseGold;
};
//一件采摘被狗咬延后飘动画的信息
struct HintData 
{
	HintData()
	{
	}

	int hint_index;

};
//一键采摘时飘没钱偷的延后动画数据
struct NomoneyPickData 
{
	NomoneyPickData()
		:plot_id(0)
		,msg_id(0)
	{
	}

	int plot_id;
	int msg_id;
};
//农场装饰实例ID和物品ID结构
struct FarmDecID 
{
	FarmDecID()
		:ins_id("")
		,tplt_id(0)
	{
	}

	std::string ins_id;
	int			tplt_id;
};
//欢乐农场
class FunnyFarm :
	public GameState
{
public:

	static FunnyFarm& getSingleton()
	{
		static FunnyFarm farm;
		return farm;
	}

	virtual void init();
	virtual void update();
	virtual void Fupdate();
	virtual void cleanup();
public:
	/*请求进入场景
	@param 进入的场景的主人*/
	void reqEnterFarm(string owner);
	
	/*通知农场数据*/
	void notiFarmData();

	/**
	*/
	void assartPlot(int index);
	/*请求开垦农田*/
	void reqAssartPlot();

	/* 请求锄地*/
	void reqHoeing(int plotIdx);

	/*请求种植
	@param plotIdx 农田索引
	@param cropTepid 农作物的模板ID*/
	void reqSow(int plotIdx,uint64 item_inst_id);

	/**
	*/
	void reqUsePropStr(int plotIdx, std::string item_inst_id);


	/*请求使用道具
	@param plotIdx 农田索引
	@param cropTepid 农作物的模板ID*/
	void reqUseProp(int plotIdx, uint64 item_inst_id);

	/*请求采摘农作物
	@param owner 农场主
	@param plotIdx 地块索引*/
	void reqPickCrop(string owner, int plotIdx);
	/**
	*/
	void reqPickAllCrops(string owner);
	
	/*请求修改公告
	@param id 公告ID*/
	void reqChgBulletin(int id);

	/*请求获得好友农场的状态
	@param page 好友列表的页码*/
	void reqFriendFarmState();
	void notiFriendFarmState();

	/*通知好友农场的状态
	@param data 状态列表
	*/
	//void notiFriendFarmState(string) 

	/*请求除草除虫浇水,放草放虫
	@param 农场主
	@param 地块索引
	*/
	void reqWeed(string owner, int plotIdx);
	void reqWorn(string owner, int plotIdx);
	void reqWater(string owner, int plotIdx);
	void reqPutGrass(string owner, int plotIdx);
	void reqPutPest(string owner, int plotIdx);

	void reqAnnounceList();
	/*
	请求日志列表，清除日志，请求留言，请求留言列表
	请求清除留言，请求回复留言
	*/
	void reqLogList(int type, const std::string & account);
	void reqclearLog();
	void reqComment(string account,string comment);
	void reqCommentList(const std::string & account);
	void reqClearComment();
	void reqReplyComment(string account,string comment);

	/*咬了
	@param hurt 伤害值
	*/
	void notiBite();
	void notiUsePet();
	void notiStopPet();
	void notiFoodEmpty();
	void notiChgBulletin();

	/*
	通知日志列表，留言列表，留言结果
	*/
	void notiLogList();
	void notiCommentList();
	void notiComment();

	/*
	请求锁定、解锁仓库物品，请求卖出仓库所有物品，请求卖出一种物品，通知全部卖出的价格
	*/
	void reqLockStorageItem( std::string instanceID);
	void reqUnLockStorageItem( std::string instanceID);
	void reqAllSell(int type);
	void reqOneSell( std::string instanceID, int count);

	// 请求买东西
	void reqBuyGoods(int templateId, int type, int count,int pay_type);

	//处理重复购买
	void notiExistPurchase();

	void notiAllSell();

	/*农场设置
	请求农场设置  通知农场设置 请求重置农场设置 
	*/
	void reqSetting();
	void reqSetSetting();
	std::string& replacestring(std::string& source_str,std::string find_str);
	void reqSettingDefault();
	void notiFarmSetting();
	// 农场设置恢复默认值
	void notiSettingDefault();
	void setSetting(const notify_farm_setting & data);
	void setSettingFlag(bool flag){ mResetFlag = flag;}
	bool getSettingFlag(){return mResetFlag;}
	void notiSetResult();
	void notiExpData();
	void notiFarmInfo();
	void notiAllPickData();
	void notiFarmMoney();
	void notiFarmEqGold();
	void notifyFarmAwards();
	/**请求更换装饰
	*/
	void reqChangeDec(std::string instanceID,int tpltID);

	/**请求移除装饰
	*/
	void reqRemoveDec(int tpltID);

	/**红土地相关
	*/
	void reqCanUpgradeLand();
	void reqUpgradeLand();

	/**玩家EQ信息
	*/
	void notiPlayerEqInfo();

/// 农作物逻辑
public:
	//是耕地
	bool isPlot(int index);
	//有作物
	bool hasCrop(int index);
//	bool hasCrop(int index);
	void onClickedUnit(FarmPlantUnit::PickObjectUnit* objunit);

	//设置操作动作状态
	void setAction(int act){mAction = act;}

	//获取动作状态
	int getAction(){return mAction;}

	//获取好友农场的信息
	SelfFarmInfo getSelfFarmInfo(){return mSelfFarmInfo;}
	void removePest(int index);
	void removePickUI(int index);
	void removeFarmPaoPao(int index);
	void removeHavestUI(int index);
	void removeDexpUI(int index);
	void clearLand(FarmScene* farmScene,int index);
private:
	void _updateCrops();

	void _startupPlantUnits();
	int _calculSecs(CropsCtrl& cropctrl);
	void _reqSowAction(string owner, int plotIdx);
	void _reqUseProp(int plotIdx);
	void _addMouseAnimateWnd();
	void _initFarmUI();
	void _addAnimateImg(int num,int time, std::string imgset,std::string imgName,int dx = 10,int dy =50);
	void pushAllPickInfo(int index,int aexp,farm_crop_data cropData);
	void pushNomoneyPickInfo(int plot_id,int msg_id);
	bool popAllPickInfo();
	bool popNomoneyPickInfo();

	DWORD			mCropsStartPos
					,mLastElapsed
					;

	int				mAction;					//操作动作
	CEGUI::Window*	mMouseAnimateWnd;			//鼠标动画窗口
	CEGUI::Animate	mMouseAnimate;
	CEGUI::AnimateBox *mAniWnd;

public:
	string			mAccount;					//农场主人
	string			mLastAcc;					//上次进入的农场玩家
	string			mUsername;
	string			mSeedingImgset;
	string			mSeedingIcon;
	SelfFarmInfo	mSelfFarmInfo;			//本人农场信息
	int				mLev						//农场等级
					,mExp						//农场经验
					,mBulletin					//公告引索
					,mState						// 农场状态
					,mCropNum					//农作物数量
					,mGameGold
					,money						//农场金币
					,mEqmoney					//农场EQ币
					;
	std::queue<AllPickInfo> mAllPickBuffer;
	std::vector<PickAwards> mPickAwardsVec;
	AllHintDatas			mAllHintData;
	std::queue<HintData>	mHint_index_que;
	std::queue<NomoneyPickData> mNomoneyPickData;

	typedef std::vector<CropsCtrl> CropsCtrlList;
	typedef std::vector<CropsCtrl>::iterator CropsCtrlListIter;
	CropsCtrlList	mCropsCtrl;//农作物

	FarmDecoration	mDecoration;	//农场装饰
	//farm_pet		mPet;							//宠物狗

	//农场场景
	FarmScene*		mFarmScene;

	FunnyFarm(void)
		:mResetFlag(false),
		mSetResult(0)
		,mCropsStartPos(0)
		,mpushFlag(0)
		,mIsOneKeyHavSta(false)
		,m_curOperateType(CS_FARM_NORMAL)
//		,mClickHavest(false)
	{}
	
	virtual ~FunnyFarm(void)
	{}

	//---------------农场设置------------------
	bool mResetFlag;
	int  mSetResult;
	//
	int mpushFlag;
	bool mIsOneKeyHavSta;
	stime mCommPetFoodPeriod;
//	bool mClickHavest;

public:
	///*得到农作物
	//@param idx 农作物索引
	//*/
	//farm_crop_data& getCrop(int idx)
	//{
	//	assert(idx>=0 && idx <mCropNum);
	//	return mCropsCtrl[idx].crop;
	//}
	/*得到农作物控制器
	@param idx 农作物索引
	*/
	const CropsCtrl& getCropCtrl(int idx)
	{
		assert(idx>=0 && idx <mCropNum);
		return mCropsCtrl[idx];
	}

public:
	/**设置农场鼠标状态
	*/
	bool hasDamage(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data,bool& hasDrought,bool& hasPest,bool& hasGrass,bool& hasGain);
	bool setSelfFarmState(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data);
	void setFarmCurState(int sta, std::string imageset = "", std::string icon ="");
	bool setCurStaCur(bool hasPest, bool hasGrass,bool hasDrought);
	void setSeedingCur(int sta, std::string imageset, std::string icon);
	void _updatePos(POINT posabs);
	void hideMouseAnimateWnd();
	void playMouseAnimate();
	void getMouseAniInfoFromXml(int id,int dx=10,int dy=50);
	bool canPick(int index);
	void setPushFlag(int fg);
	void setHarvestDymanicUI(int index,int templtID,int cropsnum);
	void setHintDymanicUI(int index,int imageID,int lose_gold);
	int getFarmMoney(){return money;}
	int getFarmEqMoney(){return mEqmoney;}
	void checkAllpickAward();
	void checkAllHintData();
	void saveCurDec_tolua(farm_decoration dec);
	int getSelfDogFoodRemainSecs();
	int getCommDogFoodRemainSecs();

// 农场好友头像用到(add by hezhr)
public:
	void insertAccountList(std::string account);
	void reqEqUsersInfo(void);
protected:
	std::vector<std::string> m_accountList;

// 农场任务(2011/4/14 add by hezhr)
public:
	void reqFarmTaskList(void);
	void handle_farm_task_list(void);
	void reqFarmTaskComplete(int task_id);
	void handle_farm_task_complete(void);
	int getFarmTaskCount(void);
	int getFarmTaskIdByIndex(int index);
	bool existFarmTask(int task_id);

// 农场防沉迷(2011/6/23 add by hezhr)
public:
	void handle_wallow_time(void);

protected:
	std::vector<player_task> m_farmTaskList;
	FarmCursorState m_curOperateType;	// 当前鼠标操作类型

};

#define sFunnyFarm FunnyFarm::getSingleton()
#endif