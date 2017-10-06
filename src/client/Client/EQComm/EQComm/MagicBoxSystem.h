/******************************************************************
* 作者: xj
* 时间: 2011-4-6
* 描述: 宝箱系统
*******************************************************************/
#pragma once

#ifndef __MagicBoxSystem_H
#define __MagicBoxSystem_H

#include "MsgHandleRegist.h"
#include "PickObjectProxy.h"
#include "ModelGcs.h"
#include "NetPacket.h"
#include "HouseScene.h"
#include "EnumDef.h"

struct MagicBoxInfo
{
	uint64 inst_id; //实例
	uint item_id; //物品
	uint money; //金钱
	uint model_id; //模型

	int x,y,z;//格子坐标
	Ogre::Vector3 pt;//绝对坐标

	int type;//mbt_magic_box:宝箱;mbt_garbage:垃圾

	MagicBoxInfo(void)
		:inst_id(0)
		,item_id(0)
		,money(0)
		,model_id(0)
		,x(0)
		,y(0)
		,z(0)
		,type(mbt_magic_box)
	{

	}

	void operator = (MagicBoxInfo &info)
	{
		inst_id = info.inst_id;
		item_id = info.item_id;
		money = info.money;
		model_id = info.model_id;
		x = info.x;
		y = info.y;
		z = info.z;
		pt = info.pt;
		type = info.type;
	}
};

class MagicBox
{
public:
	class PickObjectUnit : public PickObject
	{
	public:
		const std::string& getClassName() { return ClassName; }

		void  setCreator(MagicBox* creator){ mCreator = creator; }
		MagicBox* getCreator(){return mCreator;}

		PickObjectUnit():mCreator(NULL){}
	public:
		static const std::string ClassName;

	private:
		MagicBox* mCreator;

	};

protected:
	ModelGcs *mModel;
	MagicBoxInfo mInfo;
	PickObjectUnit  mPickObjectUnit;
	PickObjectProxy mPickObjectProxy;

public:
	void Create(MagicBoxInfo &info);
	void Dispose();
	void Release();
	void setVisible(bool visible){mModel->setVisible(visible);}	// (2011/7/6 add by hezhr)

	MagicBoxInfo &getMagickBoxInfo() { return mInfo; }
	ModelGcs* getModel() const { return mModel;}

	MagicBox(MagicBoxInfo &info);
	~MagicBox(void);
};

typedef std::map<uint64, MagicBox *> MagicBoxMap;

class MagicBoxSystem : public Ogre::Singleton<MagicBoxSystem>, public MemberMsgHandleRegistHelper
{
protected:
	MagicBoxMap mMagicBoxs;
	MagicBox	*mClickbox;

public:
	void handle_house_magic_box(MsgPack &recBuff);
	void handle_pick_magic_box_gain(MsgPack &recBuff);
	void handle_req_empty_box_positions(MsgPack &recBuff);
	void handle_del_house_magic_box(MsgPack &recBuff);
	void handle_add_house_magic_box(MsgPack &recBuff);

	void addMagicBoxs(std::vector<magic_box> &boxs);
	void outMagicBoxPos(std::vector<grid_pos> &pts, int maxCnt);

	void delMagicBox(uint64 instId);
	void clearMagicBoxs();

	void clickMagicBox(MagicBox *mbox);
	bool hasMagicBoxInGrid(HouseSceneGrid *grid);
	uint64 getMagicBoxIdInGrid(HouseSceneGrid *grid);
	MagicBox *getMagicBox(uint64 id);

	MagicBoxMap& getMagicBoxs(void){return mMagicBoxs;}	// (2011/7/6 add by hezhr)

	MagicBoxSystem(void);
	~MagicBoxSystem(void);
	void reqWalkForPickMagicbox();
};

#endif