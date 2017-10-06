/******************************************************************
* 作者: 麦_Mike
* 时间: 2010-05-23
* 描述: 游戏小模块管理
*******************************************************************/
#ifndef SMALLMODULES
#define SMALLMODULES

#include <OgreTimer.h>
#include "MsgHandleRegist.h"
#include "NetPacket.h"
#include "CEGUI.h"

class ByteArray;
struct stime;

const UINT16 MAX_COUNT = 9;

typedef struct tagHouseData
{
	std::string name;
	std::string buy_time;
	uint level;
	uint price;
	uint max_player;
	uint type;
	uint used;
	uint empty;
	int  template_id;
	uint64 instance_id;
} THouseData, *LPTHouseData;

class SmallModules : public Ogre::Singleton<SmallModules>, public MemberMsgHandleRegistHelper
{
public:
	SmallModules(void);
	~SmallModules(void);

	void init();
	void update();
	void cleanup(){}

	void test();
//***********************************************************************
// 通用函数。暂时存放于此
	std::string getTimeString(int y, int m, int d, int h, int min, int s);
	std::string getTimeString(stime time);

//***********************************************************************
// 检测响应时间  邀请相关 交易相关
	enum CheckAnswerType
	{
		ANSWER_INVITE,
		ANSWER_TRADE,

		ANSWER_NONE,
	};

	// 检测响应时间
	void checkAnswerTime();

	void setAnswerType(CheckAnswerType answerType){mAnswerType = answerType;}

	CheckAnswerType getAnswerType(){return mAnswerType;}

	void startCheckAnswer(CheckAnswerType answerType);

	void endCheckAnswer();

	void resetTimer(){mAnswerTimer.reset();}

	Ogre::Timer mAnswerTimer;		   // 响应时间

	CheckAnswerType mAnswerType;

	static bool ifReviewHouseing;

//***********************************************************************
// 邀请模块
public:

	void handle_invitation(MsgPack &recBuff);
	void handle_cancel_invitation(MsgPack &recBuff);
	void handle_start_trade(MsgPack &recBuff);
	void handle_update_trade_money(MsgPack &recBuff);
	void handle_other_update_trade_money(MsgPack &recBuff);
	void handle_other_put_trade_item(MsgPack &recBuff);
	void handle_other_unput_trade_item(MsgPack &recBuff);
	void handle_put_trade_item(MsgPack &recBuff);
	void handle_unput_trade_item(MsgPack &recBuff);
	void handle_swap_trade_item(MsgPack &recBuff);
	void handle_other_swap_trade_item(MsgPack &recBuff);
	void handle_sure_trade(MsgPack &recBuff);
	void handle_other_sure_trade(MsgPack &recBuff);
	void handle_cancel_trade(MsgPack &recBuff);
	void handle_trade_success(MsgPack &recBuff);
	void handle_player_health_status(MsgPack &recBuff);
	void handle_buy_house(MsgPack &recBuff);
	void handle_notify_buy_new_house_success(MsgPack &recBuff);
	void handle_domestic_result(MsgPack &recBuff);
	void handle_end_domestic(MsgPack &recBuff);
	void handle_start_domestic(MsgPack &recBuff);
	void handle_other_player_info(MsgPack &recBuff);
	void handle_house_basic_data(MsgPack &recBuff);
	void handle_house_clean(MsgPack &recBuff);
	void handle_clear_house_log(MsgPack &recBuff);
	void handle_set_house_welcome_words(MsgPack &recBuff);
	void handle_set_house_permission(MsgPack &recBuff);
	void handle_get_send_express_vec(MsgPack &recBuff);
	void handle_get_send_box_vec(MsgPack &recBuff);
	void handle_get_receive_box_vec(MsgPack &recBuff);
	void handle_get_viewed_box_vec(MsgPack &recBuff);
	void handle_get_receive_express_vec(MsgPack &recBuff);
	void handle_get_viewed_express_vec(MsgPack &recBuff);
	void handle_work_result(MsgPack &recBuff);
	void handle_end_work(MsgPack &recBuff);
	void handle_start_work(MsgPack &recBuff);
	void handle_start_change_looks(MsgPack &recBuff);
	void handle_change_looks(MsgPack &recBuff);
	void handle_end_change_looks(MsgPack &recBuff);
	void handle_show_picture(MsgPack &recBuff);
	void handle_house_list(MsgPack &recBuff);
	void handle_change_house(MsgPack &recBuff);
	void handle_wallow_time(MsgPack &recBuff);
	void handle_disease_special_event(MsgPack &recBuff);
	void handle_express_vec(MsgPack &recBuff);
	void handle_send_express(MsgPack &recBuff);
	void handle_end_preview_house(MsgPack &recBuff);

