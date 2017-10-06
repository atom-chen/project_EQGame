/*******************************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: Player
		游戏中玩家，玩家交互
*******************************************************************************/
#ifndef Player_H
#define Player_H

#include "EQMainPrerequisites.h"
#include "GameObj.h"
#include "ItemPack.h"
#include "NetPacket.h"
#include "PlayProperty.h"
#include "Task.h"
#include "EnumDef.h"

//装备，仓库大小
#define DEFAULT_WAREHOUSE_SIZE 30
#define DEFAULT_BODY_SIZE 8
#define DEFAULT_SHOPCENTER_SIZE 36
#define DEFAULT_SHOPCENTER_RECOMMEND_SIZE 20
#define DEFAULT_FITMODE_SHOPCENTER_SIZE 24
#define DEFAULT_FITMODE_SHOPPINGCART_SIZE 12
#define DEFAULT_NPCSHOP_SIZE 50
#define DEFAULT_STORAGE_SIZE 40
/*
前置声明
*/
struct notify_package;
struct player_basic_data;
struct ShopCenterRow;
struct NpcShopRow;
/**
角色类设计目标，可包含玩家，NPC
*/
class Player : public GameObj
{
public:
	//角色装备位置
	enum eBody
	{
		BODY_HAT = 0,	//帽子
		BODY_CLOTHES,	//衣服
		BODY_GLASSES,	//眼镜
		BODY_TROUSERS,	//裤子
		BODY_BACK,		//背部
		BODY_SHOES,		//鞋子
		BODY_HAND,		//手持
		BODY_TRINKET,	//饰品
	};

/*
数据初始化，释放----------------------------------------------------------------
*/
public:
	Player(void);
	virtual ~Player(void);
	void				init();
	void				initShopCenter();
	virtual void		cleanup();
/*
帐号相关的----------------------------------------------------------------------
*/
public:
	//帐号
	void				setAccount(std::string a);
	const std::string&	getAccount();
/*
行走，坐标同步------------------------------------------------------------------
*/
public:
	//移动
	virtual void		moveTo(Ogre::Vector3& dest);
	//帧更新
	virtual bool		update();

	// 角色移动时，加入光照信息
	virtual void		setPos(Ogre::Vector3& v);
	//
	virtual void		reqStartWalk(Ogre::Vector3& dest){}

	//播放表情动作
	void				playAnimation(const std::string & ani, const std::string & player, const std::string & target,bool loop);
	
	//获取当前播放的动作名
	std::string			getCurrentAnimation();

	virtual void		setVisible(bool visible);

	// 设置头顶名字是否可见(2011/6/17 add by hezhr)
	void				setHeadUpVisible(bool visible);

private:
	// 计算光照监听
	void				_calcLightListener(Ogre::Vector3& pos);

/*
装备道具相关--------------------------------------------------------------------
*/
public:
	//更新基本属性
	void				updateBaseLook();
	//设置物品包（装备，仓库，etc...）
	void				setPack(notify_package& pack , 
							ItemPack::PackType pt = ItemPack::PT_NONE);
	//设置仓库数据
	void				setStoragePack(notify_player_depot& pack);
	//更新背包数据
	void				updateWarehouse();
	//更新装备数据
	void				updateBodyItem();
	//更新仓库数据
	void				updateStorage() const;
	//检查item是否在背包、装备或仓库里面（hezhr）
	bool				isInItemPack(int id);
	//得物品包里的配置信息（装备，仓库，etc...）
	const ItemRow*		getItemRow(uint index,
								ItemPack::PackType pt = ItemPack::PT_NONE);
	const ItemGrid*		getItemGird(uint index,
								ItemPack::PackType pt = ItemPack::PT_NONE);
	const ItemGrid*		getItemGridByGlobaIdx(uint index,
								ItemPack::PackType pt = ItemPack::PT_NONE);
	//
	uint				getItemID(uint index,ItemPack::PackType pt);
	uint				getItemIDByGlobaIdx(uint index,ItemPack::PackType pt);
	std::string			getItemDelTime(uint index,ItemPack::PackType pt);
	std::string			getItemDelTimeByGlobalIdx(uint index,ItemPack::PackType pt);
	uint64				getItemInstanceID(uint index,ItemPack::PackType pt);
	std::string			getItemInsIdStr(uint index,ItemPack::PackType pt);

