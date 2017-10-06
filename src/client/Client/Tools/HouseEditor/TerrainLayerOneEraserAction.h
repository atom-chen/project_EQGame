/******************************************************************************
* 作者: Y3
* 时间: 2005-7-2
* 描述: 来自TD
******************************************************************************/
#ifndef __TerrainLayerOneEraserAction_H__
#define __TerrainLayerOneEraserAction_H__

#include "PaintAction.h"


class GridSelection;

class TerrainLayerOneEraserAction : public PaintAction
{
public:
	static TerrainLayerOneEraserAction* create();

public:
	TerrainLayerOneEraserAction();
	~TerrainLayerOneEraserAction();

	const std::string& getName(void) const;

	//  void setParameter(const std::string& name, const std::string& value);
	//   std::string getParameter(const std::string& name) const;

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:
	GridSelection* mCurrentGrids;
	GridSelection* mHintModified;
	GridSelection* mModifiedGrids;

	void _buildHitIndicator(const Ogre::Vector2& pt);
	void _doErase(GridSelection* modified);

	void _onActive(bool active);

	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);   

};



#endif // __TerrainLayerOneEraserAction_H__
