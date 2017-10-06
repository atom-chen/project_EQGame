/******************************************************************
* ����: ��_Mike
* ʱ��: 2010-05-23
* ����: ��ϷСģ�����
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
// ͨ�ú�������ʱ����ڴ�
	std::string getTimeString(int y, int m, int d, int h, int min, int s);
	std::string getTimeString(stime time);

//***********************************************************************
// �����Ӧʱ��  ������� �������
	enum CheckAnswerType
	{
		ANSWER_INVITE,
		ANSWER_TRADE,

		ANSWER_NONE,
	};

	// �����Ӧʱ��
	void checkAnswerTime();

	void setAnswerType(CheckAnswerType answerType){mAnswerType = answerType;}

	CheckAnswerType getAnswerType(){return mAnswerType;}

	void startCheckAnswer(CheckAnswerType answerType);

	void endCheckAnswer();

	void resetTimer(){mAnswerTimer.reset();}

	Ogre::Timer mAnswerTimer;		   // ��Ӧʱ��

	CheckAnswerType mAnswerType;

	static bool ifReviewHouseing;

//***********************************************************************
// ����ģ��
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

	// ���մ���������Ϣ 
	//bool handleByInvite(UINT16 msgType, ByteArray& recBuff);

	// ��������
	void agreeInvite();

	// �ܾ�����
	void disagreeInvite();

	// �����������
	void inviteOtherPlayer(const int & type, const std::string & inviteeAcc);

	void setInviteType(int type){mInviteType = type;}
	void setInvitee(const std::string & invitee){mInvitee = invitee;}

	int getInviteType(){return mInviteType;}
	std::string getInvitee(){return mInvitee;}
	std::string getInvitorAcc(){return mInviterAcc;}


	// �ڵ�������Ի����15����û��Ӧȡ������
	void checkInviteAnswer();

private:
	int mInviteType;		// ��������
	std::string mInvitee;	// ����������
	std::string mInviterAcc;	//�������ʺ�
//***********************************************************************

//***********************************************************************
// ����ģ�� -- ���֮��Ľ���
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
	// �Է�������ͨ��PickObj��ȡ
	//void inviteDeal();
	void inviteDealByPicked();

	//bool handleByTrade(UINT16 msgType, ByteArray& recBuff);

	// ���ͻ�õ���Ʒ
	void sendPutItem(int packetIndex, int tradeIndex);

	// ���ͻ�õĽ�Ǯ
	void sendUpdateMoney(int money);

	// ���ͳ�������Ʒ
	void sendUnputItem(int tradeIndex, int storageIndex);

	// ����ȡ����������
	void reqCancelTrade();

	// ����ȷ����������
	void reqSureTrade();
	
	// ���ͽ�����������������Ʒ
	void sendSwap(int srcIndex, int destIndex);

	// ���ý���������
	void setTradePack(int tradeIndex, int itemId, uint count, GridType type);
	void setTradePack(int tradeIndex, int itemId, std::string effTime, uint count, GridType type);

	int	 getTradeItemId(int tradeIndex, GridType type) const;
	uint getTradeItemCount(int tradeIndex, GridType type) const;
	std::string getTradeItemEffectTime(int tradeIndex, GridType type);

	// ��ý��׵õ��� ��ʧȥ�Ľ�Ǯ
	int getAcquireMoney(){return mGetMoney;}

	int getLoseMoney(){return mPutMoney;}
	
	// ��ý��׷�����
	std::string getTraderName();

	// ��ս��װ���
	void clearTradePack();

	// 
	void initTradePack();

	// ��������Ʒ
	void swapPackItem(int fromIndex, int toIndex, GridType type);

	// һ�����ȷ���� ��һ��30����û���κ�
	// ��������Ϊȡ������
	void checkTradeAnswer();
private:
	/**
		��ʱ���յ��������иı��ʱ�� UI������
		�������и��Ӹ���һ�顣����ʹ��ItemPack��
		��vector����ʵ��
	*/
	std::vector<TradePack> mMyPack;		// ��������
	std::vector<TradePack> mOtherPack;	// �Է�����

	int			mPutMoney;         // ����ʧȥ��Ǯ
	int			mGetMoney;		   // ���׻�õ�Ǯ

	std::string mTraderAcc;		   // ���뷽���ʺ�

//***********************************************************************

//***********************************************************************
// ����ģ�� --
public:
	//bool handleByDisease(UINT16 msgType, ByteArray& recBuff);
//***********************************************************************

