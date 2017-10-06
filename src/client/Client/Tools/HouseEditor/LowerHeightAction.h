/******************************************************************************
* 作者: Y3
* 时间: 2005-6-4
* 描述: 来自TD
******************************************************************************/
#ifndef __LowerHeightAction_H__
#define __LowerHeightAction_H__

#include "TimeBasedHeightAction.h"



class JunctionSelection;

class LowerHeightAction : public TimeBasedHeightAction
{
public:
	static LowerHeightAction* create();

public:
	LowerHeightAction();

	const std::string& getName(void) const;

protected:
	Real mAdjust;

	void _prepareUpdate(const JunctionSelection& selection, Real seconds);
	Real _computeHeight(const JunctionSelection::Junction& junction, Real seconds);
};



#endif // __LowerHeightAction_H__
