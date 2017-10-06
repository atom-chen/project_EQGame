/******************************************************************************
* 作者: Y3
* 时间: 2005-7-2
* 描述: 来自TD
******************************************************************************/
#ifndef __SetTerrainHeightAction_H__
#define __SetTerrainHeightAction_H__

#include "HeightAction.h"
//#include "HitInfo.h"


/** 根据ui界面中设置的数值来设置地形的高度
*/
class SetTerrainHeightAction : public HeightAction
{
public:
	static SetTerrainHeightAction* create();

public:
	SetTerrainHeightAction();
	~SetTerrainHeightAction();

	const std::string& getName(void) const;

	void setParameter(const std::string& name, const std::string& value);
	std::string getParameter(const std::string& name) const;

protected:

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	void _doModify(const Ogre::Vector2& pt);

protected:

	int mTerrainHeight;
	int mTerrainHeightIncrement;

	JunctionSelection* mModifiedJunctions;
};



#endif // __SetTerrainHeightAction_H__
