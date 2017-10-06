/******************************************************************************
* 作者: Y3
* 时间: 2009-11-04
* 描述: 换装相关的数据表
******************************************************************************/
#ifndef AvatarTable_H
#define AvatarTable_H

#include "EQMainPrerequisites.h"


/** 挂点类型
*/
enum EquipHangType
{
	EHT_NONE = 0x00000000,
	EHT_HAT = 0x00000001,		// 头/帽子
	EHT_LEFTHAND = 0x00000002,	// 左手
	EHT_RIGHTHAND = 0x00000004,	// 右手
	EHT_BACK = 0x00000008,		// 背部
	EHT_GLASSES = 0x00000010,	// 眼镜
};
/** 性别类型
*/
enum GenderType
{
	GT_MALE		= 1,	// 男
	GT_FEMALE	= 2,	// 女
};

/** 头发替换表
*/
struct HairReplaceRow
{
	HairReplaceRow()
	{
	}

	uint		hair;		
	uint		type[5]; 
};
typedef std::map<uint, HairReplaceRow>		HairReplaceTable; 


/** 骨骼动作
*/
struct SkelActionRow
{
	SkelActionRow()
	{
	}

	std::string name;
	std::string action;
	std::string desc;
	std::string logicName;
	std::string logicDesc;
};
typedef std::map<std::string, std::vector<SkelActionRow> >	SkelActionTable; 


/** 装备栏物品挂接关系
*/ 
struct EquipSlotHangRow
{
	EquipSlotHangRow()
		: rID( 0 )
		, slot( 0 )
		, hairRep( 0 )
	{
	}

	uint			rID;
	uint 			slot;
	std::string		bone1; 
	std::string		bone2; 
	uchar			hairRep;	
};
typedef std::map<uint, std::vector<EquipSlotHangRow> >		EquipSlotHangTable;


/** 装备表现
*/ 
struct EquipDisplayRow
{
	EquipDisplayRow()
		: id( 0 )
		, share1( 0 )
		, share2( 0 )
		, share3( 0 )
		, hang1( 0 )
		, hang2( 0 )
		, hangID( 0 )
	{
	}

	uint			id;
	uint			share1;
	uint			share2;
	uint			share3;
	uint			hang1;
	uint			hang2;
	uint			hangID;
};
typedef stdext::hash_map<uint, EquipDisplayRow>		EquipDisplayTable; 


/** 角色种类
*/ 
struct RoleRaceRow
{
	RoleRaceRow()
		: id( 0 )
		, race( 0 )
		, gender( GT_MALE )
		, skinColor( 0 )
		, hairColor( 0 )
		, defHead( 0 )
		, defBody( 0 )	
		, defHair( 0 )
		, defBeard( 0 )
		, defWear1( 0 )
		, defWear2( 0 )
		, initScale(100)
	{
	}

	uint			id;
	uchar 			race;
	uchar			gender;
	std::string 	skeleton;
	uchar			skinColor;
	uchar			hairColor;
	uint			defBody;
	uint			defHead;
	uint			defHair;
	uint			defBeard;
	uint			defWear1;
	uint			defWear2;
	float           initScale;
};
typedef std::map<uint, RoleRaceRow>					RoleRaceTable;


/** 角色部件类型
*/
enum RoleSectionType
{
	RS_BODY = 0,
	RS_HEAD,
	RS_HAIR,
	RS_BEARD,		// 胡子
	RS_RAWEQUIP1,	// 初始装备1
	RS_RAWEQUIP2,	// 初始装备2
};


/** 角色部件
*/
struct RoleSectionRow
{
	RoleSectionRow()
		: roleID( 0 )
		, type( 0 )
		, color( 0 )
		, modDisID( 0 )
		, gender( GT_MALE )
		, canSel( true )
		, createVisible( true )
	{
	}

	uchar			type;
	uchar			color;
	uchar			gender;	
	bool			createVisible; // 控制创建时是否可见
	bool			canSel;
	uint			roleID;
	uint			modDisID;
};
typedef std::multimap<uint, RoleSectionRow>			RoleSectionTable;
typedef std::vector<const RoleSectionRow *>			RoleSectionRows;

