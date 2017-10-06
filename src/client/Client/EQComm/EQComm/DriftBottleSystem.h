/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		30/5/2011	14:17
* 文件: 	DriftBottleSystem.h
* 描述:		漂流瓶系统 Singleton
******************
*********************************************************************/
#ifndef DRIFTBOTTLESYSTEM_H
#define DRIFTBOTTLESYSTEM_H

#include "DriftBottleMgr.h"
#include "DriftBottleConnectMgr.h"

#define PICK_MAX 3
#define DROP_MAX 6
class DriftBottleSystem
{
public:
	/**
	*/
	DriftBottleSystem();
	~DriftBottleSystem();

	static DriftBottleSystem& getSingleton()
	{
		static DriftBottleSystem ms_db_sys;
		return ms_db_sys;
	}

public:
	/**
	*/
	bool isDbMgrCreate();

	/**
	*/
	bool isDbCMgrConnect();

	/**
	*/
	DriftBottleMgr* getDriftBottleMgrPtr();

	/**
	*/
	void dropBottle(std::string content, int bottle_type, int req_sex, int acc_visible);

	/**
	*/
	void pickBottle();

	/**
	*/
	void deleteBottle(uint del_id,int own_list);
	/**
	*/
	void viewBottle();

	/**
	*/
	void setDropCount(int count);

	/**
	*/
	void setPickCount(int count);

	/**
	*/
	int getDropCount(){return mDropCount;}

	/**
	*/
	int getPickCount(){return mPickCount;}

	/**
	*/
	std::string caculateTimeInterval(std::string sec_str);

	/**
	*/
	void reqEqUserInfo(std::string req_acc);

	/**
	*/
	void update();

protected:
private:
	DriftBottleMgr* mDbMgr;
	DriftBottleConnectMgr* mDbCMgr;
	int mIsdrop;
	int mIspick;
	int mDropCount;
	int mPickCount;

};

#define sDriftBottleSys DriftBottleSystem::getSingleton()
#endif
