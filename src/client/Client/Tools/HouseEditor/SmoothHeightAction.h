/******************************************************************************
* 作者: Y3
* 时间: 2005-6-4
* 描述: 来自TD
******************************************************************************/
#ifndef __SmoothHeightAction_H__
#define __SmoothHeightAction_H__

#include "TimeBasedHeightAction.h"


class JunctionSelection;

class SmoothHeightAction : public TimeBasedHeightAction
{
public:
	static SmoothHeightAction* create();
public:
	SmoothHeightAction();

	const std::string& getName(void) const;

protected:
	Real mAverageHeight;

	void computeAverageHeight(void);

	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	Real _computeHeight(const JunctionSelection::Junction& junction, Real seconds);
};


#endif // __SmoothHeightAction_H__
