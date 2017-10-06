/******************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏状态机管理
*       
******************************************************************/
#ifndef GameState_H
#define GameState_H

#include "EQMainPrerequisites.h"
#include "StateDef.h"

class EQLogin;
class MainGame;
/**
	游戏状态基类
*/
class GameState
{
public:
	virtual void init(){}
	//
	virtual void cleanup(){}
	//
	virtual void update(){}

	GameState(){}
	virtual ~GameState(){}
};
/**
	游戏状态管理器单件体
*/
class GameStateMgr : public Ogre::Singleton<GameStateMgr>
{
public:
	GameStateMgr(void) {}
	virtual ~GameStateMgr(void) {}

	void init();
	void cleanup();
	void update();

	void init(int state);
	void cleanup(int state);
	void update(int state);


	/**
		设置游戏状态，缓冲在update()里执行
	*/
	void setState(GameStateDef nextState,GameState* pGs=NULL);
	GameStateDef getState(){return mCurState;}

	bool gaming(){return mCurState != GS_LOGIN;}
	bool isFunnyFarm(){return mCurState == GS_FunnyFarm;}
	bool inMainGame(){return mCurState == GS_MAINGAME;}

	MainGame* getMainGame()
	{
		return mCurState == GS_MAINGAME 
			?	(MainGame*)mState[mCurState] :	 NULL;
	}

private:
	GameStateDef	mCurState,mNextState;
	GameState*		mState[GS_COUNT];
	/**
		触发执行状态切换
	*/
	void _doStateChg(GameState* pGs=NULL);
};
#define  sStateMgr GameStateMgr::getSingleton()
#endif