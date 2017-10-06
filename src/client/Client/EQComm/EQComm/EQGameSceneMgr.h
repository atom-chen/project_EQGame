/********************************************************************
* 作者: lf
* 时间: 2009-5-10
* 描述: 场景管理
**********************************************************************/
#ifndef EQGameSceneMgr_H
#define EQGameSceneMgr_H

#include "EQMainPrerequisites.h"

#include "EQGameScene.h"

struct notify_house_data;
struct req_enter_common_scene;
struct req_enter_player_house;
class PlayerHouse;
class Particle;
namespace SearchSystem
{
	class NavSearchSystem;
};

class EQGameSceneMgr : public Ogre::Singleton<EQGameSceneMgr>
{
public:
	EQGameSceneMgr();
	~EQGameSceneMgr(void);

	enum eSceneType
	{
		SCENE_NONE = 0,
		SCENE_HOUSE,
		SCENE_COMMON,
		SCENE_FARM,
		SCENE_HOUSE_PREVIEW,
	};

	enum eWeather
	{
		WEATHER_NONE = 0,
		WEATHER_RAIN,
		WEATHER_SNOW,
	};

private:
	Ogre::SceneManager* mSceneMgr;
	EQGameScene* mGameScene;
	eSceneType mSceneType;
	SearchSystem::NavSearchSystem*	mNavSys;

	Particle* mWeatherParticle;
	eWeather mWeatherType;

	DWORD mLoadingBegin;
	bool mIsLoading;

private:
	void _createScene(uint sceneID, uint bornID);
	void _refreshWeather();

public:
	void init(Ogre::SceneManager* sm);
	void initDayNightTimer();
	void update();
	void cleanup();

	void startLoading();
	void endLoading();
	bool hasScene() { return mGameScene!=NULL; }
	int  checkTeleportID() { return mGameScene->checkTeleportID(); }

	void recHousedata(std::string& owner, house_data& data);
	bool reqChangeScene(eSceneType sceneType, std::string account);
	void reqChangeSceneByGuide(uint guideId, std::string account,uint copy_id);
	void reqTeleport(uint id,uint copy_id);
	void reqEnterCommonScene( req_enter_common_scene & commonScene);
	void reqEnterPlayerHouse( req_enter_player_house & playerHouse);

	void changeScene(eSceneType sceneType, uint sceneID, uint bornID);
	eSceneType getSceneType() const { return mSceneType; }

	bool getDayNight();
	void changeDayNight(bool daynight, bool immediately=false);
	void switchDayNight(bool immediately=false);

	//请求预览房屋数据
	void reqPreviewHousedata(int houseTpltID);
	
	//请求回服务器当前场景数据
	void reqReseeScene();

	//请求离开农场
	void reqLeaveFarm();

	bool inMyHouse();
	PlayerHouse* getHouse();

	SearchSystem::NavSearchSystem* getNavSearchSystem(){return mNavSys;}

	bool getIsLoading(){ return mIsLoading; }
	void setIsLoading(bool b) { mIsLoading = b; }

	void setWeatherType(eWeather weather);
	void setWeatherPos(const Ogre::Vector3& pos);
	void reqCommSeceneState(uint guide_id);
};

#define sEQGameSceneMgr EQGameSceneMgr::getSingleton()
#endif