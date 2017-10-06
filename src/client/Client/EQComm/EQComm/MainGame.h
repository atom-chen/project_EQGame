/*******************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ����Ϸ����
********************************************************************/
#ifndef MainGame_H
#define MainGame_H

#include "GameState.h"
/**
	����Ϸ����
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