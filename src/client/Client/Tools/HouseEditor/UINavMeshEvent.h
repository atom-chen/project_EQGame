/******************************************************************************
* 作者: Y3
* 时间: 2010-10-22
* 描述: 导航网格对应的事件
******************************************************************************/
#ifndef UINavMeshEvent_H
#define UINavMeshEvent_H

#include "MfcInputEvent.h"

#include "SearchSystem/NavSearchSystem.h"


class TestNavMeshPathEvent : public MfcInputEvent
{
public:
	explicit TestNavMeshPathEvent();
	~TestNavMeshPathEvent();

	static TestNavMeshPathEvent * create();

public:
	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

	void LButtonDown( UINT nFlags, CPoint point, bool & processed );

	void LButtonUp( UINT nFlags, CPoint point, bool & processed );

	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );

protected:
	void lineStart(CPoint pt);
	void lineMove(CPoint pt);
	void lineDraw();
	void _lineDestroy(bool withNode = true);
	bool lineEnd();

	void exce( bool bAdded );

	void clearPath();
	void searchPath();
	void drawPath();
	void _destroyManualObject( Ogre::ManualObject * obj );

protected:
	bool						mLining;
	bool						mPicking;
	Ogre::Vector3				mStartLinePos;
	Ogre::Vector3				mEndLinePos;
	Ogre::SceneNode *			mLineObjNode;
	Ogre::ManualObject *		mLineObj;


	Ogre::ManualObject *	mPathObj;	// 路径图形表示

	std::vector<SearchSystem::Vector2f>	mPath;
};

#endif