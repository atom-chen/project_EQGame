#include "HouseEditorPCH.h"
#include "Manip.h"
#include "Action.h"

#include "MfcInputEventMgr.h"
#include "GraphicsSystem.h"


class Manip::UpdateListener : public Ogre::RenderTargetListener
{
public:
	Manip* mParent;
	unsigned long mLastTime;
	CPoint point;

protected:
	bool adding;

public:
	UpdateListener(Manip* parent)
		: mParent(parent)
		, mLastTime(0)
		, adding(false)
	{
	}

	~UpdateListener()
	{
		disable();
	}

	void enable(void)
	{
		if ( !adding )
		{
			GraphicsSystem::getSingleton().getWindow()->addListener(this);
			adding = true;
		}
		mLastTime = Ogre::Root::getSingleton().getTimer()->getMicroseconds();
	}

	void disable(void)
	{
		if ( adding )
		{
			GraphicsSystem::getSingleton().getWindow()->removeListener(this);
			adding = false;
		}
	}

	void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
	{
		unsigned long now = Ogre::Root::getSingleton().getTimer()->getMicroseconds();
		unsigned long delta = now - mLastTime;
		mLastTime = now;
		mParent->onUpdate(point.x, point.y, delta / Ogre::Real(1000*1000));
	}


};


//////////////////////////////////////////////////////////////////////////

Manip::Manip()
    : MfcInputEvent(epTDAction)
	, mDecisionMode(false)
    , mDragStarted(false)
{
	mUpdateListener = new UpdateListener(this);
}

Manip::~Manip()
{
	delete mUpdateListener;
}

//////////////////////////////////////////////////////////////////////////

void Manip::onBegin(const Ogre::Vector2& pt)
{
    mDragStarted = true;
    mDragOrigin = mDragCurrent = pt;
    mDragDelta = Ogre::Vector2(0, 0);
    _onBegin(pt);
}

void Manip::onMotion(const Ogre::Vector2& pt)
{
    if (mDragStarted)
    {
        mDragDelta = pt - mDragCurrent;
        mDragCurrent = pt;
        _onDrag(pt);
    }
    else
        _onMove(pt);
}

void Manip::onEnd(const Ogre::Vector2& pt, bool canceled)
{
    _onEnd(pt, canceled);
    mDragStarted = false;
}

void Manip::onUpdate(const Ogre::Vector2& pt, Real seconds)
{
    _onUpdate(pt, seconds);
}

//////////////////////////////////////////////////////////////////////////

void Manip::onBegin(Real x, Real y)
{
    onBegin(Ogre::Vector2(x, y));
}

void Manip::onMotion(Real x, Real y)
{
    onMotion(Ogre::Vector2(x, y));
}

void Manip::onEnd(Real x, Real y, bool canceled)
{
    onEnd(Ogre::Vector2(x, y), canceled);
}

void Manip::onUpdate(Real x, Real y, Real seconds)
{
    onUpdate(Ogre::Vector2(x, y), seconds);
}

//////////////////////////////////////////////////////////////////////////

void
Manip::_onBegin(const Ogre::Vector2& pt)
{
}

void
Manip::_onMove(const Ogre::Vector2& pt)
{
}

void
Manip::_onDrag(const Ogre::Vector2& pt)
{
}

void
Manip::_onClick(const Ogre::Vector2& pt)
{
}

void
Manip::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
}

void
Manip::_onUpdate(const Ogre::Vector2& pt, Real seconds)
{
}



//--------------------------------------------------------------------------
void Manip::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	// processed = true; //一般不处理这个事件

	Action *action = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( !action )
		return;

	mUpdateListener->point = point;
	action->onMotion(point.x, point.y);

}
//--------------------------------------------------------------------------
void Manip::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	//
	Action *action = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( !action )
		return;

	mUpdateListener->point = point;
	mUpdateListener->enable();
	action->onBegin(point.x, point.y);
}
//--------------------------------------------------------------------------
void Manip::LButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	//
	Action *action = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( !action )
		return;

	mUpdateListener->point = point;
	action->onEnd(point.x, point.y, false);
	mUpdateListener->disable();
}

