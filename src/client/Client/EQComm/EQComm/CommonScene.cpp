#include "EQCommPCH.h"

#include "CommonScene.h"
#include "LogicTable.h"
#include "SceneManager.h"
#include "MainPlayer.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "GameState.h"
#include "EQOgreSys.h"
#include "ConfigOption.h"
#include "AmbientMgr.h"

CommonScene::CommonScene(int id) : EQGameScene(id)
{
	addListener(new CommonSceneListener);
}

CommonScene::~CommonScene()
{
	removeListener();
}

void CommonSceneListener::postCreateScene(EQGameScene* s)
{
	sStateMgr.setState(GS_MAINGAME);

	//室外还原阴影
	EQOgreSys::getSingleton().setShadow(EQOgreSys::getSingleton().getConfigOption()->getShadow());

	//初始化室外灯开关
// 	if(AmbientMgr::getSingleton().getDayNight())
// 		ZoneLightMgr::getSingleton().turnOffAllLights();
// 	else
// 		ZoneLightMgr::getSingleton().turnOnAllLights();
	EQSceneManager::getSingleton().turnAllLight(!AmbientMgr::getSingleton().getDayNight());

	//室外需要自动开关灯
	AmbientMgr::getSingleton().setAutoTurnOnOffLight(true);
}