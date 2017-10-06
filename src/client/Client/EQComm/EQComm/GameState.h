/******************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��Ϸ״̬������
*       
******************************************************************/
#ifndef GameState_H
#define GameState_H

#include "EQMainPrerequisites.h"
#include "StateDef.h"

class EQLogin;
class MainGame;
/**
	��Ϸ״̬����
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
	��Ϸ״̬������������
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
		������Ϸ״̬��������update()��ִ��
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
		����ִ��״̬�л�
	*/
	void _doStateChg(GameState* pGs=NULL);
};
#define  sStateMgr GameStateMgr::getSingleton()
#endif