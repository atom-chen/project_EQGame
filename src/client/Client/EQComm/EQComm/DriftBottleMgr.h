/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		30/5/2011	13:33
* 文件: 	DriftBottleMgr.h
* 描述:		漂流瓶管理器
******************
*********************************************************************/
#ifndef DRIFTBOTTLEMGR_H
#define DRIFTBOTTLEMGR_H

#include "DriftBottle.h"

class DriftBottleMgr
{
public:
	DriftBottleMgr();
	~DriftBottleMgr();

// 	static DriftBottleMgr& getSingleton()
// 	{
// 		static DriftBottleMgr s_db_mgr;
// 		return s_db_mgr;
// 	}

public:
	/**
	*/
	bool addDriftBottle(int bottle_type,DriftBottle* bottle, int own_list = 0);

	/**
	*/
	bool removeDriftBottle(DriftBottle* bottle);

	/**
	*/
	bool removeAllDriftBottle();

	/**
	*/
	
	/**
	*/
	//bool addDriftBottle(int bottle_type);

	/**
	*/
	bool createDriftBottle(bottle_info& bt_info, int own_list = 0, std::string imageset ="", std::string bottle_image ="");

	/**
	*/
	bool deleteDriftBottle(DriftBottle *bottle);

	/**
	*/
	bool deleteDriftBottle(uint64 id, int own_list);

	/**
	*/
	bool isDriftBottlePresent(DriftBottle* bottle);

	/**
	*/
	DriftBottle* getDriftBottle();

	/**
	*/
	bool isBottleExist(DriftBottle* bottle);

	/**
	*/
	bool isBottleExist(bottle_info& bt_info, int own_list);

	/**
	*/
	void bottleListUpdate(DriftBottle* bottle,int own_list);

	/**
	*/
	bool clearDropBottlesList();


	/**辅助函数
	*/
	std::string transformInsIdToStr(uint64 insID);

	/**
	*/
	uint64 transformInsIdToUi64(std::string insId_str);

protected:

private:
	typedef int BottleType;
	typedef std::multimap<BottleType, DriftBottle*> DriftBottleList;
	typedef DriftBottleList::const_iterator Const_BottleIterType;
	typedef DriftBottleList::iterator BottleIterType;
	typedef std::pair<Const_BottleIterType, Const_BottleIterType> BottleTypeRangePair;
	DriftBottleList mDropDriftBottleList;
	DriftBottleList mPickDriftBottleList;
};

// #define sDriftBottleMgr DriftBottleMgr::getSingleton()
#endif
