#include "EQCommPCH.h"
#include "InputMgr.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "EQWndProc.h"
#include "GameState.h"
#include "CursorMgr.h"
#include "FitmentEvent.h"

//-----------------------------------------------------------------------
void InputEvtMgr::clearEvents()
{
	mListener.clear();
}
//-----------------------------------------------------------------------
void InputEvtMgr::LButtonDown(UINT flags, POINT point)
{
	//if (sCursorMgr.getState() == CursorMgr::CS_SPLIT_ITEM)
	//	sCursorMgr.backtoLastState();
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->LButtonDown(flags,point);
	}
	mMouseLastPoint = point;
	mLMousedown = true;
}
//-----------------------------------------------------------------------
void InputEvtMgr::LButtonUp(UINT flags, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->LButtonUp(flags,point);
	}
	resetFlag();
}
//-----------------------------------------------------------------------
void InputEvtMgr::RButtonDown(UINT flags, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->RButtonDown(flags,point);
	}
	mRBtmdown=true;
}
//-----------------------------------------------------------------------
void InputEvtMgr::RButtonUp(UINT flags, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->RButtonUp(flags,point);
	}
	resetFlag();
}
//-----------------------------------------------------------------------
void InputEvtMgr::MButtonDown(UINT flags, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->MButtonDown(flags,point);
	}
	mMidmousedown = true;
}
//-----------------------------------------------------------------------
void InputEvtMgr::MButtonUp(UINT flags, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->MButtonUp(flags,point);
	}
	resetFlag();
}
//-----------------------------------------------------------------------
void InputEvtMgr::MouseMove(UINT flags, POINT delta, POINT point)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->MouseMove(flags, delta, point);
	}
}
//-----------------------------------------------------------------------
void InputEvtMgr::MouseWheel(UINT flags, POINT point, short zDelta)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->MouseWheel(flags,point,zDelta);
	}
}
//-----------------------------------------------------------------------
void InputEvtMgr::KeyDown(UINT flags, UINT nChar, UINT nRepCnt)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->KeyDown(flags,nChar,nRepCnt);
	}
}
//-----------------------------------------------------------------------
void InputEvtMgr::KeyUp(UINT flags, UINT nChar, UINT nRepCnt)
{
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->KeyUp(flags,nChar,nRepCnt);
	}
}
//-----------------------------------------------------------------------


//------------------------------------------------------------------------------
void InputEvtMgr::update(float timElapsed)
{
	if(mLMousedown)
	{
		mLmousedownElapsed += timElapsed;
#define DRAG_TIMER 0.5
		if( mLmousedownElapsed >= DRAG_TIMER /*&& !sFitMgr.isFiting()*/
			/*&& !sStateMgr.isFunnyFarm()*/)
		{
			mLMouseDraged = true;
			//sCursorMgr.setState(CursorMgr::CS_LBDRAG_SCENE);
		}
	}
	else
	{
		mLmousedownElapsed = 0;
		mLMouseDraged = false;
	}

	//
	for (int i = 0; i < mNumListener; i++)
	{
		mListener[i]->Update(timElapsed);
	}
}
//------------------------------------------------------------------------------
void InputEvtMgr::resetFlag()
{
	mLmousedownElapsed = 0;
	mLMousedown = false;
	mLMouseDraged = false;
	mMidmousedown = false;
	mRBtmdown=false;
}
//------------------------------------------------------------------------------
void InputEvtMgr::lostFocus()
{
	resetFlag();
	if (!sStateMgr.isFunnyFarm())
	{
		sCursorMgr.setState(CursorMgr::CS_NORMAL);
	}
	CEGUI::MouseCursor::getSingleton().hide();
}
