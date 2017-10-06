#include "EQCommPCH.h"
#include "MainGame.h"
#include "EQGameMgr.h"
#include "MainPlayer.h"
#include "EQCamera.h"
#include "FitmentEvent.h"
#include "EQOgreSys.h"
#include "Variable.h"
//EQMain
#include "OgreMaxSceneLoader.h"
#include "SceneManager.h"
#include "HouseResManager.h"
#include "EQGameSceneMgr.h"
#include "DragItem.h"

//----------------------------------------------------------
MainGame::MainGame(void)
{
}
//---------------------------------------------------------
MainGame::~MainGame(void)
{
}
//----------------------------------------------------------
void MainGame::init()
{
	//主角色及场景彀
	MainPlayer::getSingleton().createAvatar(1);
	sDragItem.init();
}
//----------------------------------------------------------
void MainGame::update()
{
	MainPlayer::getSingleton().update();
}
//----------------------------------------------------------
void MainGame::cleanup()
{
	//EQGameSceneMgr::getSingleton().changeScene(SCENE_NONE, 0);
}