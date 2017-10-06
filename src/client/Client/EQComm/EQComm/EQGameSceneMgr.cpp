#include "EQCommPCH.h"

#include "EQGameSceneMgr.h"
#include "AmbientMgr.h"
#include "MainPlayer.h"
#include "FitmentInteractMgr.h"
#include "FitmentEvent.h"
#include "NetPacket.h"
#include "EQGameMgr.h"
#include "EQMsgManager.h"
#include "NetPacket.h"
#include "PickMgr.h"
#include "SystemGcs.h"
#include "EQCamera.h"
#include "ObjMgr.h"
#include "Variable.h"
#include "SceneManager.h"
#include "LogicTable.h"
#include "BodyActionMgr.h"
#include "CommonScene.h"
#include "PlayerHouse.h"
#include "GameEventMgr.h"
#include "FunnyFarmScene.h"
#include "EQOgreSys.h"
#include "CEGUIEx/CEGUIMultiLineBox.h"
#include "DynamicUIMgr.h"
#include "CursorMgr.h"
#include "SearchSystem/NavSearchSystem.h"
#include "ConfigOption.h"

#include "SceneInfo.h"
#include "GiftSystem.h"

#include "Timekeeper.h"
#include "timer.h"
#include "SmallModules.h"
#include "MagicBoxSystem.h"
#include "RecycleResource.h"
#include "ParticleMgr.h"
#include "EQVoiceMgr.h"
#include "AppListener.h"
#include "task.h"
#include "GameState.h"

