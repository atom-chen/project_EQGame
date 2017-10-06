/******************************************************************************
* 作者: Y3
* 时间: 2010-07-30
* 描述: 农场场景
******************************************************************************/
#ifndef FarmScene_H
#define FarmScene_H

#include "EQMainPrerequisites.h"
#include "ModelGcs.h"
#include "PickObjectProxy.h"

class FarmScene;
#define GRASS_MAX 3
//土地状态
enum eLandState
{
	LS_WASTE = 0,		//荒地
	LS_NORMAL,			//普通可种植地
	LS_DROUGHT_NORMAL,	//干旱普通地
	LS_RED,				//红地
	LS_DROUGHT_RED,		//干旱红地
	LS_NUM,
};

/// 农场土地块区域信息
struct FarmLandAreaInfo {
	Ogre::Vector3		beginPos;
	int					xSize;
	int					zSize;
};


/// 农场物件信息
struct FarmObjectInfo
{
	std::string			mName;		//!< 物件名
	Ogre::Vector3		mPos;
	float				mYDegree;
	std::string			mMesh;
	std::string			mMtrl;
};


/// 农场种植单位
class FarmPlantUnit
{
public:
	class PickObjectUnit : public PickObject
	{
		friend class FarmPlantUnit;
	public:
		/// 名字为 FarmPlantUnit::PickObjectUnit
		const std::string& getClassName();

		/// 获取名称, 可以根据名称才判定选中的物件. 现在3个物件的名称分别是"Land", "Grass", "Plant"
		const std::string& getName() {return mName;}

		FarmPlantUnit* getCreator() {return mCreator;}
	protected:
		FarmPlantUnit*	mCreator;
		std::string		mName;
	};

public:
	FarmPlantUnit(FarmScene& creator, int index);
	~FarmPlantUnit();

	/// 变更土地
	void changeLand(const std::string& mesh, const std::string& material = "");
	/// 复位土地为原来的效果
	void resetLand();

	/// 种植植物, 如果已经有植物, 则替换
	void seedPlant(const std::string& mesh, const std::string& material = "");
	/// 移除植物
	void removePlant();

	/// 种草
	void seedGrass();

	/// 标识扩展
	void signExpand();
	/// 除草
	void removeGrass();

	/// 标识扩展
	void removeSignExpand();
	/// 获取单位应有的坐标
	const Ogre::Vector3& getUnitPos() {return mPos;}

	/// 获取索引编号
	int getIndex() {return mIndex;}

	ModelGcs* getLand() {return mLand;}
	ModelGcs* getGrass(int idx) {return idx >= GRASS_MAX ? NULL : mGrass[idx];}
	ModelGcs* getPlant() {return mPlant;}

	const Ogre::Vector3& getPos(){return mPos;}

protected:
	void calcPos();

protected:
	FarmScene&		mCreator;
	Ogre::Vector3	mPos;

	int 			mIndex;
	ModelGcs*		mLand;
	ModelGcs*		mGrass[GRASS_MAX];
	ModelGcs*		mPlant;
	ModelGcs*		mSignExpand;
	PickObjectProxy	mLandPxy;
	PickObjectProxy	mGrassPxy[GRASS_MAX];
	PickObjectProxy	mPlantPxy;
	PickObjectProxy	mSignExpandPxy;
	PickObjectUnit	mLandPo;
	PickObjectUnit	mGrassPo[GRASS_MAX];
	PickObjectUnit	mPlantPo;
	PickObjectUnit	mSignExpandPo;
};


/// 农场物件
class FarmObject : public PickObject
{
public:
	FarmObject(FarmScene& creator, const std::string& name);
	~FarmObject();

	const std::string& getClassName();

	void changeObject(const std::string& mesh, const std::string& material = "");

	const std::string& getName() {return mName;}
	ModelGcs* getModel() {return mObject;}

protected:
	FarmScene&			mCreator;

	std::string			mName;
	ModelGcs*			mObject;
	PickObjectProxy		mObjectPxy;
};


/** 农场场景
@remarks 可以和HouseScene并存, 不过目前不允许HouseScene有房屋存在. 这方面可以有待
	日后扩展
@par 与农场物件选取的功能在操控方面处理更合适.
*/
class FarmScene
{
	friend class FarmPlantUnit;
	friend class FarmObject;
public:
	FarmScene();
	~FarmScene();

	/// 获取土地的个数
	int getLandSize();

	/// 变更土地
	void changeLand(int index, const std::string& mesh, const std::string& material = "");
	/// 复位土地为原来的效果
	void resetLand(int index);

	/// 种植植物, 如果已经有植物, 则替换
	void seedPlant(int index , const std::string& mesh, const std::string& material = "");
	/// 移除植物
	void removePlant(int index);

	/// 种草
	void seedGrass(int index);
	
	/// 标识扩展
	void signExpand(int index);
	/// 除草
	void removeGrass(int index);

	/// 标识扩展
	void removeSignExpand(int index);


	/// 增加或更换农场物件
	void changeFarmObject(const std::string& name, const std::string& mesh, const std::string& material = "");
	/// 移除农场物件
	void removeFarmObject(const std::string& name);


	//??植物和土地的高亮处理, 是否需要在这一并处理??

	/// 复位农场初始效果. 不包括场景效果
	void setInitialState();
	/// 更换场景
	//TODO:


	/// 通过索引, 获取农场种植单位
	FarmPlantUnit* getFarmPlantUnit(int index);

	/// 获取农场物件
	FarmObject* getFarmObject(const std::string& name);

	Ogre::SceneNode* getSceneNode();

	/// 从xml初始化
	void initialFromXml();

	//根据状态更换
	void changeLand(int index, int landstate);

	//得到地块坐标
	const Ogre::Vector3& getLandUintPos(int index);

protected:
	void initPlantUnit();
	void initFarmObject();

protected:
	/// 土地之间的间隔
	float									mLandInterval;
	/// 植物种植离土地中心点的高度
	float									mSeedHeight;
	/// 土地块信息
	std::vector<FarmLandAreaInfo>			mLandAreaInfos;
	/// 物件信息
	std::map<std::string, FarmObjectInfo>	mObjectInfos;

	/// 农场种植单位实例列表
	std::vector<FarmPlantUnit*>				mFarmPlantUnits;
	/// 农场物件实例列表
	std::map<std::string, FarmObject*>		mFarmObjects;

	/// 初始土地模型
	std::string								mInitialLandMesh;
	std::string								mInitialLandMaterial;
public:
	/// 杂草模型,扩建标识，土地模型
	std::string								mGrassMesh;
	std::string								mGrassMaterial
											,mExpandSignMesh
											,mExpandSignMaterial
											,mLandStateMesh[LS_NUM]
											,mLandStateMaterial[LS_NUM]
											;
	Ogre::Vector3 mGrassPos[GRASS_MAX];

};



#endif