	// 接收处理邀请信息 
	//bool handleByInvite(UINT16 msgType, ByteArray& recBuff);

	// 接受邀请
	void agreeInvite();

	// 拒绝邀请
	void disagreeInvite();

	// 邀请其他玩家
	void inviteOtherPlayer(const int & type, const std::string & inviteeAcc);

	void setInviteType(int type){mInviteType = type;}
	void setInvitee(const std::string & invitee){mInvitee = invitee;}

	int getInviteType(){return mInviteType;}
	std::string getInvitee(){return mInvitee;}
	std::string getInvitorAcc(){return mInviterAcc;}


	// 在弹出邀请对话框后15秒内没响应取消邀请
	void checkInviteAnswer();

private:
	int mInviteType;		// 邀请类型
	std::string mInvitee;	// 邀请人名字
	std::string mInviterAcc;	//邀请人帐号
//***********************************************************************

//***********************************************************************
// 交易模块 -- 玩家之间的交易
public:

	enum GridType
	{
		MYSELF,
		OTHER,
	};

	struct TradePack
	{
		TradePack()
			:templateId(-1)
			,count(0)
			,effectTime("")
		{
			
		}

		int templateId;
		uint count;
		std::string effectTime;
	};

	void inviteDeal(const std::string & name);
	// 对方的名字通过PickObj获取
	//void inviteDeal();
	void inviteDealByPicked();

	//bool handleByTrade(UINT16 msgType, ByteArray& recBuff);

	// 发送获得的物品
	void sendPutItem(int packetIndex, int tradeIndex);

	// 发送获得的金钱
	void sendUpdateMoney(int money);

	// 发送撤销的物品
	void sendUnputItem(int tradeIndex, int storageIndex);

	// 发送取消交易请求
	void reqCancelTrade();

	// 发送确定交易请求
	void reqSureTrade();
	
	// 发送交换俩个交易栏的物品
	void sendSwap(int srcIndex, int destIndex);

	// 设置交易栏包裹
	void setTradePack(int tradeIndex, int itemId, uint count, GridType type);
	void setTradePack(int tradeIndex, int itemId, std::string effTime, uint count, GridType type);

	int	 getTradeItemId(int tradeIndex, GridType type) const;
	uint getTradeItemCount(int tradeIndex, GridType type) const;
	std::string getTradeItemEffectTime(int tradeIndex, GridType type);

	// 获得交易得到的 和失去的金钱
	int getAcquireMoney(){return mGetMoney;}

	int getLoseMoney(){return mPutMoney;}
	
	// 获得交易方名字
	std::string getTraderName();

	// 清空交易包裹
	void clearTradePack();

	// 
	void initTradePack();

	// 交换俩物品
	void swapPackItem(int fromIndex, int toIndex, GridType type);

	// 一方点击确定后 另一方30秒内没有任何
	// 操作则视为取消交易
	void checkTradeAnswer();
private:
	/**
		暂时在收到交易栏有改变的时候， UI将交易
		栏的所有格子更新一遍。无需使用ItemPack，
		用vector即可实现
	*/
	std::vector<TradePack> mMyPack;		// 己方格子
	std::vector<TradePack> mOtherPack;	// 对方格子

