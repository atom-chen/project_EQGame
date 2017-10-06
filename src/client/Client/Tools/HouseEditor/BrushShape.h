/******************************************************************************
* ����: Y3
* ʱ��: 2005-5-31
* ����: ��ˢ��״. ����TD
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
