/******************************************************************************
* 作者: Y3
* 时间: 2009-12-19
* 描述: 网格窗口对应的事件
******************************************************************************/
#ifndef UIGridEvent_H
#define UIGridEvent_H


#include "MfcInputEvent.h"

template <class T> class CompareVector3Template;


class HouseSceneComponent;
class HouseSceneGrid;

/** 创建删除网格事件
*/
class CreateGridEvent : public MfcInputEvent
{
public:
	explicit CreateGridEvent();
	~CreateGridEvent();

	static CreateGridEvent * create();
private:
	bool exec(CPoint point);

public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );

	void RButtonDown( UINT nFlags, CPoint point, bool & processed );

	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );
};


class GridLineEvent : public MfcInputEvent
{
public:
	explicit GridLineEvent( int pri, bool bMoreOp );
	virtual ~GridLineEvent();
protected:
	void lineStart(CPoint pt);
	void lineMove(CPoint pt);
	void lineDraw();
	void _lineDestroy(bool withNode = true);
	bool lineEnd();
protected:
	virtual void exce( bool bAdded ) = 0;
public:
	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

	void LButtonDown( UINT nFlags, CPoint point, bool & processed );

	void LButtonUp( UINT nFlags, CPoint point, bool & processed );

	void RButtonDown( UINT nFlags, CPoint point, bool & processed );

	void RButtonUp( UINT nFlags, CPoint point, bool & processed );

	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );
protected:
	bool						mLining;

	// 如果mMoreOp 为true, 第一次点击确定起始grid, 第二次点击确定截止grid
	// 如果mMoreOp 为false, 只用一次点击就完成上述过程
	bool						mMoreOp;
	bool						mPicking;
	Ogre::Vector3				mStartLinePos;
	Ogre::Vector3				mEndLinePos;
	HouseSceneGrid *			mStartGrid;
	HouseSceneGrid *			mEndGrid;
protected:	// 图形相关
	Ogre::SceneNode *			mLineObjNode;
	Ogre::ManualObject *		mLineObj;
};


/** 创建删除墙体事件
*/
class WallGridEvent : public GridLineEvent
{
public:
	WallGridEvent();
	~WallGridEvent();

	static WallGridEvent * create();
protected:
	virtual void exce( bool bAdded );
public:
};



/** 增删梯子事件
*/
class CreateLinkGridEvent : public GridLineEvent
{
public:
	explicit CreateLinkGridEvent();
	~CreateLinkGridEvent();

	static CreateLinkGridEvent * create();
protected:
	virtual void exce( bool bAdded );
};




/** 浏览网格
*/
class BrowseGridEvent : public MfcInputEvent
{
public:
	BrowseGridEvent();
	~BrowseGridEvent();

	static BrowseGridEvent * create();
public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );

private:
	bool exec(CPoint point);
};


/** 编辑房间事件
*/
class EditRoomEvent : public MfcInputEvent
{
public:
	explicit EditRoomEvent();
	~EditRoomEvent();

	static EditRoomEvent * create();
private:
	bool exec(CPoint point);

public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
	void RButtonDown( UINT nFlags, CPoint point, bool & processed );
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );
	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:
};



/** 测试自动寻路事件
*/
class TestAutoFindPathEvent : public GridLineEvent
{
	typedef CompareVector3Template<int> TPosition;
	typedef std::vector<TPosition> TPosArray;
public:
	explicit TestAutoFindPathEvent();
	~TestAutoFindPathEvent();

	static TestAutoFindPathEvent * create();
protected:
	void clearPath();
	void searchPath();
	void drawPath();
	void _destroyManualObject( Ogre::ManualObject * obj );
protected:
	virtual void exce( bool bAdded );
protected: // 图形相关
	Ogre::ManualObject *	mPathObj;	// 路径图形表示
protected: 
	TPosArray				mPosArray;	// 路径数据	
};


#endif
