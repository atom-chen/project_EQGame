/******************************************************************************
* ����: Y3
* ʱ��: 2005-5-31
* ����: ��ˢ��״ʵ����. ����TD
******************************************************************************/
#ifndef __BrushShapes_H__
#define __BrushShapes_H__

#include "BrushShape.h"

class Selection;

class BoxBrushShape : public BrushShape
{
public:
	virtual const std::string& getType(void) const;
	virtual void build(Selection& selection, Real xcentre, Real zcentre, int xsize, int zsize);
};

class EllipseBrushShape : public BrushShape
{
public:
	virtual const std::string& getType(void) const;
	virtual void build(Selection& selection, Real xcentre, Real zcentre, int xsize, int zsize);
};


#endif // __BrushShapes_H__
