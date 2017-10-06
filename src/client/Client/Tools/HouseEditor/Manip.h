/******************************************************************************
* 作者: Y3
* 时间: 2005-5-31
* 描述: 来自TD
******************************************************************************/
#ifndef __Manip_H__
#define __Manip_H__

#include "MfcInputEvent.h"


class Manip : public MfcInputEvent
{
protected:
	bool mDecisionMode;
	bool mDragStarted;
	Ogre::Vector2 mDragOrigin;
	Ogre::Vector2 mDragCurrent;
	Ogre::Vector2 mDragDelta;

public:
	Manip();
	virtual ~Manip();

public:
	void onBegin(const Ogre::Vector2& pt);
	void onMotion(const Ogre::Vector2& pt);
	void onEnd(const Ogre::Vector2& pt, bool canceled = false);
	void onUpdate(const Ogre::Vector2& pt, Real seconds);

	void onBegin(Real x, Real y);
	void onMotion(Real x, Real y);
	void onEnd(Real x, Real y, bool canceled = false);
	void onUpdate(Real x, Real y, Real seconds);

protected:
	virtual void _onBegin(const Ogre::Vector2& pt);
	virtual void _onMove(const Ogre::Vector2& pt);
	virtual void _onDrag(const Ogre::Vector2& pt);
	virtual void _onClick(const Ogre::Vector2& pt);
	virtual void _onEnd(const Ogre::Vector2& pt, bool canceled);
	virtual void _onUpdate(const Ogre::Vector2& pt, Real seconds);

protected:
	class UpdateListener;
	friend UpdateListener;

	UpdateListener* mUpdateListener;

public:	// override 这部分还有更多的事件需要处理, 以后慢慢增加
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
	void LButtonUp( UINT nFlags, CPoint point, bool & processed );
};


#endif // __Manip_H__
