/******************************************************************************
* 作者: Y3
* 时间: 2005-5-31
* 描述: 来自TD
******************************************************************************/
#ifndef __SELECTION_H__
#define __SELECTION_H__



class Selection
{
protected:

public:
	// Accessor
	Selection() {}
	virtual ~Selection() {}

	virtual const std::string& getType(void) const = 0;
	virtual bool empty(void) const = 0;
	virtual void reset(void) = 0;
	virtual void apply(void) = 0;
	virtual void notifyModified(void) const = 0;
};


#endif // __Selection_H__
