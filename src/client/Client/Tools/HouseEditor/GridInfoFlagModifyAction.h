/******************************************************************************
* 作者: Y3
* 时间: 2005-7-2
* 描述: 来自TD
******************************************************************************/
#ifndef __GridInfoFlagModifyAction_H__
#define __GridInfoFlagModifyAction_H__

#include "Action.h"


class GridSelection;

class GridInfoFlagModifyAction : public Action
{
public:
	GridInfoFlagModifyAction();

	~GridInfoFlagModifyAction();

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	void _modifyFlag(const Ogre::Vector2& pt);
	void _doFinish(GridSelection* origin, bool canceled);

protected:

	unsigned int mModifiedFlag;
	std::string mHitIndicatorName;
};

//////////////////////////////////////////////////////////////////////////
class FlipDiagonalAction : public GridInfoFlagModifyAction
{
public:
	static FlipDiagonalAction* create();

public:
	FlipDiagonalAction();

	const std::string& getName(void) const;
};

//////////////////////////////////////////////////////////////////////////
/** 我实在不知道这个有少用途 --Y3
*/
class DisableDecalAction : public GridInfoFlagModifyAction
{
public:
	DisableDecalAction();

	const std::string& getName(void) const;
};

#endif // __GridInfoFlagModifyAction_H__