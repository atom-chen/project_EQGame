#pragma once
#include "eqgamescene.h"

class FarmSceneListener : public EQGameScene::Listener
{
	virtual void postCreateScene(EQGameScene* s);
};

class FunnyFarmScene :
	public EQGameScene
{
public:
	FunnyFarmScene(int id);
	~FunnyFarmScene(void);
};
