/*******************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 主游戏管理
********************************************************************/
#ifndef MainGame_H
#define MainGame_H

#include "GameState.h"
/**
	主游戏管理
*/
class MainGame : public GameState
{
public:
	MainGame(void);
	virtual ~MainGame(void);

	static MainGame& getSingleton()
	{
		static MainGame singleton;
		return singleton;
	}

	virtual void init();
	virtual void update();
	virtual void cleanup();

};
#define sMainGame MainGame::getSingleton()
#endif