	uint				getPackCurPage(ItemPack::PackType pt = ItemPack::PT_NONE);
	uint				getPackPageCount(ItemPack::PackType pt = ItemPack::PT_NONE);
	bool				setPackLastPage(ItemPack::PackType pt);
	bool				setPackNextPage(ItemPack::PackType pt);
	bool				setPackCurPage(ItemPack::PackType pt, uint page);

	//商城相关
	const ShopCenterRow*	getShopCenterRow(uint index, ItemPack::PackType pt);
	void					setShopCenterType(uint type, int sex, ItemPack::PackType pt, int arg);
	void				addShoppingCart(const ShopCenterRow* row, uint uid);
	int					delShoppingCart(int index);
	int					delShoppingCartByData(uint uid);
	void				clearShoppingCart();
	void				sendShoppingCart();
	int					getShoppingCartPrice(int money_type);

	//角色属性
	player_property&	getProperty(){return mProperty.getPro();}
	PlayProperty&		getPropertyModule(){return mProperty;}

	void				setGold(player_coin& coin);
	int					getGameGold() { return mGameCoin;} 
	int					getEQGold() { return mEQCoin; }
	// 仓库相关处理
	int					getStorageCoin() const {return mStorageCoin;}
	void				reqSaveMoneyToStorage(const uint moneyCount) const;
	void				reqGetMoneyFromStorage(const uint moneyCount) const;

	//聊天泡泡
	void				showChatPaoPao(const char* text, uint life=0);
	std::string			getChatPaoPaoName(){return mChatPaoPao;}
	void				setChatPaoPaoIsVisible(bool visible);
	bool				getChatPaoPaoIsVisible(){return mChatPaoPaoIsVisible;}
	//头顶信息
	void				showHeadup();
	//头顶大表情
	void				showHeadupBigExpression(int id);

	//任务
	Task&				getTask() { return mTask; }

private:
	void				_takeOffAll();
	//穿在角色身上的装备
	ItemPack			mBody;
	//背包（包裹）
	ItemPack			mWarehouse;
	//仓库
	ItemPack			mStorage;
	//商城
	ItemPack			mShopCenter;
	ItemPack			mShopCenterRecommend;
	ItemPack			mFitModeShopCenter;
	ItemPack			mFitModeShoppingCart;
	//npc商店
	ItemPack			mNpcShop;
	//任务
	Task				mTask;

	//角色属性
	PlayProperty		mProperty;
	//钱
	int					mGameCoin;
	int					mEQCoin;
	int					mStorageCoin;

	//聊天泡泡
	std::string			mChatPaoPao;
	bool				mChatPaoPaoIsVisible;
	std::string			mHeadup;
	std::string			mBigExp;

	//疾病粒子
	uint				mDiseaseParticle;

/*
角色基本属性相关----------------------------------------------------------------
*/
public:
	//获得玩家等级
	int getLevel(){return mPlayerBD.level;}
	int getSex(){return mPlayerBD.sex;}
	std::string getName(){return mPlayerBD.username;}
	std::string getPlayerAccount(){return mPlayerBD.account;}
	void setHealtyState(int healthState);
	int getHealtyState() const;
	//不包含道具这块的角色基本信息（目前创建角色用到）
	player_basic_data	mPlayerBD;
	void setPlayerData(player_basic_data& data);
	player_basic_data* getPlayerData(){return &mPlayerBD;}
	bool isMale();
	//玩家是否有发送GM命令的权限(2011/8/11 add by hezhr)
	bool isGmEnabled(){return GM_LEVEL(mPlayerBD.gm_enabled)==gm_enable ? true: false;}

/*
马赛克效果
*/
private:
	uint			mMosaicID;
	Ogre::Vector3	mMosaicP1,mMosaicP2;
	void			_updateMosaicEffect();
	void		    _calcuBoxToRec(Ogre::AxisAlignedBox& box, int width, int height, int& minX, int& minY, int& maxX, int& maxY);
public:
	void			addMosaicEffect(Ogre::Vector3& p1, Ogre::Vector3& p2);
	void			removeMosaicEffect();

};
#endif