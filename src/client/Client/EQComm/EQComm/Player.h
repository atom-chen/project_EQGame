/*******************************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: Player
		��Ϸ����ң���ҽ���
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

//װ�����ֿ��С
#define DEFAULT_WAREHOUSE_SIZE 30
#define DEFAULT_BODY_SIZE 8
#define DEFAULT_SHOPCENTER_SIZE 36
#define DEFAULT_SHOPCENTER_RECOMMEND_SIZE 20
#define DEFAULT_FITMODE_SHOPCENTER_SIZE 24
#define DEFAULT_FITMODE_SHOPPINGCART_SIZE 12
#define DEFAULT_NPCSHOP_SIZE 50
#define DEFAULT_STORAGE_SIZE 40
/*
ǰ������
*/
struct notify_package;
struct player_basic_data;
struct ShopCenterRow;
struct NpcShopRow;
/**
��ɫ�����Ŀ�꣬�ɰ�����ң�NPC
*/
class Player : public GameObj
{
public:
	//��ɫװ��λ��
	enum eBody
	{
		BODY_HAT = 0,	//ñ��
		BODY_CLOTHES,	//�·�
		BODY_GLASSES,	//�۾�
		BODY_TROUSERS,	//����
		BODY_BACK,		//����
		BODY_SHOES,		//Ь��
		BODY_HAND,		//�ֳ�
		BODY_TRINKET,	//��Ʒ
	};

/*
���ݳ�ʼ�����ͷ�----------------------------------------------------------------
*/
public:
	Player(void);
	virtual ~Player(void);
	void				init();
	void				initShopCenter();
	virtual void		cleanup();
/*
�ʺ���ص�----------------------------------------------------------------------
*/
public:
	//�ʺ�
	void				setAccount(std::string a);
	const std::string&	getAccount();
/*
���ߣ�����ͬ��------------------------------------------------------------------
*/
public:
	//�ƶ�
	virtual void		moveTo(Ogre::Vector3& dest);
	//֡����
	virtual bool		update();

	// ��ɫ�ƶ�ʱ�����������Ϣ
	virtual void		setPos(Ogre::Vector3& v);
	//
	virtual void		reqStartWalk(Ogre::Vector3& dest){}

	//���ű��鶯��
	void				playAnimation(const std::string & ani, const std::string & player, const std::string & target,bool loop);
	
	//��ȡ��ǰ���ŵĶ�����
	std::string			getCurrentAnimation();

	virtual void		setVisible(bool visible);

	// ����ͷ�������Ƿ�ɼ�(2011/6/17 add by hezhr)
	void				setHeadUpVisible(bool visible);

private:
	// ������ռ���
	void				_calcLightListener(Ogre::Vector3& pos);

/*
װ���������--------------------------------------------------------------------
*/
public:
	//���»�������
	void				updateBaseLook();
	//������Ʒ����װ�����ֿ⣬etc...��
	void				setPack(notify_package& pack , 
							ItemPack::PackType pt = ItemPack::PT_NONE);
	//���òֿ�����
	void				setStoragePack(notify_player_depot& pack);
	//���±�������
	void				updateWarehouse();
	//����װ������
	void				updateBodyItem();
	//���²ֿ�����
	void				updateStorage() const;
	//���item�Ƿ��ڱ�����װ����ֿ����棨hezhr��
	bool				isInItemPack(int id);
	//����Ʒ�����������Ϣ��װ�����ֿ⣬etc...��
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

	//�̳����
	const ShopCenterRow*	getShopCenterRow(uint index, ItemPack::PackType pt);
	void					setShopCenterType(uint type, int sex, ItemPack::PackType pt, int arg);
	void				addShoppingCart(const ShopCenterRow* row, uint uid);
	int					delShoppingCart(int index);
	int					delShoppingCartByData(uint uid);
	void				clearShoppingCart();
	void				sendShoppingCart();
	int					getShoppingCartPrice(int money_type);

	//��ɫ����
	player_property&	getProperty(){return mProperty.getPro();}
	PlayProperty&		getPropertyModule(){return mProperty;}

	void				setGold(player_coin& coin);
	int					getGameGold() { return mGameCoin;} 
	int					getEQGold() { return mEQCoin; }
	// �ֿ���ش���
	int					getStorageCoin() const {return mStorageCoin;}
	void				reqSaveMoneyToStorage(const uint moneyCount) const;
	void				reqGetMoneyFromStorage(const uint moneyCount) const;

	//��������
	void				showChatPaoPao(const char* text, uint life=0);
	std::string			getChatPaoPaoName(){return mChatPaoPao;}
	void				setChatPaoPaoIsVisible(bool visible);
	bool				getChatPaoPaoIsVisible(){return mChatPaoPaoIsVisible;}
	//ͷ����Ϣ
	void				showHeadup();
	//ͷ�������
	void				showHeadupBigExpression(int id);

	//����
	Task&				getTask() { return mTask; }

private:
	void				_takeOffAll();
	//���ڽ�ɫ���ϵ�װ��
	ItemPack			mBody;
	//������������
	ItemPack			mWarehouse;
	//�ֿ�
	ItemPack			mStorage;
	//�̳�
	ItemPack			mShopCenter;
	ItemPack			mShopCenterRecommend;
	ItemPack			mFitModeShopCenter;
	ItemPack			mFitModeShoppingCart;
	//npc�̵�
	ItemPack			mNpcShop;
	//����
	Task				mTask;

	//��ɫ����
	PlayProperty		mProperty;
	//Ǯ
	int					mGameCoin;
	int					mEQCoin;
	int					mStorageCoin;

	//��������
	std::string			mChatPaoPao;
	bool				mChatPaoPaoIsVisible;
	std::string			mHeadup;
	std::string			mBigExp;

	//��������
	uint				mDiseaseParticle;

/*
��ɫ�����������----------------------------------------------------------------
*/
public:
	//�����ҵȼ�
	int getLevel(){return mPlayerBD.level;}
	int getSex(){return mPlayerBD.sex;}
	std::string getName(){return mPlayerBD.username;}
	std::string getPlayerAccount(){return mPlayerBD.account;}
	void setHealtyState(int healthState);
	int getHealtyState() const;
	//�������������Ľ�ɫ������Ϣ��Ŀǰ������ɫ�õ���
	player_basic_data	mPlayerBD;
	void setPlayerData(player_basic_data& data);
	player_basic_data* getPlayerData(){return &mPlayerBD;}
	bool isMale();
	//����Ƿ��з���GM�����Ȩ��(2011/8/11 add by hezhr)
	bool isGmEnabled(){return GM_LEVEL(mPlayerBD.gm_enabled)==gm_enable ? true: false;}

/*
������Ч��
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