//***********************************************************************
// ��������ģ��
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
// ����ϵͳ 2010-07-29 xiaoMing
public:
	// ��ȡ������ ��LUAʹ��
	void saveDoServiceData();
	void reqStartDomestic();
	void reqDomesticService(int id);
	//bool handleByDoService(UINT16 msgType, ByteArray& recBuff);
	void changeValue();
	int getServiceId(){return mServiceId;}
	// ��ô�����
	int getMoneyRate(){return mMoneyRate;}
	void reqEndService(int id);

	// ����ģ��
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
// �鿴���������Ϣ
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
// ����Ժ
public:
	//	�����շ�����
	enum BeautyMoneyType
	{
		BMT_NONE,
		BMT_EQ,		//	EQ��
		BMT_GOLD	//	���
	};
	//	���ݲ�λ����
	enum BeautyAdjustType
	{
		BAT_NONE,
		BAT_HAIR,		//	����
		BAT_HAIRCOLOR,	//	��ɫ
		BAT_FACE,		//	����
		BAT_SKINCOLOR,	//	��ɫ
		BAT_BEARD		//	����
	};
	//	���ݲ�������
	enum BeautySectionType
	{
		BST_NONE,
		BST_HEAD,	//	ͷ��
		BST_HAIR,	//	����
		BST_BEARD	//	����
	};
public:
	void initBeautyRTT(void);
	void reqStartBeauty(void);
	void reqCancelBeauty(void);
	void reqEndBeauty(void);
	//bool handleBeauty(UINT16 msgType, ByteArray& recBuf);
	void beautyRTTRotate(int degree, int axes);
	//	�ı�����ԺRTT�����
	void beautyRTTChangeLooks(BeautyAdjustType adjustType, int val);
	//	��ȡ�������λ��ֵ
	// type: 1��MainPlayer����2��BeautyRTT��
	int getSectionVal(int type, BeautyAdjustType adjustType);
	//	���ô��ڵ�ͼƬ
	void setBeautyChooseUI(std::string winname, BeautyAdjustType adjustType, int val);
	//	�����̬������imageset��window
	void clearBeautyChooseUI(void);
	//	���治ͬ���ݲ�λ�����͵�����
	void saveBeautySectionTypes(BeautySectionType sectionType, int adjustType, BeautyMoneyType moneyType);
	//	���淢ɫ�ͷ�ɫ������
	//	sex: 1.�У�2.Ů
	void saveBeautyColors(int sex, BeautySectionType sectionType, BeautyMoneyType moneyType);
	int getBeautyTypes(BeautyAdjustType adjustType, BeautyMoneyType moneyType);
	//	���뵱ǰ����
	void insertCurVal(BeautyAdjustType adjustType, BeautyMoneyType moneyType, int val);
	void _insertVal(std::vector<int>& vec, int val);
	//	������ݲ�λ����
	void clearBeautySectionTypes(void);
	//	��ȡҳ��
	//	total: ������maxCount: ÿҳ�����
	int getBeautyPages(int total, int maxCount);
	//	��ȡ��ͬ���ݲ�λ�Ĳ�ͬ���͵�ֵ
	int getBeautySectionVal(BeautyAdjustType adjustType, int index, BeautyMoneyType moneyType);
	//	��ȡ������͵ķ���
	//	����ֵ: -1����ȡ�����д�
	int getBeautySectionMoney(int sex, BeautyAdjustType adjustType, int val);
public:
	bool mInitBeautyRTTFlag;	// ����ԺRTT��ʼ��ʶ
	float mScreenWidth;	// ��Ļ���
	float mScreenHeight;	// ��Ļ�߶�
	int mImagesetFlag;	// ֵ��0~7
	// ���治ͬ��λ��ͬ�շ����͵���������
	std::vector<int> mHairVec_EQ, mHairColorVec_EQ, mFaceVec_EQ, mSkinColorVec_EQ, mBeardVec_EQ;
	std::vector<int> mHairVec_GOLD, mHairColorVec_GOLD, mFaceVec_GOLD, mSkinColorVec_GOLD, mBeardVec_GOLD;
//***********************************************************************

//***********************************************************************
// �����
public:
	//bool hangdePlayerGuide(UINT16 msgType, ByteArray& recBuf);

//***********************************************************************
// �鿴������Ϣ
public:
	void reqHouseBaseInfo();
	void reqHouseList();
	void reqHouseListByAcc(const char *acc);
	void reqChangeHouse(size_t idx, size_t type);
	void reqClearHouseLog();
	// ���÷���Ȩ��
	void reqSetHousePermission(int furPerMission, int accessPerMission);
	void reqSetHouseWelWords(const std::string& welWord );
	//bool handleHouseInfo(UINT16 msgType, ByteArray& recBuff);

	// �������
	// ���������¼
	void reqGiftLog(int type, uint pageNum) const;

	void reqCleanGiftLog(int type) const;

	void showLogGift(const char* instanceIDStr) const; // ��������� ��ʾ����
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
// ��
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