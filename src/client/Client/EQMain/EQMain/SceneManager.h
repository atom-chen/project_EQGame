/******************************************************************************
* 作者: Y3
* 时间: 2009-12-01
* 描述: 场景管理器
******************************************************************************/
#ifndef SceneManager_H
#define SceneManager_H

#include "EQMainPrerequisites.h"

#include "AmbientMgr.h"

class HouseScene;
class SceneInfo;
class Terrain;
class TerrainData;
class FarmScene;

/** 场景管理器.
@remarks 目前的实现可能只是临时做法. 一般来说同一时刻只能有一个场景
*/
class EQSceneManager : public Ogre::Singleton<EQSceneManager>
{
public:
	EQSceneManager();
	~EQSceneManager();

	/// 创建空的房屋
	HouseScene* createNullHouseScene(Ogre::Camera* cam);
	void destroyHouseScene();
	HouseScene* getHouseScene() { return mHouseScene; }


	void loadSceneInfo(const std::string& sceneName, bool inEditor = false);
	void resetSceneInfo();
	SceneInfo* getSceneInfo() {return mSceneInfo;}
	Terrain* getTerrain();
	TerrainData* getTerrainData();


	FarmScene* loadFarmScene();
	void destroyFarmScene();
	FarmScene* getFarmScene() {return mFarmScene;}


	/// 加载房屋和场景
	void loadHouseAndScene( const std::string& sceneName, Ogre::Camera* cam, bool inEditor = false, bool cutScene = true );
	void destroyHouseAndScene();
	/// 删除所有场景
	void destroyScene();

	void update( float delta );

	void turnAllLight(bool on);
	void changeSkybox(std::string skybox);

protected:
	HouseScene*	mHouseScene;
	SceneInfo* mSceneInfo;
	FarmScene* mFarmScene;
	AmbientMgr*	mAmbientMgr;
};

#endif
