#include "EQFarmPCH.h"

#include "ScriptFunction.h"
#include "EQGameSceneMgr.h"
#include "FunnyFarm.h"

//------------------------------------------------------------------------------
bool loadFarmScence()
{
	sStateMgr.setState(GS_FunnyFarm,&sFunnyFarm);
	return true;
}

//--------------------------------------------------------------------------------
bool logoutFarm()
{
	//sStateMgr.setState(GS_MAINGAME);
	/*sEQGameSceneMgr.reqChangeScene(EQGameSceneMgr::SCENE_COMMON,MainPlayer::getSingleton().getAccount());*/
	//sEQGameSceneMgr.reqReseeScene();
	sEQGameSceneMgr.reqLeaveFarm();
	return true;
}
//-------------------------------------------------------------------------------
bool setFarmCurState(int sta)
{
	sFunnyFarm.setFarmCurState(sta);
	return true;
}
//-------------------------------------------------------------------------------
bool setFarmSeedingCurSta(int sta, std::string imageset, std::string icon)
{
	sFunnyFarm.setSeedingCur(sta,imageset,icon);
	return true;
}