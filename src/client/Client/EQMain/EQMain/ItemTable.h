/******************************************************************************
* 作者: Y3
* 时间: 2009-11-04
* 描述: 物品相关的数据表
******************************************************************************/
#ifndef ItemTable_H
#define ItemTable_H

#include "EQMainPrerequisites.h"

/** 物品类型
*/
enum ItemType 
{
	IT_EQUIPMENT = 1,
};


/** 装备栏类型
*/
enum EquipSlotType
{
	ES_NONE		= 0,
	ES_HAT		= 0x0001,		//帽子
	ES_COAT		= 0x0002,		//上衣
	ES_NOSE		= 0x0004,		//眼镜
	ES_PANTS	= 0x0008,		//下衣
	ES_BACK		= 0x0010,		//背部
	ES_SHOES	= 0x0020,		//鞋子
	ES_HAND		= 0x0040,		//手持
	ES_ORNAMENT = 0x0080,		//饰物	
	ES_ALL		= 0x00FF,
};

/** 装备大分类
*/
enum ItemEquipBigType
{
	IEBT_NONE	= 0,	// 
	IEBT_SUIT	= 1,	// 套装
	IEBT_COAT	= 2,	// 上衣
	IEBT_PANTS	= 3,	// 下衣
	IEBT_HAT	= 4,	// 帽子
	IEBT_BACK	= 5,	// 披风/背包
	IEBT_NOSE	= 6,	// 眼镜
	IEBT_HAND	= 7,	// 手持
	IEBT_ORNAMENT	= 8,	// 饰物
	IEBT_SHOES		= 9,	// 鞋子
};

struct ItemEquipRow
{
	ItemEquipRow()
		: id( 0 )
		, name()
		, sex( 0 )
		, equip_pos( 0 )
		, equip_placeholder( 0 )
		, equip_disp_id( 0 )
		, place( 0 )
		, action( 0 )
	{
	}

	uint			id;
	std::string		name;				// 名称
	uchar			sex;				// 性别
	uchar			equip_pos;			// 装备位置
	uint			equip_placeholder;	// 占位
	uint			equip_disp_id;		// 装备表现ID
	uint			place;				// 存放方式
	uint			action;				// 调用原始动作
	uint			type1;				// 大分类
	uint			type2;				// 小分类
};

/** 物品小分类信息
*/
struct SmallTypesRow
{
	SmallTypesRow()
		: sex(0)
		, big_type(0)
	{
	}

	uchar			sex;			// 性别
	uint			big_type;		// 大分类 
	std::vector<std::string>	small_types;	// 小分类
};

/** 物品信息
*/
struct ItemRow
{
	ItemRow()
		: id( 0 )
		, type( 0 )
		, name("")
		, icon("")
		, imageset("")
		, tooltip(0)
		, overlap(0)
		, bind(0)
		, subID(0)
		, sellPrice(0)
		, useDel(0)
		, effectTimeID(0)
		, useID(0)
		, voiceid(-1)
		, dvoiceid(-1)
		, uvoiceid(-1)
	{
	}

	uint			id;
	std::string		name;
	uint 			type;
	std::string		imageset;
	std::string		icon;
	uint			tooltip;
	uint			overlap;
	uint			bind;
	uint			subID;
	uint            sellPrice;
	uint			useDel;
	uint			effectTimeID;
	uint			useID;
	uint			voiceid;//使用音效
	uint			dvoiceid;//放下音效
	uint			uvoiceid;//抓取音效
};

/** 物品时效信息
*/
struct ItemEffectTimeRow
{
	ItemEffectTimeRow()
		: id( 0 )
		, type( 0 )
		, effectTime( 0 )
		, tradeCutTime( 0 )
		, delTime( 0 )

	{
	}

	uint			id;
	uint 			type;
	uint			effectTime;
	uint			tradeCutTime;
	uint			delTime;
};

/** 创建角色时的服装选择信息
*/
struct FashionSelectRow 
{
	FashionSelectRow()
		: id(0)
		, sex(0)
		, sort1(0)
		, sort2(0)
		, dressId(0)

	{
	}
	
	uint	id;			//索引
	uint	sex;
	uint	sort1;		//分类1
	uint	sort2;		//分类2
	uint	dressId;	//服装Id
};

/**系统消息结构
*/
struct  SysMsgRow
{
	SysMsgRow()
		: id(0)
		, content("")
		,type("")
	{
	}

	/**  
		随机取消息内容 (分号(;)分隔的)
	*/
	const std::string randContent() const;
	
	uint id;
	std::string content;
	std::string type;
};

typedef stdext::hash_map<uint, ItemRow>				ItemTable;
typedef stdext::hash_map<std::string, ItemRow>		ItemTableEx;
typedef stdext::hash_map<uint, ItemEffectTimeRow>	ItemEffectTimeTable;
typedef stdext::hash_map<uint, ItemEquipRow>		ItemEquipTable; 
typedef stdext::hash_map<uint, FashionSelectRow>	FashionSelectTable;
typedef stdext::hash_map<uint, SysMsgRow>			SysMsgRowTable;

struct ModelDisplayRow;
class ItemTableManager : public Ogre::Singleton<ItemTableManager>
{
public:
	void load();
public:
	const ItemTable & getItemTable() const;
	const FashionSelectTable & getFashionSelectTable() const;
	const std::vector<SmallTypesRow> & getSmallTypesTable() const; 
public:
	const ItemRow * getItemRow( uint id ) const;
	const ItemRow * getItemRowEx( std::string name ) const;
	const ItemRow * getItemRowBySubId( uint sub_id, uint type ) const;
	const ItemEffectTimeRow * getItemEffectTimeRow( uint id) const;
	const ItemEquipRow * getItemEquipRow( uint id ) const;
	const ItemEquipRow * getEquipRowByItem( uint id ) const;
	//
	const FashionSelectRow * getFashionRow(uint id)	const;
	const uint getFashionRowCount() const; //服装选择表的记录数	
	const SysMsgRow * getSysMsgRow(uint id) const;
		
public:
	/** 取得某个装备栏相关的所有物品 
		@param slot 装备栏
	*/
	void getSlotCollect( uint slot, std::vector<const ItemEquipRow *> & arr ) const;

	/** 取得大分类下的所有物品 
		@param bigType 大分类 参考enum ItemEquipBigType
	*/
	void getBigTypeCollect( uint bigType, std::vector<const ItemEquipRow *> & arr ) const;

	/** 取得某个装备栏相关的所有模型表现 
		@param slot 装备栏
	*/
	void getItemModels( uint id, std::vector<const ModelDisplayRow *> & modelRows ) const;

private:
	void loadItemTable();
	void loadItemEffectTimeTable();
	void loadItemEquipTable();
	//
	void loadFashionTable();
	void loadSysMsgTable();
	void loadMutexActionMsgTable();
	void loadSmallTypesTable();
private:
	ItemTable			mItemTable; 
	ItemTableEx			mItemTableEx;
	ItemEffectTimeTable	mItemEffectTimeTable;
	ItemEquipTable		mItemEquipTable;
	FashionSelectTable	mFashionTable;
	std::vector<SmallTypesRow> mSmallTypesTable;
	SysMsgRowTable		mSysMsgTable;
};






#endif