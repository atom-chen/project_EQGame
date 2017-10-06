/******************************************************************************
* ����: lf
* ʱ��: 2010-9-3
* ����: ��������
******************************************************************************/
#ifndef CommonScene_H
#define CommonScene_H

#include "EQMainPrerequisites.h"
#include "EQGameScene.h"

class CommonSceneListener : public EQGameScene::Listener
{
	virtual void postCreateScene(EQGameScene* s);
};

class CommonScene : public EQGameScene
{
	friend CommonSceneListener;
public:
	CommonScene(int id);
	~CommonScene();
};

#endif
