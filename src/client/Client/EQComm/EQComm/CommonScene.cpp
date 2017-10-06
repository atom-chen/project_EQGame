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

	//���⻹ԭ��Ӱ
	EQOgreSys::getSingleton().setShadow(EQOgreSys::getSingleton().getConfigOption()->getShadow());

	//��ʼ������ƿ���
// 	if(AmbientMgr::getSingleton().getDayNight())
// 		ZoneLightMgr::getSingleton().turnOffAllLights();
// 	else
// 		ZoneLightMgr::getSingleton().turnOnAllLights();
	EQSceneManager::getSingleton().turnAllLight(!AmbientMgr::getSingleton().getDayNight());

	//������Ҫ�Զ����ص�
	AmbientMgr::getSingleton().setAutoTurnOnOffLight(true);
}