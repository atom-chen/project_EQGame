/******************************************************************************
* 作者: Y3
* 时间: 2009-11-24
* 描述: 房屋相关的数据表
******************************************************************************/
#ifndef HouseTable_H
#define HouseTable_H

#include "EQMainPrerequisites.h"

#include "CompareVectorTemplate.h"

/**玩家房屋基本配置表，参考服务器，by_麦
*/
struct PlayerHouseRow
{
	int		id;						//00001000
	std::string	name;				//>男子单身宿舍</name>
	uint	level;					//>1</level>
	uint	type;					//>1</type>
	int		scene_id;				// 场景id，对应scene.xml表
	std::string	house_furniture;	//>default1.xml</house_furniture>
	uint	maxPlayer;				// 容纳最大玩家数
	int		previewBorn;			//预览镜头位置
};
typedef std::map<ushort, PlayerHouseRow>		PlayerHouseTable; 

/** 房屋配件类型. 
@remarks 以下这些是预先定义的类型.(0-127)为保留类型.  用户自定义类型应从128开始
*/
enum HouseComponentPropertyType
{
	HCP_NONE				= 0,
	HCP_STATIC				= 1,			//简单的类型
	HCP_FLOOR				= 2,			//地板
	HCP_OUT_WALL			= 3,			//外墙
	HCP_IN_WALL				= 4,			//内墙
	HCP_ROOF				= 5,			//屋顶
	HCP_TCX					= 6,			//T脚线
};

enum HouseGridType
{
	HGT_FRAME				= 1,			//框架网格
};



/** 房屋部件属性
*/
struct HouseCompPropRow
{
	typedef CompareVector2Template<int> GridPos;
	typedef std::vector<GridPos> GridPosArray;

	HouseCompPropRow()
		: PropID( 0 )
		, BeWatchMode( 0 )
		, SelfHeight( 0 )
		, OffsetHeight( 0 )
		, AcceptLay( 0 )
		, LayType( 0 )
		, CanRep( false )
		, LightID( 0 )
		, HoldType( 0 )
	{
	}

	ushort			PropID;			// 属性唯一id
	std::string 	Name;			// 属性名称
	bool			CanRep;			// 可否被替换
	uchar			BeWatchMode;	// 被观察模式
	GridPosArray	HoldGrid;		// 占据网格
	GridPosArray	StopGrid;		// 阻挡网格
	GridPosArray	LayGrid;		// 摆放网格
	ushort			SelfHeight;		// 自身高度(厘米)	
	ushort			OffsetHeight;	// 偏移高度(厘米)
	uint			LayType;		// 放置类型(用位表示)
	uint			AcceptLay;		// 可放置其上的类型(用位表示)
	uint			LightID;		// 携带光源
	uchar			HoldType;		// 占据类型，可用这来判断是否是靠墙物件
};
typedef std::map<ushort, HouseCompPropRow>		HouseCompPropTable; 


/** 房屋部件表
*/
struct HouseCompRow
{
	HouseCompRow()
		: ID( 0 )
		, PropID( 0 )
		, DisID( 0 )
		, LoopID( 0 )
	{
	}

	uint				ID;			// 部件ID
	std::string			Name;		// 部件名
	ushort				PropID;		// 房屋部件属性
	uint				DisID;		// 图形表现ID
	Ogre::Vector3		Pos;		// 位移
	Ogre::Quaternion	Rot;		// 旋转
	Ogre::Vector3		Scale;		// 缩放
	uint				LoopID;		// 环形菜单
	uint				LampType;	// 灯具类型（用于开关灯动画）
	std::vector<uint>	Particle;	// 粒子
};
typedef stdext::hash_map<uint, HouseCompRow>		HouseCompTable; 


/** 房屋网格类型表
*/
struct HouseGridTypeRow
{
	HouseGridTypeRow()
		: TypeID( 0 )
		, ShowGridFitment( false )
		, AcceptLay( 0 )
	{
	}

	ushort				TypeID;		// 类型唯一ID
	std::string			Name;		// 属性名称
	Ogre::ColourValue	ColorSign;	// 网格的颜色标志
	bool				ShowGridFitment;	// 在家装模式显示网格外框
	uint				AcceptLay;		// 可放置其上的类型(用位表示)
	bool				InDoor;		//属于室内或室外
};
typedef std::map<ushort, HouseGridTypeRow>		HouseGridTypeTable; 


/** 房屋相关表格管理器
*/
class HouseTableManager : public Ogre::Singleton<HouseTableManager>
{
public:
	const PlayerHouseRow*		getPlayerHouseRow( int id ) const; 
	const HouseCompPropRow*		getHouseCompPropRow( ushort id ) const; 
	const HouseCompRow*			getHouseCompRow( uint id ) const; 
	const HouseGridTypeRow*		getHouseGridTypeRow( ushort id ) const;
public:
	const PlayerHouseTable&		getPlayerHouseTable() const {return mPlayerHouseTable;} 
	const HouseCompPropTable&	getHouseCompPropTable() const {return mHouseCompPropTable;} 
	const HouseCompTable&		getHouseCompTable() const {return mHouseCompTable;} 
	const HouseGridTypeTable&	getHouseGridTypeTable() const {return mHouseGridTypeTable;} 

	void load();

private:
	bool extract2DPos( const std::string & s, int & x, int & y );
private:
	void loadPlayerHouseTable();
	void loadHouseCompPropTable();
	void loadHouseCompTable();
	void loadHouseGridTypeTable();

private:	
	PlayerHouseTable	mPlayerHouseTable;
	HouseCompPropTable	mHouseCompPropTable; 
	HouseCompTable		mHouseCompTable;
	HouseGridTypeTable	mHouseGridTypeTable;

};


#endif