	int			mPutMoney;         // 交易失去的钱
	int			mGetMoney;		   // 交易获得的钱

	std::string mTraderAcc;		   // 邀请方的帐号

//***********************************************************************

//***********************************************************************
// 疾病模块 --
public:
	//bool handleByDisease(UINT16 msgType, ByteArray& recBuff);
//***********************************************************************

//***********************************************************************
// 房屋售卖模块
public:
	//bool handleByHouseSales(UINT16 msgType, ByteArray& recBuff);
	void reqBuyHouse(int id);
	void reqBuyHouseAdd(int id);
	void reqBuyHouseReplace(int id, int type);
	void reqPreviewHouse(int houseSaleTableId);
	void reqPreviewHouse1(int houseTpltId);
	void previewHouse(int houseTpltId);
	void reqEndPreviewHouse();
	void saveHouseShopData();
//***********************************************************************

//***********************************************************************
// 家政系统 2010-07-29 xiaoMing
public:
	// 读取家政表 给LUA使用
	void saveDoServiceData();
	void reqStartDomestic();
	void reqDomesticService(int id);
	//bool handleByDoService(UINT16 msgType, ByteArray& recBuff);
	void changeValue();
	int getServiceId(){return mServiceId;}
	// 获得打折率
	int getMoneyRate(){return mMoneyRate;}
	void reqEndService(int id);

	// 设置模糊
	void setBlur(bool state) const;
public:
	int mChangeActive;
	int mChangeHouseClean;
	int mChangeCharm;
	int mChangeClean;
	int mChangeHealth;
	int mCostMoney;
	int mMoneyRate;