//-----------------------------------------------------------------------
template<> EQGameSceneMgr* Ogre::Singleton<EQGameSceneMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
EQGameSceneMgr::EQGameSceneMgr():
mGameScene(NULL),
mSceneType(SCENE_NONE),
mIsLoading(false),
mWeatherType(WEATHER_NONE),
mWeatherParticle(NULL)
{
	mNavSys = new SearchSystem::NavSearchSystem(0.001f);	//导航网格寻路系统
}
//-----------------------------------------------------------------------
EQGameSceneMgr::~EQGameSceneMgr()
{
	if ( mGameScene )
		delete mGameScene;

	delete mNavSys;
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::init(Ogre::SceneManager* sm)
{
	mSceneMgr = sm;

	new ObjMgr;						//角色管理
	new MainPlayer;					//主控角色
	new FitmentInteractMgr();
	new FitmentEventMgr;			//家具管理

	MainPlayer::getSingleton().init();

	Ogre::Light* mLight = AmbientMgr::getSingleton().createMainLight();

	float x,y,z;

	mLight->setType(Ogre::Light::LT_DIRECTIONAL);

	x = VariableSystem::getSingleton().GetAs_Float("SunDiffuseColourR");
	y = VariableSystem::getSingleton().GetAs_Float("SunDiffuseColourG");
	z = VariableSystem::getSingleton().GetAs_Float("SunDiffuseColourB");
	mLight->setDiffuseColour(Ogre::ColourValue(x, y, z));

	x = VariableSystem::getSingleton().GetAs_Float("SunSpecularColourR");
	y = VariableSystem::getSingleton().GetAs_Float("SunSpecularColourG");
	z = VariableSystem::getSingleton().GetAs_Float("SunSpecularColourB");
	mLight->setSpecularColour(Ogre::ColourValue(x, y, z));

	x = VariableSystem::getSingleton().GetAs_Float("SunDirectionX");
	y = VariableSystem::getSingleton().GetAs_Float("SunDirectionY");
	z = VariableSystem::getSingleton().GetAs_Float("SunDirectionZ");
	mLight->setDirection(Ogre::Vector3( x, y, z )); 
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::cleanup()
{
	ObjMgr::getSingleton().cleanup();
	delete ObjMgr::getSingletonPtr();
	MainPlayer::getSingleton().cleanup();
	delete MainPlayer::getSingletonPtr();

	delete FitmentEventMgr::getSingletonPtr();
	delete FitmentInteractMgr::getSingletonPtr();
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::update()
{
	ObjMgr::getSingleton().update();
	AmbientMgr::getSingleton().update();
	FitmentInteractMgr::getSingleton().update();
	// 人物自身动作, 人与人动作
// 	BodyActionMgr::getSingleton().update();
	ParticleMgr::getSingleton().update();
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::recHousedata(std::string& owner, house_data& data)
{
	//assert(mSceneType==SCENE_HOUSE);

	PlayerHouse* house = dynamic_cast<PlayerHouse*>(mGameScene);
	if(house)
		house->createHousedata(owner, data);

	if (data.house_clean < 30)
	{
		// 运行新手教学任务(房屋清洁度低到30以下)
		if (data.owner == MainPlayer::getSingleton().getAccount())
			MainPlayer::getSingleton().getTask().startNewerTeachTask(15013);		
	}

	// 接收礼物数据
	GiftSystem::getSingleton().recGiftData(data.gift_box_vec);
	//接收宝箱
	MagicBoxSystem::getSingleton().clearMagicBoxs();
	MagicBoxSystem::getSingleton().addMagicBoxs(data.magic_box_vec);
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::reqPreviewHousedata(int houseTpltID)
{
	::req_preview_shop_house req;
	req.house_id = houseTpltID;
	sGameMgr.getEQMsgManager()->Send(req);
	SmallModules::ifReviewHouseing = true;
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::reqReseeScene()
{
	::req_end_preview_house req;
	sGameMgr.getEQMsgManager()->Send(req);
	//SmallModules::ifReviewHouseing = false;
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::reqLeaveFarm()
{
	req_leave_farm req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------------------------------
bool EQGameSceneMgr::reqChangeScene( eSceneType sceneType, std::string account )
{
	switch(sceneType)
	{
	case SCENE_HOUSE:
		{
			req_enter_player_house playerHouse;
			playerHouse.account = account;//MainPlayer::getSingleton().getAccount();
			reqEnterPlayerHouse(playerHouse);
		}

		break;	
	case SCENE_COMMON:
		{
			req_enter_common_scene commonScene;
			commonScene.scene_id = 1;
			reqEnterCommonScene(commonScene);
		}
		break;
	//case SCENE_FARM:
	//	{
	//		req_enter_farm farm;
	//		farm.owner = account;
	//		reqEnterCommonScene(farm);
	//	}
	//	break;

	default:
		assert(0);
	}
	return true;
}
//-------------------------------------------------------------------------------
void EQGameSceneMgr::reqCommSeceneState(uint guide_id)
{
	const GotoRow *row = LogicTableManager::getSingleton().getGotoRow(guide_id);
	assert(row);
	req_common_scene_state req_css;
	req_css.teleport_id = row->target;
	sGameMgr.getEQMsgManager()->Send((INetPacket&)req_css);
}
//-------------------------------------------------------------------------------
void EQGameSceneMgr::reqChangeSceneByGuide( uint guideId, std::string account,uint copy_id )
{
	const GotoRow *row = LogicTableManager::getSingleton().getGotoRow(guideId);
	assert(row);

	/*const TeleportRow *trow = LogicTableManager::getSingleton().getTeleportRow(row->target);
	assert(trow);

	const SceneRow *crow = LogicTableManager::getSingleton().getSceneRow(trow->dst_scene_id);
	assert(crow);

	switch(crow->type)
	{
	case 1:
		{
			req_enter_player_house playerHouse;
			playerHouse.account = account;
			reqEnterPlayerHouse(playerHouse);
		}
		break;
	case 2:
		{
			req_enter_common_scene commonScene;
			commonScene.scene_id = trow->dst_scene_id;
			reqEnterCommonScene(commonScene);
		}
		break;
	default:
		break;
	}*/

	reqTeleport(row->target,copy_id);
}
//-------------------------------------------------------------------------------
void EQGameSceneMgr::reqTeleport( uint id,uint copy_id )
{
	req_teleport pack;
	pack.id = id;
	pack.copy_id = copy_id;
	sGameMgr.getEQMsgManager()->Send((INetPacket&)pack);
}
//-------------------------------------------------------------------------------
void EQGameSceneMgr::reqEnterCommonScene( req_enter_common_scene & commonScene)
{
	sGameMgr.getEQMsgManager()->Send((INetPacket&)commonScene);
}
//---------------------------------------------------------------
void EQGameSceneMgr::reqEnterPlayerHouse( req_enter_player_house & playerHouse)
{
	sGameMgr.getEQMsgManager()->Send((INetPacket&)playerHouse);
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::changeScene(eSceneType sceneType, uint sceneID, uint bornID)
{
	if (sceneType != SCENE_FARM)
	{
		if(luabind::call_function<bool>(sLuaMgr.getL(), "DBS_getDropingFlag")
			|| luabind::call_function<bool>(sLuaMgr.getL(), "DBS_getPickingFlag"))
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_cantChangeSceneTip");
				return;
		}
		sMainPlayer.stopAnimation();
		sMainPlayer.setState(GOS_STAND_IDLE);
	}

	EQVoiceMgr::getSingleton().stopAll();
	ObjMgr::getSingleton().AllPlayerGosNone();

	if(mGameScene && mGameScene->getListener())
		mGameScene->getListener()->preDestroyScene(mGameScene);

	mSceneType = sceneType;
	if(mGameScene)
	{
		EQSceneManager::getSingleton().destroyScene();
		mNavSys->destroy();
		delete mGameScene;

		// 删除礼物模型
		if(NULL != GiftSystem::getSingletonPtr())
			GiftSystem::getSingleton().deleteAllGift();

		// 删除宝箱
		if(NULL != MagicBoxSystem::getSingletonPtr())
			MagicBoxSystem::getSingleton().clearMagicBoxs();

		// 放这里是因为第一次的start由login模块来调
		if(sceneType != SCENE_NONE)
			startLoading();
	}
	else
	{
		GameEventMgr::getSingleton().fireEvent("ENTER_GAME");
	}

	switch(mSceneType)
	{
	case SCENE_HOUSE:
		mGameScene = new PlayerHouse(sceneID, false);
		break;

	case SCENE_COMMON:
		mGameScene = new CommonScene(sceneID);
		break;		

	case SCENE_FARM:
		mGameScene = new FunnyFarmScene(sceneID);
		break;

	case SCENE_HOUSE_PREVIEW:
		mGameScene = new PlayerHouse(sceneID, true);
		break;

	case SCENE_NONE:
		return;

	default:
		assert(0);
	}

	_createScene(sceneID, bornID);
	_refreshWeather();
	AmbientMgr::getSingleton().refresh();

	GameEventMgr::getSingleton().fireEvent("CHANGE_SCENE");

	if(mGameScene->getListener())
		mGameScene->getListener()->postCreateScene(mGameScene);


	// 运行新手教学任务(随机选择出现的，出现条件是每次切换地图后)
	MainPlayer::getSingleton().getTask().randomTeachTask();
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::_createScene(uint sceneID, uint bornID)
{
	const SceneRow* row = LogicTableManager::getSingleton().getSceneRow(sceneID);
	assert(row);
	BornRow* born = LogicTableManager::getSingleton().getBornRow(bornID);

	EQCamera* cam = EQGameMgr::getSingleton().getMainCamera();
	cam->getOgreCam()->setFarClipDistance(row->far_clip_distance);
	cam->setLookAtMax(row->lookatMax);
	cam->setLookAtMin(row->lookatMin);
	cam->setPitchMax(row->pitchMax);
	cam->setPitchMin(row->pitchMin);
	cam->setYawMax(row->yawMax);
	cam->setYawMin(row->yawMin);
	cam->setZoomMax(row->zoomMax);
	cam->setZoomMin(row->zoomMin);
	if(born)
	{
		cam->lookAt(born->lookat);
		cam->pitch(born->pitch);
		cam->yaw(born->yaw);
		cam->zoom(born->zoom);
	}

	EQSceneManager::getSingleton().loadHouseAndScene(row->file, EQGameMgr::getSingleton().getMainCamera()->getOgreCam());
	switch(mSceneType)
	{
	case SCENE_HOUSE:
		{
			if(born)
			{
				getHouse()->setBornPos(born->pos);
				sMainPlayer.setPos(born->pos);
				sMainPlayer.getMainNode()->setOrientation( 
					Ogre::Quaternion((Ogre::Radian)Ogre::Math::DegreesToRadians(Ogre::Real(born->dir)), Ogre::Vector3::UNIT_Y) );
			}

			sMainPlayer.setVisible(true);
			sMainPlayer.setHeadUpVisible(true);
			sGameMgr.getMainCamera()->setFollowMe(true);
		}
		break;
	case SCENE_COMMON:
		{
			//加载寻路数据
			std::string logicName = row->file;
			if (Ogre::StringUtil::endsWith(logicName, ".scene"))
				logicName.erase(logicName.length() - (sizeof(".scene") - 1));
			if ( !mNavSys->load(logicName + ".nav") )
				EQ_EXCEPT("load '" + logicName + ".nav' fail!", "EQGameSceneMgr::_createScene");

			if(born)
			{
				sMainPlayer.setPos(born->pos);
				sMainPlayer.getMainNode()->setOrientation( 
					Ogre::Quaternion((Ogre::Radian)Ogre::Math::DegreesToRadians(Ogre::Real(born->dir)), Ogre::Vector3::UNIT_Y) );
			}

			sMainPlayer.setVisible(true);
			sMainPlayer.setHeadUpVisible(true);
			sGameMgr.getMainCamera()->setFollowMe(true);
		}
		break;
	case SCENE_FARM:
		{
			EQSceneManager::getSingleton().loadFarmScene();

			sMainPlayer.setVisible(false);
			sMainPlayer.setHeadUpVisible(false);
			sGameMgr.getMainCamera()->setFollowMe(false);
		}
		break;
	case SCENE_HOUSE_PREVIEW:
		{
			sMainPlayer.setVisible(false);
			sMainPlayer.setHeadUpVisible(false);
			sGameMgr.getMainCamera()->resetCamMovSpeed(10);
			sGameMgr.getMainCamera()->setFollowMe(false);
		}
		break;
	default:
		assert(0);
		return;
	}

	mGameScene->initTeleports();

	EQVoiceMgr::getSingleton().play(row->voiceid, true);

	EQOgreSys::getSingleton().setContact(AmbientMgr::getSingleton().mContrast);
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::_refreshWeather()
{
	switch(mSceneType)
	{
	case SCENE_NONE:
	case SCENE_HOUSE:
	case SCENE_HOUSE_PREVIEW:
		setWeatherType(WEATHER_NONE);
		break;
	case SCENE_COMMON:
		setWeatherType(mWeatherType);
		setWeatherPos(MainPlayer::getSingleton().getPos());
		break;
	case SCENE_FARM:
		setWeatherType(mWeatherType);
		break;
	}
}
//-----------------------------------------------------------------------
bool EQGameSceneMgr::getDayNight()
{
	return AmbientMgr::getSingleton().getDayNight();
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::changeDayNight(bool daynight, bool immediately)
{
	AmbientMgr::getSingleton().changeDayNight(daynight, immediately);

	GameEventMgr::getSingleton().fireEvent("CHANGE_DAYNIGHT");
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::switchDayNight(bool immediately)
{
	AmbientMgr::getSingleton().switchDayNight(immediately);

	GameEventMgr::getSingleton().fireEvent("CHANGE_DAYNIGHT");
}
//-----------------------------------------------------------------------
bool EQGameSceneMgr::inMyHouse()
{
	if(SCENE_HOUSE == mSceneType)
	{	
		PlayerHouse* house = getHouse();

		return sObjMgr.isMainPlayer(house->getOwner());
	}
	else
	{
		return false;
	}
}
//-----------------------------------------------------------------------
PlayerHouse* EQGameSceneMgr::getHouse()
{
	assert(mSceneType==SCENE_HOUSE);

	return dynamic_cast<PlayerHouse*>(mGameScene);
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::startLoading()
{
	mLoadingBegin = GetTickCount();
	mIsLoading = true;

	luabind::call_function<void>(sLuaMgr.getL(), "MainUI_startLoading");
	CursorMgr::getSingleton().setVisible(false);
	CursorMgr::getSingleton().setCurTarget(0);
	CursorMgr::getSingleton().setCurTargetType(0);

	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Goto_OnBeginChangeScene");

	//loading界面得立即显示
	EQOgreSys::getSingleton().renderOneFrame();

	//资源回收
	PrepareRecycleResource();
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::endLoading()
{
	//资源回收
	RecycleResource();

	luabind::call_function<void>(sLuaMgr.getL(), "MainUI_endLoading");
	CursorMgr::getSingleton().setVisible(true);

	mIsLoading = false;
	luabind::call_function<void>(sLuaMgr.getL(), "GameScene_endLoading");

	releaseMemory(NULL);
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::setWeatherType(eWeather weather)
{
	if(weather == mWeatherType)
		return;

	mWeatherType = weather;

	if(mWeatherParticle)
	{
		ParticleMgr::getSingleton().removeParticle(mWeatherParticle);
		mWeatherParticle = NULL;
	}

	if(mWeatherType != WEATHER_NONE)
	{
		mWeatherParticle = ParticleMgr::getSingleton().createParticle(mWeatherType, "weather");
	}
}
//-----------------------------------------------------------------------
void EQGameSceneMgr::setWeatherPos(const Ogre::Vector3& pos)
{
	if(mWeatherParticle)
		mWeatherParticle->setTransform( TransformInfo(pos) );
}
//-----------------------------------------------------------------------








//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
std::vector<std::pair<stime, bool>> vTime;
void FindNearestInterval(int& interval, bool& daynight)
{
	stime now = Timekeeper::getSingleton().getTimeNow();

	//解析`
	if(vTime.size() == 0)
	{
		//同步年月日
		stime time;
		memcpy(&time, &now, sizeof(stime));

		std::vector<std::string> strArr; 
		strArr = Ogre::StringUtil::split( sVariableSystem.GetAs_String("DayNightParam"), " \r\n\t()," );
		for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
		{
			sscanf_s((*it).c_str(), "%i:%i:%i", &time.hour, &time.minute, &time.second);
			it++;
			vTime.push_back(std::make_pair(time, Ogre::StringConverter::parseBool(*it)));
		}
	}

	//寻找离现在最近的时间段
	bool found=false;
	for(uint i=0; i<vTime.size(); i++)
	{
		interval = sTimekeeper.calculElapsedSecs(now, vTime[i].first);
		if(interval > 0)
		{
			found = true; 
			daynight = vTime[i].second;
			break;
		}
	}

	//没找到就是隔天的第一个时间段
	if(!found)
	{
		interval = sTimekeeper.calculElapsedSecs(now, vTime[0].first) + 3600*24;
		daynight = vTime[0].second;
	}
}

void DayNightTimerMode1(Timer *pTimer, unsigned long param1, unsigned long param2)
{
	bool daynight = !AmbientMgr::getSingleton().getDayNight();
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DayNightTimer", daynight);
	if(sFitMgr.isFiting())
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DayNightTimer", daynight);
	else
		if (!luabind::call_function<bool>(sLuaMgr.getL(), "CreatePlayer_getFirstInflag"))
		{
			EQGameSceneMgr::getSingleton().changeDayNight(daynight);
		}
		else
		{
			if ( !sEQGameSceneMgr.inMyHouse())
			{
				EQGameSceneMgr::getSingleton().changeDayNight(daynight);
			}
		}
}

void DayNightTimerMode2(Timer *pTimer, unsigned long param1, unsigned long param2)
{
	bool daynight = (bool)(param1!=0);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DayNightTimer", daynight);
	if(sFitMgr.isFiting())
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DayNightTimer", daynight);
	else
	{
		if (!luabind::call_function<bool>(sLuaMgr.getL(), "CreatePlayer_getFirstInflag"))
		{	//加个容错处理
			if(EQGameSceneMgr::getSingleton().hasScene())
				EQGameSceneMgr::getSingleton().changeDayNight(daynight);
			else
				AmbientMgr::getSingleton().changeDayNight(daynight, true);
		}
		else
		{
			if (!sEQGameSceneMgr.inMyHouse())
			{
				if(EQGameSceneMgr::getSingleton().hasScene())
					EQGameSceneMgr::getSingleton().changeDayNight(daynight);
				else
					AmbientMgr::getSingleton().changeDayNight(daynight, true);
			}
		}
	}

	int interval;
	FindNearestInterval(interval, daynight);

	Timer *timer = createTimer(interval * 1000, 1, ulong(daynight));
	addTimerListener(timer, TE_TIMER, DayNightTimerMode2);
	startTimer(timer);
}

void EQGameSceneMgr::initDayNightTimer()
{
	int mode = sVariableSystem.GetAs_Int("DayNightMode");
	if(mode == 1)
	{
		Timer *pTimer = createTimer(sVariableSystem.GetAs_Int("DayNightParam")*1000, 0);
		addTimerListener(pTimer, TE_TIMER, DayNightTimerMode1);
		startTimer(pTimer);
	}
	else if(mode == 2)
	{
		int interval;
		bool daynight;
		FindNearestInterval(interval, daynight);

		//登录时是最近时间段的相反.
		AmbientMgr::getSingleton().changeDayNight(!daynight, true);
		if(sStateMgr.inMainGame())
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DayNightTimer", !daynight);
		GameEventMgr::getSingleton().fireEvent("CHANGE_DAYNIGHT");

		Timer *timer = createTimer(interval*1000, 1, ulong(daynight));
		addTimerListener(timer, TE_TIMER, DayNightTimerMode2);
		startTimer(timer);
	}
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------