/******************************************************************************
* 作者: Y3
* 时间: 2005-6-7
* 描述: 来自TD
******************************************************************************/
#ifndef __PaintAction_H__
#define __PaintAction_H__

#include "Action.h"

#define TexTileSize 64



class GridSelection;

struct PaintPixmap
{
	std::string textureName;
	Real left, top, right, bottom;

	PaintPixmap()
		: textureName()
		, left(0)
		, top(0)
		, right(1)
		, bottom(1)
	{
	}

	PaintPixmap( std::string name ) : textureName(name), left(0.0f), top(0.0f), bottom(1.0f), right(1.0f)
	{
	}
};

/** 画刷action的基类，负责处理设置所选纹理等基本操作
*/
class PaintAction : public Action
{
public:

	PaintAction();

	virtual ~PaintAction();

	void setPixmap(const std::string& textureName, Real left = 0, Real top = 0, Real right = 1, Real bottom = 1 );
	void setPixmap(const PaintPixmap &pixmap);
	int getPixmapId(void) const;
	bool getPixmapTransparent(void) const;

	void setPixmapOrientationType( int orientation )
	{
		mOrientationType = orientation;
	}

	int getPixmapOrientationType(void)
	{
		return mOrientationType;
	}

	void PaintAction::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:

	PaintPixmap mPixmap;
	mutable int mPixmapId;
	mutable bool mPixmapTransparent;
	mutable bool mPixmapInfoValid;
	/// orientation
	int mOrientationType;

protected:

	void updatePixmapInfo(void) const;

	void doFinish(GridSelection* origin, bool canceled);

};

#endif // __PaintAction_H__