	int mServiceId;
//***********************************************************************

//***********************************************************************
// 查看其他玩家信息
public:
	enum GetPlayerType
	{
		PICKED = 1,
		FRIEND,
	};
	void reqOtherPlayer(GetPlayerType type);
	//bool handleOtherPlayer(UINT16 msgType, ByteArray& recBuff);
	void initOtherPlayerRTT();
	void otherPlayerRttRotate(int degree,int axes);
public:
	bool mInitRttFlag;
//***********************************************************************

//***********************************************************************
// 美容院
public:
	//	美容收费类型
	enum BeautyMoneyType
	{
		BMT_NONE,
		BMT_EQ,		//	EQ点
		BMT_GOLD	//	金币
	};
	//	美容部位类型
	enum BeautyAdjustType
	{
		BAT_NONE,
		BAT_HAIR,		//	发型
		BAT_HAIRCOLOR,	//	发色
		BAT_FACE,		//	脸型
		BAT_SKINCOLOR,	//	肤色
		BAT_BEARD		//	胡子
	};
	//	美容部件类型
	enum BeautySectionType
	{
		BST_NONE,
		BST_HEAD,	//	头部
		BST_HAIR,	//	发型
		BST_BEARD	//	胡子
	};
public:
	void initBeautyRTT(void);
	void reqStartBeauty(void);
	void reqCancelBeauty(void);
	void reqEndBeauty(void);
	//bool handleBeauty(UINT16 msgType, ByteArray& recBuf);
	void beautyRTTRotate(int degree, int axes);
	//	改变美容院RTT的外观
	void beautyRTTChangeLooks(BeautyAdjustType adjustType, int val);
	//	获取人物各部位的值
	// type: 1（MainPlayer），2（BeautyRTT）
	int getSectionVal(int type, BeautyAdjustType adjustType);
	//	设置窗口的图片
	void setBeautyChooseUI(std::string winname, BeautyAdjustType adjustType, int val);
	//	清除动态创建的imageset和window
	void clearBeautyChooseUI(void);
	//	保存不同美容部位的类型的数量
	void saveBeautySectionTypes(BeautySectionType sectionType, int adjustType, BeautyMoneyType moneyType);
	//	保存发色和肤色的数量
	//	sex: 1.男，2.女
	void saveBeautyColors(int sex, BeautySectionType sectionType, BeautyMoneyType moneyType);
	int getBeautyTypes(BeautyAdjustType adjustType, BeautyMoneyType moneyType);
	//	插入当前数据
	void insertCurVal(BeautyAdjustType adjustType, BeautyMoneyType moneyType, int val);
	void _insertVal(std::vector<int>& vec, int val);
	//	清除美容部位数据
	void clearBeautySectionTypes(void);
	//	获取页数
	//	total: 数量；maxCount: 每页最多数
	int getBeautyPages(int total, int maxCount);
	//	获取不同美容部位的不同类型的值
	int getBeautySectionVal(BeautyAdjustType adjustType, int index, BeautyMoneyType moneyType);
	//	获取相关类型的费用
	//	返回值: -1（获取数据有错）
	int getBeautySectionMoney(int sex, BeautyAdjustType adjustType, int val);
public:
	bool mInitBeautyRTTFlag;	// 美容院RTT初始标识
	float mScreenWidth;	// 屏幕宽度
	float mScreenHeight;	// 屏幕高度
	int mImagesetFlag;	// 值：0~7
	// 保存不同部位不同收费类型的美容数据
	std::vector<int> mHairVec_EQ, mHairColorVec_EQ, mFaceVec_EQ, mSkinColorVec_EQ, mBeardVec_EQ;
	std::vector<int> mHairVec_GOLD, mHairColorVec_GOLD, mFaceVec_GOLD, mSkinColorVec_GOLD, mBeardVec_GOLD;
//***********************************************************************

//***********************************************************************
// 玩家向导
public:
	//bool hangdePlayerGuide(UINT16 msgType, ByteArray& recBuf);

//***********************************************************************
// 查看房屋信息
public:
	void reqHouseBaseInfo();
	void reqHouseList();
	void reqHouseListByAcc(const char *acc);
	void reqChangeHouse(size_t idx, size_t type);
	void reqClearHouseLog();
	// 设置房屋权限
	void reqSetHousePermission(int furPerMission, int accessPerMission);
	void reqSetHouseWelWords(const std::string& welWord );
	//bool handleHouseInfo(UINT16 msgType, ByteArray& recBuff);

	// 礼物相关
	// 请求礼物记录
	void reqGiftLog(int type, uint pageNum) const;

	void reqCleanGiftLog(int type) const;

	void showLogGift(const char* instanceIDStr) const; // 点击超链接 显示礼物
	void showLogGiftByIcon(const char *iid) const;
	void delLogGiftByIcon(const char *iid);
	void initGiftLog();

	typedef std::vector<THouseData> THouseDataList;

	size_t getHouseSize();
	size_t getHouseListSize() { return mHouseDataList.size(); }
	const THouseData *getHouseListData(size_t idx) const { return &mHouseDataList[idx]; }

	size_t getFHouseSize();
	size_t getFHouseListSize() { return mFHouseDataList.size(); }
	const THouseData *getFHouseListData(size_t idx) const { return &mFHouseDataList[idx]; }

private:
	//my house data list
	THouseDataList mHouseDataList;
	//friend house data list
	THouseDataList mFHouseDataList;
	//
	std::vector<gift_express> mSendExpressList;
	std::map<uint64, gift_express> mRecvExpressMap;
	stime mTimeCur;

//***********************************************************************

//***********************************************************************
// 打工
public:
	void reqWork(uint id) const;
	void reqStartWork() const;
	void reqEndWork(uint id) const;
	//bool handleByWorks(UINT16 msgType, ByteArray& recBuff);
	void saveWorks();
	void handle_notify_common_scene_state(MsgPack &recBuff);
	bool OnLineListSelectionAccepted(const CEGUI::EventArgs& args);
public:
	int mWorkEventID;
	int mWorkMoney;
	int mEventMoney;
	int mEventItem;
	int mWorkId;
};

#endif