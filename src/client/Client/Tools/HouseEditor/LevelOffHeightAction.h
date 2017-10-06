/******************************************************************************
* 作者: Y3
* 时间: 2005-7-2
* 描述: 来自TD
******************************************************************************/
#ifndef __LevelOffHeightAction_H__
#define __LevelOffHeightAction_H__

#include "HeightAction.h"
#include "TerrainSelections.h"


class JunctionSelection;

class LevelOffHeightAction : public HeightAction
{
public:
	static LevelOffHeightAction* create();

public:
	LevelOffHeightAction();
	~LevelOffHeightAction();

	const std::string& getName(void) const;

protected:
	JunctionSelection* mModifiedJunctions;

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	virtual void _doModify(const Ogre::Vector2& pt);
};


#endif // __LevelOffHeightAction_H__
