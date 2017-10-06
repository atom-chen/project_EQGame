/******************************************************************************
* 作者: Y3
* 时间: 2005-6-4
* 描述: 来自TD
******************************************************************************/
#ifndef __TimeBasedHeightAction_H__
#define __TimeBasedHeightAction_H__

#include "HeightAction.h"
#include "TerrainSelections.h"


class JunctionSelection;

class TimeBasedHeightAction : public HeightAction
{
public:
	TimeBasedHeightAction();
	~TimeBasedHeightAction();

protected:
	JunctionSelection* mModifiedJunctions;

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onUpdate(const Ogre::Vector2& pt, Real seconds);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	virtual void _prepareUpdate(const JunctionSelection& selection, Real seconds);
	virtual Real _computeHeight(const JunctionSelection::Junction& junction, Real seconds) = 0;
};


#endif // __TimeBasedHeightAction_H__
