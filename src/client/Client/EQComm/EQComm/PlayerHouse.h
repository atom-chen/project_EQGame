/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-4-14
* 描述: 玩家房屋
*       
******************************************************************/
#ifndef PlayerHouse_H
#define PlayerHouse_H

#include "NetPacket.h"
#include "EQGameScene.h"

class HouseSceneGrid;
class HouseScene;

class PlayerHouseListener : public EQGameScene::Listener
{
	virtual void postCreateScene(EQGameScene* s);
	virtual void preDestroyScene(EQGameScene* s);
};

class PlayerHouse : public EQGameScene
{
	friend PlayerHouseListener;
public:
	PlayerHouse(uint id, bool preview);
	~PlayerHouse(void);

	void createHousedata(std::string& owner, house_data& data);

	//当前使用物品在仓库在的位置
	void setCurUsingBagIdx(int idx){mCurUsingBagIdx=idx;}
	int  getCurUsingBagIdx(){return mCurUsingBagIdx;}

	//
	void reqPlace(req_placed_furniture& place);
	void reqMove(req_move_furniture& move);
	void reqRecovery(house_furniture* fit);

	uint64 getInstanceID( int floor, int x, int z, int bottom);
	const house_furniture* getFurniture(int floor, int x, int z, int bottom);	// (2011/9/22 add by hezhr)
	void getAllInstanceID(std::vector<uint64>& ins);
	house_furniture* getFitByGird( int x, int z);
	uint getUidByInstanceID(uint64 instance_id);

	void addOneFurniture(house_furniture* fit);
	void recoveryOneFurniture(uint64 instance_id);
	void reqMoveFit( int x, int z);
	uint64 moveFit( HouseSceneGrid * oldGrid, 
					float	x, 
					float	z, 
					int		face, 
					int		height,
					int		floor,
					int		bottom);

	void setOwner(std::string& owner) { mOwner = owner; }
	std::string& getOwner() { return mOwner; }

	bool changeRoomMaterial(int floor_id, int room_id, int type, std::string tex);
	std::string getRoomMaterial(int floor_id, int room_id, int type);	// 获取房屋对应部件的材料 (2011/7/25 add by hezhr)
	int getLevel()const{return mLevel;}

	bool isPreview()  { return mPreview; }

	void setBornPos(Ogre::Vector3& pos) { mBornPos = pos; }
	Ogre::Vector3& getBornPos() { return mBornPos; }

	void setHouseTpltID(int id){mHouseTpltID=id;}

private:
	void clearHouse();
	void putAllWallPaper();
	void putAllFurniture();
	void putOneFurniture(house_furniture& fur);
	void putAllComponent();
	void putOneComponent(house_component_tex& fur);
	void putOneComponent(house_component_mesh& fur);
	void delOneFurniture(uint64 instance_id);

	uint64								mInstance_id;
	std::vector<house_furniture>		mFurniture_vec;
	std::vector<room_tex>				mRoom_tex_vec;
	std::vector<house_component_tex>	mComponent_tex_vec;
	std::vector<house_component_mesh>	mComponent_mesh_vec;

	std::map<uint64, uint>				mCompDisUid;
	HouseScene* mHc;
	
	//当前使用物品在仓库在的位置
	int mCurUsingBagIdx;

	int mLevel;

	//房主
	std::string mOwner;
	int mHouseTpltID;

	//是否是预览
	bool mPreview;

	//出生地
	Ogre::Vector3 mBornPos;

public:
	//一些临时家具，目前只用于商城预览
	//以后如果还用于其他地方，可考虑和原有机制合并
	struct TempCompInfo
	{
		house_furniture comp;
		house_furniture rep;
	};
	typedef std::map<uint, TempCompInfo> TempCompTable; 

	TempCompTable		mTempComp;
 	bool mTempMark;
 
 	void setTempMark(bool mark) { mTempMark = mark; }
 	void showAllTempMark();
 	void hideAllTempMark();
 	uint addTempFurniture(house_furniture& comp, house_furniture& rep);
 	void delTempFurniture(uint uid);
 	bool getTempFurniture(uint uid, house_furniture& comp);
 	void clearTempFurniture();
 	uint isTempFurniture(HouseSceneGrid* grid, int bottom);
	void moveTempFurniture(uint uid, int floor, HouseSceneGrid* grid, int bottom, int face);
	uint fetchTempFurnitureRep(uint uid);
};

#endif