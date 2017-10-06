/******************************************************************
* 作者: XiaoMing
* 时间: 2010-12-01
* 描述: 礼物系统
*******************************************************************/
#ifndef GIFT_SYSTEM_H
#define GIFT_SYSTEM_H

#include "..\..\EQMain\EQMain\PickObjectProxy.h"
#include "HouseScene.h"

class ModelGcs;
class ByteArray;
class FlashDlg;
struct gift_box;
struct gift_express;

typedef unsigned __int64  uint64;
typedef unsigned __int32  uint32;
//--------------------------------------------------------
struct GiftDate
{
	GiftDate()
		:_year(1970)
		,_month(1)
		,_day(1)
	{

	}

	void reset()
	{
		_year = 1970;
		_month = 1;
		_day = 1;
	}

	uint _year;
	uint _month;
	uint _day;
public:
	void set(uint year, uint month, uint day);
};
//--------------------------------------------------------

//礼物基本信息
struct GiftInfo
{
	/*不使用NetPacket里的数据结构，NetPacket更新频繁-。-*/
	GiftInfo()
		:expressType(0)
		,itemTemplateId(0)
		,cardId(0)
		,boxId(0)
		,gridX(0)
		,gridZ(0)
		,floorNum(0)
		,itemOverlay(0)
		,itemInstanceId(0)
		,instanceId(0)
		,name("")
		,greetings("")
		,giverName("")
		,giverAcc("")
		,recipientAcc("")
		,reciverName("")
		,effecTime("")
	{}

	~GiftInfo()
	{

	}
public:
	GiftInfo(const GiftInfo &info)
		:expressType(info.expressType)
		,itemTemplateId(info.itemTemplateId)
		,cardId(info.cardId)
		,boxId(info.boxId)
		,gridX(info.gridX)
		,gridZ(info.gridZ)
		,floorNum(info.floorNum)
		,itemOverlay(info.itemOverlay)
		,itemInstanceId(info.itemInstanceId)
		,instanceId(info.instanceId)
		,name(info.name)
		,greetings(info.greetings)
		,giverName(info.giverName)
		,giverAcc(info.giverAcc)
		,reciverName(info.reciverName)
		,recipientAcc(info.recipientAcc)
		,sendTime(info.sendTime)
		,reciveTime(info.reciveTime)
		,effecTime(info.effecTime)
	{

	}

	void reset();
	
	int     expressType;
	int		itemTemplateId;		// 物品模板ID
	int		cardId;		// 贺卡
	int		boxId;				// 礼盒ID
	int     gridX;				// 放置的格子的X坐标
	int     gridZ;              // 放置的格子的Z坐标
	int     floorNum;			// 放置的格子的楼层
	int		itemOverlay;		// 物品数量
	int		storageIndex;		// 物品在背包中的索引
	uint64	itemInstanceId;		// 物品实例ID
	uint64  instanceId;			// 礼物实例ID（服务器端）
	std::string name;			// 礼物名
	std::string greetings;		// 祝福语
	std::string giverName;		// 送礼者名字
	std::string giverAcc;		// 送礼者帐号
	std::string recipientAcc;	// 收礼者账号
	std::string reciverName;    // 收礼者名字
	GiftDate	sendTime;		// 礼物发送时间
	GiftDate	reciveTime;		// 礼物接收时间
	std::string effecTime;		// 礼物物品的时效
};
//--------------------------------------------------------
//礼物
class Gift
{
public:
	// 该类用于点击礼物时使用
	class PickObjectUnit : public PickObject
	{
	public:
		// 名字为 Gift::PickObjectUnit
		const std::string& getClassName();

		void  setCreator(Gift* creator){ mCreator = creator; }
		Gift* getCreator(){return mCreator;}

		PickObjectUnit()
			:mCreator(NULL)
		{}
	private:
		Gift* mCreator;

	};

public:
	ModelGcs* getModel() const { return mModel;}
	void      setModel(ModelGcs* model);
	void	  setPosition();	// 设置礼物模型位置
	void      setGiftInfo(const GiftInfo & info){ mInfo = info; }
	const GiftInfo & getGiftInfo() const {return mInfo;}

	uint32    getIndex() const{return mIndex;}
	void	  setIndex(const uint32 index) { mIndex = index; }

	void	  setInstanceId(const uint64 instanceId){mInfo.instanceId = instanceId;}
	uint64	  getInstanceId() const {return mInfo.instanceId;}
	// 重置（回收）礼物数据
	void      reset();

