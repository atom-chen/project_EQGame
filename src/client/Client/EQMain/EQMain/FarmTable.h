/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		1/7/2010	17:25
* 文件: 	FarmTable.h
* 描述:		加载农场相关表格管理器
******************
*********************************************************************/
#ifndef FARMTABLE_H
#define FARMTABLE_H

#include "EQMainPrerequisites.h"


//农作物各个阶段
enum eCropStages
{
	CS_SPROUT = 0,		//发牙
	CS_SLEAF,			//小叶子
	CS_BLEAF,			//大叶子
	CS_BLOOM,			//开花
	CS_FRUIT,			//果实
	CS_PICK,			//采
	//CS_OTHER,			//变
	CS_DIED,			//死
	CROP_STAGE_NUM
};
/**
*/
struct CropStaNameRow 
{
	CropStaNameRow()
		:id(0)
		,sprout_name("")
		,s_leaf_name("")
		,b_leaf_name("")
		,bloom_name("")
		,fruit_name("")
		,pick_name("")
	{
	}

	int id;
	std::string sprout_name;
	std::string s_leaf_name;
	std::string b_leaf_name;
	std::string bloom_name;
	std::string fruit_name;
	std::string	pick_name;
};
typedef stdext::hash_map<uint,CropStaNameRow> CropStaNameTable;

/**农场鼠标动画
*/
struct MouseAnimateRow 
{
	MouseAnimateRow()
		:id(0)
		,name("")
		,frames(0)
		,time(0)
		,imgset("")
		,icon("")
	{
	}

	int id;
	std::string name;
	int frames;
	int time;
	std::string imgset;
	std::string icon;
};
typedef stdext::hash_map<uint,MouseAnimateRow> MouseAnimateTable;

/**农场道具结构
*/
struct FarmPropRow 
{
	FarmPropRow()
		:prop_id(0)
		,prop_type(0)
	{
	}

	uint prop_id;
	uint prop_type;

};
typedef stdext::hash_map<uint,FarmPropRow> FarmPropTable;

/**农场商店的物品结构
*/
struct FarmShopRow 
{
	FarmShopRow()
		:item_id(0)
		,gameprice(0)
		,eqprice(0)
		,intro("")
		,itemLevel(0)
		,special("")
		,repeatBuy(0)
	{
	}

	uint		item_id;
	uint		gameprice;
	uint		eqprice;
	std::string intro;
	uint		itemLevel;
	std::string special;
	int			repeatBuy;
};
typedef stdext::hash_map<uint, FarmShopRow> FarmShopTable;

//
struct PlantDisplayRow 
{
	PlantDisplayRow()
	{
		ZeroMemory(farmdisplayid,CROP_STAGE_NUM * sizeof(int));
	}

	int seedid;
	int farmdisplayid[CROP_STAGE_NUM];
};
typedef stdext::hash_map<int, PlantDisplayRow> PlantDisplayTable;

//
struct FarmDisplayRow 
{
	FarmDisplayRow() : id(0)
		, modelDisplayid(0)
		, displayType(0)
	{
	}
	int id;
	int modelDisplayid;
	int displayType;
};
typedef stdext::hash_map<int, FarmDisplayRow> FarmDisplayTable;

struct SeedRow
{
	SeedRow()
		:id(0)
		,name("")
		,expect_output(0)
		,sprout_time(0)
		,s_leaf_time(0)
		,b_leaf_time(0)
		,bloom_time(0)
		,fruit_time(0)
		,pick_time(0)
		,use_level(0)
		,pick_exp(0)
		,fruit_id(0)
	{
		ZeroMemory(stageTimer,sizeof(int)*CROP_STAGE_NUM);
	}

	uint id;
	std::string name;

	uint expect_output;

	uint sprout_time;
	uint s_leaf_time;
	uint b_leaf_time;
	uint bloom_time;
	uint fruit_time;
	uint pick_time;
	uint use_level;
	uint pick_exp;
	uint fruit_id;

	int	 stageTimer[CROP_STAGE_NUM];
};
typedef stdext::hash_map<uint, SeedRow> SeedTable;

