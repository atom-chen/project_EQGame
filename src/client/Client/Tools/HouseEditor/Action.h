/******************************************************************************
* 作者: Y3
* 时间: 2005-5-31
* 描述: 操作行为. 来自TD
******************************************************************************/
#ifndef __Action_H__
#define __Action_H__

#include "Manip.h"

class Action : public Manip
{
protected:
	bool mActived;
public:
	void setActive(bool active);

public:
	Action();
	~Action();

	virtual const std::string& getName(void) const = 0;

	virtual void setParameter(const std::string& name, const std::string& value);
	virtual std::string getParameter(const std::string& name) const;

public: //override
	bool isTDAction() { return true; }

protected:

	bool getActive(void) const
	{
		return mActived;
	}

protected:
	virtual void _onActive(bool active);
};


#endif // __Action_H__
