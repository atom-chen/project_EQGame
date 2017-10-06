/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-19
* ����: ���񴰿ڶ�Ӧ���¼�
******************************************************************************/
#ifndef UIGridEvent_H
#define UIGridEvent_H


#include "MfcInputEvent.h"

template <class T> class CompareVector3Template;


class HouseSceneComponent;
class HouseSceneGrid;

/** ����ɾ�������¼�
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

	// ���mMoreOp Ϊtrue, ��һ�ε��ȷ����ʼgrid, �ڶ��ε��ȷ����ֹgrid
	// ���mMoreOp Ϊfalse, ֻ��һ�ε���������������
	bool						mMoreOp;
	bool						mPicking;
	Ogre::Vector3				mStartLinePos;
	Ogre::Vector3				mEndLinePos;
	HouseSceneGrid *			mStartGrid;
	HouseSceneGrid *			mEndGrid;
protected:	// ͼ�����
	Ogre::SceneNode *			mLineObjNode;
	Ogre::ManualObject *		mLineObj;
};


/** ����ɾ��ǽ���¼�
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



/** ��ɾ�����¼�
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




/** �������
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


/** �༭�����¼�
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



/** �����Զ�Ѱ·�¼�
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
protected: // ͼ�����
	Ogre::ManualObject *	mPathObj;	// ·��ͼ�α�ʾ
protected: 
	TPosArray				mPosArray;	// ·������	
};


#endif
