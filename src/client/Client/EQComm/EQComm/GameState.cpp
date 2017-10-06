#include "EQCommPCH.h"
#include "GameState.h"
#include "MainGame.h"
#include "EQLogin.h"

//-------------------------------------------------------------
template<> GameStateMgr* Ogre::Singleton<GameStateMgr>::ms_Singleton = 0;
//--------------------------------------------------------------

void GameStateMgr::init()
{
	for(int i=0; i < GS_COUNT; i++)
	{
		mState[i] = NULL;
	}
	mNextState = mCurState = GS_LOGIN;
	init(mCurState);
}

//-------------------------------------------------------------
void GameStateMgr::_doStateChg(GameState* pGs)
{
	if(mNextState == mCurState)
		return;

	if(mState[mNextState] == NULL)
	{
		switch(mNextState)
		{
		case GS_LOGIN:
			mState[mNextState] = NULL;
			break;
		case GS_MAINGAME:
			mState[mNextState] = &sMainGame;
			break;
		case GS_FunnyFarm:
			mState[mNextState] = pGs;//&sFunnyFarm;
			break;
		}
	}

	cleanup(mCurState);
	init(mNextState);

	mCurState = mNextState;
}
//---------------------------------------------------------------
void GameStateMgr::update()
{
	update(mCurState);
}
//---------------------------------------------------------------
void GameStateMgr::setState(GameStateDef nextState,GameState* pGs)
{
	if(nextState == mNextState
		|| nextState == mCurState)
		return;

	mNextState = nextState;
	_doStateChg(pGs);
}
//------------------------------------------------------------------
void GameStateMgr::cleanup()
{
	cleanup(mCurState);
}

void GameStateMgr::init(int state)
{
	if(NULL == mState[state])
		return;

	mState[state]->init();

}

void GameStateMgr::cleanup(int state)
{
	if(NULL == mState[state])
		return;

	mState[state]->cleanup();

}

void GameStateMgr::update(int state)
{
	if(NULL == mState[state])
		return;

	mState[state]->update();

}
