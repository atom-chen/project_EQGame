/******************************************************************************
* 作者: Y3
* 时间: 2009-07-13
* 描述: 可选取物件的代理类
******************************************************************************/
#ifndef PickObjectProxy_H
#define PickObjectProxy_H

#include "EQMainPrerequisites.h"

#include "PickObject.h"

class PickObjectProxy : public Ogre::UserDefinedObject
{
public:
	PickObjectProxy();
	PickObjectProxy(PickObject* obj);
	virtual ~PickObjectProxy();

	/** @copydoc Ogre::UserDefinedObject::getTypeName */
	virtual const std::string& getTypeName(void) const;

	void setObject(PickObject* obj) {mObject = obj;}

	PickObject* getObject() {return mObject;}

	/** The type name of this user defined object.
	@remarks
	Declare as public so we can easy to identifying a given
	user defined object is an instance of this classes.
	*/
	static const std::string msTypeName;

protected:
	PickObject*			mObject;



public:
	static PickObject* pickUpObject(Ogre::MovableObject* movable);
};

#endif
