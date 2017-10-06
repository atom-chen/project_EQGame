#include "EQMainPCH.h"

#include "SceneManager.h"
#include "HouseScene.h"
#include "HouseResManager.h"
#include "SystemGcs.h"
#include "HouseSceneSerializer.h"
#include "TerrainData.h"
#include "SceneInfo.h"
#include "FarmScene.h"

//-----------------------------------------------------------------------
template<> EQSceneManager* Ogre::Singleton<EQSceneManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
EQSceneManager::EQSceneManager() :
mHouseScene(0),
mFarmScene(0)
{
	mAmbientMgr = new AmbientMgr();
	mSceneInfo = new SceneInfo;
}
//-----------------------------------------------------------------------
EQSceneManager::~EQSceneManager()
{
	destroyScene();

	delete mSceneInfo;
	delete mAmbientMgr;
}
//-----------------------------------------------------------------------
HouseScene* EQSceneManager::createNullHouseScene(Ogre::Camera* cam)
{
	if ( mHouseScene )
	{
		EQ_EXCEPT("House scene is created!", "SceneManager::createNullHouseScene" );
	}

	mHouseScene = new HouseScene(cam);
	return mHouseScene;
}
//-----------------------------------------------------------------------
void EQSceneManager::destroyHouseScene()
{
	if ( mHouseScene )
	{
		delete mHouseScene;
		mHouseScene = NULL;
	}
}
//-----------------------------------------------------------------------
void EQSceneManager::loadSceneInfo(const std::string& sceneName, bool inEditor)
{
	resetSceneInfo();

	HouseSceneSerializer serializer;
	serializer.load(sceneName, inEditor, false);
	serializer.createScene(NULL);
}
//-----------------------------------------------------------------------
void EQSceneManager::resetSceneInfo()
{
	mSceneInfo->reset();
}
//-----------------------------------------------------------------------
Terrain* EQSceneManager::getTerrain()
{

	return mSceneInfo->getTerrain();
}
//-----------------------------------------------------------------------
TerrainData* EQSceneManager::getTerrainData()
{

	return mSceneInfo->getTerrainData();
}

//-----------------------------------------------------------------------
void EQSceneManager::update( float delta )
{
	mSceneInfo->update(delta);

	if ( mHouseScene )
		mHouseScene->update(delta);
}
//-----------------------------------------------------------------------
FarmScene* EQSceneManager::loadFarmScene()
{
	destroyFarmScene();

	mFarmScene = new FarmScene;
	mFarmScene->initialFromXml();
	mFarmScene->setInitialState();

	return mFarmScene;
}
//-----------------------------------------------------------------------
void EQSceneManager::destroyFarmScene()
{
	if ( mFarmScene )
	{
		delete mFarmScene;
		mFarmScene = NULL;
	}
}
//-----------------------------------------------------------------------
void EQSceneManager::loadHouseAndScene( const std::string& sceneName, Ogre::Camera* cam, bool inEditor, bool cutScene)
{
	destroyHouseAndScene();

	HouseSceneSerializer serializer;
	serializer.load(sceneName, inEditor);
	serializer.createScene(cam);

	if ( !mSceneInfo->isInStaticScene() && cutScene  )
	{
		mSceneInfo->enterOctreeScene(cam);
	}
}
//-----------------------------------------------------------------------
void EQSceneManager::destroyHouseAndScene()
{
	destroyHouseScene();
	resetSceneInfo();

}
//-----------------------------------------------------------------------
void EQSceneManager::destroyScene()
{
	destroyHouseAndScene();
	//Å©³¡
	destroyFarmScene();
}
//-----------------------------------------------------------------------
void EQSceneManager::turnAllLight(bool on)
{
	if(mHouseScene)
		mHouseScene->turnAllLight(on);

	if(mSceneInfo)
		mSceneInfo->turnAllLight(on);
}
//-----------------------------------------------------------------------
void EQSceneManager::changeSkybox(std::string skybox)
{
	if(mSceneInfo)
		mSceneInfo->changeSkybox(skybox);
}