/** 美容
*/
struct ChangeLooksRow
{
	ChangeLooksRow()
		: id(0)
		, money_type(0)
		, money(0)
	{
	}

	uint id;         // 性别 * 10000 +  改变类型(1.发型, 2.发色 3.脸形 4.肤色 5.胡子) * 1000 + 改变后的新值(新发型，新颜色)
	uint money_type; // 费用类型(1为游戏金币，2为点卡点数)
	uint money;      // 费用
};
typedef std::map<uint, ChangeLooksRow>				ChangeLooksTable;

class AvatarTableManager : public Ogre::Singleton<AvatarTableManager>
{
public:
	typedef std::set<uint> TColors;
public:
	void load();	
public:
	const HairReplaceRow *		getHairReplaceRow( uint hair ) const; 
	const EquipDisplayRow *		getEquipDisplayRow( uint id ) const; 
	const RoleRaceRow * 		getRoleRaceRow( uint id ) const;
	const EquipSlotHangRow *	getEquipSlotHangRow( uint id, uint slot ) const;

	
	/** 取得改变容貌相关费用
		@param sex 性别
		@param type 改变类型(1.发型, 2.发色 3.脸形 4.肤色 5.胡子) 参考AvatarAdjustType
		@param val 新发型，新肤色 值
	*/
	const ChangeLooksRow *		getChangeLooksRow( uint sex, uint type, uint val ) const;

	/** 取得某个骨格的所有动作
	*/
	void getSkelActions( const std::string & sSkeleton, std::vector<std::string> & arr ) const; 
	
	/** 取得角色部件
		@param id: 角色类型id
	*/
	void getRoleSectionRows( uint id, RoleSectionRows & rows ) const;

	/** 取得角色部件
		@param id: 角色类型id
		@param type: 角色部位(头，身体之类)
	*/
	void getRoleSectionRows( uint id, uint type, RoleSectionRows & rows ) const;

	/** 取得角色部件
		@param id: 角色类型id
		@param type: 角色部位(头，身体之类)
		@param color: 颜色
	*/
	void getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows ) const;

	/** 取得角色部件
	@param id: 角色类型id
	@param type: 角色部位(头，身体之类)
	@param color: 颜色
	@param forCreateRole: 是否为创建角色用的
	*/
	void getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows, 
							bool sel, bool forCreateRole ) const;

	void getSkeletons( std::vector<std::string> & arr ) const;

	/** 取得某个动作的中文描述
	*/
	void getSkelActionDescs( const std::string & sSkeleton, std::vector<std::string> & arr ) const; 
	/** 取得某个动作的逻辑描述
	*/
	const char* getSkelActionLogicDesc( const std::string & sSkeleton, const std::string & sLogicName ) const;
public:
	const HairReplaceTable &	getHairReplaceTable() const; 
	const SkelActionTable &		getSkelActionTable() const; 
	const EquipSlotHangTable &	getEquipSlotHangTable() const;
	const EquipDisplayTable &	getEquipDisplayTable() const; 
	const RoleRaceTable	& 		getRoleRaceTable() const;
	const RoleSectionTable &	getRoleSectionTable() const;
public:
	/** 取得某种角色的肤色列表
	*/
	TColors getSkinColors( uint roleID ) const;

	/** 取得某种角色的发色列表
	*/
	TColors getHairColors( uint roleID ) const;
private:
	void loadHairReplaceTable();
	void loadSkelActionTable();
	void loadEquipSlotHangTable();	
	void loadEquipDisplayTable();
	void loadRoleRaceTable();
	void loadCreateRoleRaceTable();
	void loadRoleSectionTable();
	void loadCreateRoleSectionTable();
	void loadChangeLooksTable();

	void _loadRoleSectionTable(const std::string & file);
	void _loadRoleRaceTable(const std::string & file);
private:	
	HairReplaceTable	mHairReplaceTable; 
	SkelActionTable		mSkelActionTable; 
	EquipSlotHangTable	mEquipSlotHangTable;
	EquipDisplayTable	mEquipDisplayTable; 
	RoleRaceTable		mRoleRaceTable;
	RoleSectionTable	mRoleSectionTable;
	ChangeLooksTable	mChangeLooksTable;
};




#endif