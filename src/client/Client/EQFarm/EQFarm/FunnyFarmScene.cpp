#include "EQFarmPCH.h"
#include "FunnyFarmScene.h"
#include "GameState.h"
#include "FunnyFarm.h"
//------------------------------------------------------------------------------
FunnyFarmScene::FunnyFarmScene(int id)
	: EQGameScene(id)
{
	addListener(new FarmSceneListener);
}
//------------------------------------------------------------------------------
FunnyFarmScene::~FunnyFarmScene(void)
{
	removeListener();
}
//------------------------------------------------------------------------------
void FarmSceneListener::postCreateScene(EQGameScene* s)
{
	FunnyFarmScene* scene = dynamic_cast<FunnyFarmScene*>(s);
	assert(scene);

	sStateMgr.setState(GS_FunnyFarm,&sFunnyFarm);
}