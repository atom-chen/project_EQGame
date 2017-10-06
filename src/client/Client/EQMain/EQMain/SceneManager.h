/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-01
* ����: ����������
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

/** ����������.
@remarks Ŀǰ��ʵ�ֿ���ֻ����ʱ����. һ����˵ͬһʱ��ֻ����һ������
*/
class EQSceneManager : public Ogre::Singleton<EQSceneManager>
{
public:
	EQSceneManager();
	~EQSceneManager();

	/// �����յķ���
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


	/// ���ط��ݺͳ���
	void loadHouseAndScene( const std::string& sceneName, Ogre::Camera* cam, bool inEditor = false, bool cutScene = true );
	void destroyHouseAndScene();
	/// ɾ�����г���
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
