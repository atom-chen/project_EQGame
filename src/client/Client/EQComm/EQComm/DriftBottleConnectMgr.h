/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		30/5/2011	14:38
* 文件: 	DriftBottleConnectMgr.h
* 描述:		漂流瓶系统数据链接类
******************
*********************************************************************/
#ifndef DRIFTBOTTLECONNECTMGR_H
#define DRIFTBOTTLECONNECTMGR_H

#include "EQMainPrerequisites.h"
#include "MsgHandleRegist.h"
#include "NetPacket.h"

class DriftBottleConnectMgr : public MemberMsgHandleRegistHelper
{
public:
	DriftBottleConnectMgr();
	~DriftBottleConnectMgr();

// 	static DriftBottleConnectMgr& getSingleton()
// 	{
// 		static DriftBottleConnectMgr s_db_connect;
// 		return s_db_connect;
// 	}

public:
	/**
	*/
	void rec_msg_dropBottle(MsgPack &recBuff);

	/**
	*/
	void rec_msg_pickBottle(MsgPack &recBuff);

	/**
	*/
	void rec_msg_bottlesInfo(MsgPack &recBuff);

	/**
	*/
	void rec_msg_todayBottleInfo(MsgPack &recBuff);

	/**
	*/
	void rec_msg_openBottlePool(MsgPack &recBuff);

	/**
	*/
	void reqDelBottleLog(uint64 del_id);
	
	/**
	*/
	void reqDropBottle(std::string content, int bottle_type, int req_sex = -1, int acc_visible = -1);

	/**
	*/
	void reqPickBottle();

	/**
	*/
	void reqAllBottlesInfo();

	/**
	*/
	void reqEqUserInfo(std::string req_acc);


private:
	/**
	*/
	void _regist_msgHandler();
protected:
private:
	bool mHadReqAllBottlesInfo;

};
#endif