	Gift(const GiftInfo& giftInfo);
	~Gift();
private:
	void	  _setPosition(const Ogre::Vector3& position);
	void	  _setPosition(const int floorNum, const int gridX, const int gridZ);
private:
	uint32		mIndex;		 // 客户端所用的礼物索引，即礼物系统为该礼物生成的
	GiftInfo    mInfo;     
	ModelGcs*   mModel;		 // 礼盒模型
	PickObjectUnit  mPickObjectUnit;
	PickObjectProxy mPickObjectProxy;

};

//--------------------------------------------------------
// 礼物系统
// 礼物系统用于礼物的发送，以及房屋内礼物的显示等。
// 礼物的日志在GiftLogManager中管理。GiftSystem不
// 参与日志的管理
typedef std::map<uint32, Gift*> GiftMap;
typedef std::vector<Gift*> GiftBuffer;

class GiftSystem: public Ogre::Singleton<GiftSystem>
{
public:
	enum GiftServiceType
	{// 同步于服务器
		Pack = 2,
		PickUp  = 3,
		Put = 4,
		GetExpressGift = 5,
		ReplyMailGift = 6,
	};
public:
	Gift* createGift(const GiftInfo & info);
	void  deleteGift(const uint32 giftId);

	void  deleteAllGift();

	void  putGift(Gift* gift) const;
	Gift* getGift(const uint32 giftId) const;
	// 点击礼物模型
	void  clickGift(Gift* gift);

	void  cleanCurMakingGiftInfo();
	// 设置要送的礼物的物品
	// @parm storageGolbaIndex 仓库全局索引
	void  setGiftItem(int storageGolbaIndex);

	// 接收礼物数据
	void  recGiftData(const std::vector<gift_box> &giftData);

	// 消息处理
	bool  handleMsg(UINT16 msgType, ByteArray& recBuff);

	void  reqStartGiftService(const GiftServiceType type) const;
	void  reqEndGiftService() const;
	void  reqPackGift() const;
	void  reqCanclePackGift() const;
	// 求情在房屋内放置礼物
	void  reqPutGift() const;
	void  reqPickUpGift() const;
	// 请求快递礼物
	void  reqExpressGift() const;
	// 请求收取快递礼物
	void  reqReciveExpressGift() const;

	// 开始礼物服务，根据不同的服务状态，打开不同的面板
	void  startGiftService(const GiftServiceType type) ;

	// 礼物盒样式数据读取
	void  loadGiftBoxData();

	// 礼物卡样式数据读取
	void  loadGiftCardData();

	// 将网络消息结构转为GiftInfo
	void  converDataToGiftInfo(const gift_box &giftBox, GiftInfo& giftInfo) const;

	void  converDataToGiftInfo(const gift_express &giftExpress, GiftInfo& giftInfo) const;

	// 设置当前手工制作的礼物的基本信息
	void  setDIYGiftInfo(std::string giftame,
							uint cardId, 
							uint boxId,
							int  storageGolbaIndex,
							std::string greetings, 
							std::string giverName);
	// 设置当前快递的礼物的基本信息
	void  setMailGiftInfo( std::string reciveAcc,
						  std::string greetings,
						  uint cardId, 
						  int  storageGolbaIndex,
						  const uint year,
						  const uint month,
						  const uint day);

	const GiftInfo* getCurPickGiftInfo(){return &mCurPickGift->getGiftInfo();}
	// 播放Flash动画
	void playGiftCardFlash(const char* flashUrl);
	// 移动flash窗口（跟随游戏主窗口移动）
	void moveDisplayFlashWin();

	bool hasGiftInGrid(HouseSceneGrid *grid);

	// 礼物记录相关
	GiftSystem();
	~GiftSystem();
private:
	Gift* _reqGiftBuffer();  // 请求分配缓存，如果有
	void  _addGiftBuffer(Gift* gift);
	void  _recycleGift(Gift* gift);
	// 判断当前所站的格子能否放置礼物
	bool  _canPutGift(const int floorNum, const int gridX, const int gridZ) const;

	void  _pickUpGift() const;
private:
	uint32 mAutoGiftIndex;		
	GiftMap mGiftMap;		 // 管理房屋中的礼物，带模型显示
	GiftBuffer mBuffer;
public:
	// 供LUA使用
	GiftInfo mCurMakingGiftInfo; // 当前制作的礼物
	Gift*	 mCurPickGift;
	bool	 mIsLoadBoxData;
	bool	 mIsLoadCardData;
	FlashDlg* mDisplayFlashDlg;
	HWND	 mFlashHwnd;
	
};
#endif
