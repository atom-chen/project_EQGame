/******************************************************************************
* 作者: Y3
* 时间: 2005-6-4
* 描述: 来自TD
******************************************************************************/
#ifndef __RaiseHeightAction_H__
#define __RaiseHeightAction_H__

#include "TimeBasedHeightAction.h"

class JunctionSelection;

class RaiseHeightAction : public TimeBasedHeightAction
{
public:
	static RaiseHeightAction* create();

public:
	RaiseHeightAction();

	const std::string& getName(void) const;

protected:
	Real mAdjust;

	void _prepareUpdate(const JunctionSelection& selection, Real seconds);
	Real _computeHeight(const JunctionSelection::Junction& junction, Real seconds);
};



#endif // __RaiseHeightAction_H__
