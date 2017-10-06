/******************************************************************************
* 作者: Mike
* 时间: 2010-7-05
* 描述: 输入事件
******************************************************************************/
#ifndef INPUTMGR_H
#define INPUTMGR_H
#include "EQMainPrerequisites.h"
#include <vector>
#include "InputDef.h"

typedef std::vector<EQInputEventListener*> ListenerList;
typedef std::vector<EQInputEventListener*>::iterator ListenerIter;
/*
*/
class InputEvtMgr
{
public:
	InputEvtMgr(void)
	{
		mLmousedownElapsed = 0;
		mLMousedown = false;
		mLMouseDraged = false;
		mMidmousedown = false;
		mRBtmdown=false;
		mNumListener = 0;
	}

	virtual ~InputEvtMgr(void){}

	static InputEvtMgr& getSingleton(){static InputEvtMgr mgr; return mgr;}

public:
	void update(float timElapsed);
	void resetFlag();
	void lostFocus();

	//void	zChg(float zoom);
	//void	mouseMoved(bool uiDone, POINT mouse);
	//
	//void	mmouseUp(){mMidmousedown = false;}
	//void	lmouseUp(POINT p);
	void 	rmouseUp(POINT mouse);
	//void 	lmouseDown(POINT p);
	//void 	mmouseDown(){mMidmousedown=true;}
	//void	mmouseMove(POINT delta, POINT mouse);

	void 	LButtonDown(UINT flags, POINT point);
	void 	LButtonUp	(UINT flags, POINT point);
	void 	RButtonDown(UINT flags, POINT point);
	void 	RButtonUp	(UINT flags, POINT point);
	void 	MButtonDown(UINT flags, POINT point);
	void 	MButtonUp	(UINT flags, POINT point);
	void 	MouseMove	(UINT flags, POINT delta, POINT point);
	void 	MouseWheel	(UINT flags, POINT point, short zDelta);
	void 	KeyDown	(UINT flags, UINT nChar, UINT nRepCnt=0);
	void 	KeyUp		(UINT flags, UINT nChar, UINT nRepCnt=0);

public:
	void clearEvents();
	void addListener(EQInputEventListener* lis)
	{
		mListener.push_back(lis);
		mNumListener++;
		for (int i=0; i<mNumListener; i++)
		{
			for (int j=i; j<mNumListener; j++)
			{
				if (mListener[i]->getPriority() < mListener[j]->getPriority())
					std::swap(mListener[i],mListener[j]);
			}
		}
	}

private:
	bool 	_intersects(POINT mouse, Ogre::Vector3& out);
//	void	_pickMove(POINT mouse);
//	bool	_pickClick(POINT mouse);

public:
	POINT		mMouseLastPoint;		//坐标保存
	bool		mMidmousedown			//中键按下
				,mLMousedown		
				,mLMouseDraged
				,mRBtmdown;

	float		mLmousedownElapsed;

protected:
	ListenerList mListener;
	int			 mNumListener;

};
#define sInputMgr InputEvtMgr::getSingleton()
#endif