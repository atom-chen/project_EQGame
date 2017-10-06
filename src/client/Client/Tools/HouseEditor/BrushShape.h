/******************************************************************************
* 作者: Y3
* 时间: 2005-5-31
* 描述: 笔刷形状. 来自TD
******************************************************************************/
#ifndef __BRUSHSHAPE_H__
#define __BRUSHSHAPE_H__



class Selection;

class BrushShape
{
public:
	virtual ~BrushShape() {}

	virtual const std::string& getType(void) const = 0;

	virtual void build(Selection& selection, Real xcentre, Real zcentre, int xsize, int zsize) = 0;
};


#endif // __BrushShape_H__