// 装饰表
struct OrnamentRow
{
	OrnamentRow()
		:id(0)
		,type(0)
	{
	}

	uint id;
	uint type;
};
typedef stdext::hash_map<uint, OrnamentRow> OrnamentTable;

//农场等级表
struct FarmLevelRow 
{
	FarmLevelRow()
		:level(1)
		,next_exp(0)
		,sow_exp(0)
		,water_exp(0)
		,weed_exp(0)
		,assart_exp(0)
	{
	}

	uint level;
	uint next_exp;
	uint sow_exp;
	uint water_exp;
	uint weed_exp;
	uint worm_exp;
	uint assart_exp;
};
typedef stdext::hash_map<uint, FarmLevelRow> FarmLevelTable;

//农场装饰表
struct FarmDecorationRow 
{
	FarmDecorationRow() : id(0)
		, type(0)
		, period(0)
		, experience(0)
	{
	}

	int id,
		type,
		period,
		experience;
	std::vector<int>	static_model,
						dynamic_model;
	std::string			scene_name;
};
typedef stdext::hash_map<int, FarmDecorationRow> FarmDecorationTable;

// 农场任务表(2011/4/15 add by hezhr)
struct FarmTaskRow
{
	FarmTaskRow():id(0) {}

	uint						id;
	std::string 				title;
	std::string					intention;
	std::string					describe;
	std::vector<std::string>	imageset;
	std::vector<std::string>	image;
	uint						item_reward;
	uint						item_count;
	uint						game_reward;
	uint						eq_reward;
	uint						exp;
	uint						time_limit;
	uint						is_repeat;
};
typedef stdext::hash_map<uint, FarmTaskRow> FarmTaskTable;

/**农场相关表格加载管理器
*/
class FarmTableMgr : public Ogre::Singleton<FarmTableMgr>
{
public:
	void load();

public:
	const FarmShopRow* getFarmShopRow(uint index) const;
	const int getFarmShopTableCount() const;
	const FarmShopRow* getFarmItemRow(uint id) const;
	const SeedRow* getSeedRow(uint id) const;
	const OrnamentRow* getOrnamentRow(uint id) const;
	const FarmLevelRow* getFarmLevelRow(uint lv) const;
	const FarmDisplayRow* getFarmDisplayRow(int id) const;
	const PlantDisplayRow* getPlantDisplayRow(int id) const;
	const FarmDecorationRow* getFarmDecorationRow(int id) const;
	const MouseAnimateRow* getMouseAnimateRow(int id) const;
	const CropStaNameRow* getCropStaNameRow(int id) const;
	const FarmPropRow* getFarmPropRow(int id) const;
	const FarmTaskRow* getFarmTaskRow(uint id) const;
	int getFarmTaskImagesetCount(uint id) const;
	std::string getFarmTaskImageset(uint id, uint i) const;
	int getFarmTaskImageCount(uint id) const;
	std::string getFarmTaskImage(uint id, uint i) const;

protected:

private:
	void loadFarmShopTable();
	void loadSeedTable();
	void loadOrnamentTable();
	void loadFarmLevelTable();
	void _loadPlantDisplayTable();
	void _loadFarmDisplayTable();
	void _loadFarmDecorationTable();
	void loadMouseAnimateTable();
	void loadCropStaNameTable();
	void _loadFarmPropTable();
	void _loadFarmTaskTable();

private:
	FarmShopTable	mFarmShopTable;
	SeedTable		mSeedTable;
	OrnamentTable   mOrnamentTable;
	FarmLevelTable mFarmLevelTable;

	FarmDisplayTable mFarmDisplayTable;
	PlantDisplayTable mPlantDisplayTable;
	FarmDecorationTable mFarmDecorationTable;
	MouseAnimateTable mMouseAnimateTable;
	CropStaNameTable mCropStaNameTable;
	FarmPropTable	mFarmPropTable;
	FarmTaskTable m_farmTaskTable;
};

#define sFarmTableMgr FarmTableMgr::getSingleton()
#endif 