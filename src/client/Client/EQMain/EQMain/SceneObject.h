/******************************************************************************
* 作者: Y3
* 时间: 2009-07-10
* 描述: 场景物件的基类
******************************************************************************/
#ifndef SceneObject_H
#define SceneObject_H

#include "EQMainPrerequisites.h"

#include "PickObject.h"

/** 场景物件的基类. 
@remarks 目前特指与房屋场景无关的对象. 
*/
class SceneObject : public PickObject
{
public:
	SceneObject() : mId(0) {}
	virtual ~SceneObject() {}

	uint getId() {return mId;}
	
	/// 请不要轻易修改这个值
	void setId(uint id) {mId = id;}

public:
	virtual void showBoundingBox(bool show){}

	virtual void setPosition(const Ogre::Vector3& position){}
	virtual const Ogre::Vector3& getPosition() const { return Ogre::Vector3::ZERO; }
	virtual const Ogre::Vector3 & _getDerivedPosition(void) const { return Ogre::Vector3::ZERO; }

	virtual void yaw(const Ogre::Radian& angle){}
	virtual void setOrientation(const Ogre::Quaternion& qua) {}
	virtual const Ogre::Quaternion& getOrientation() const {return Ogre::Quaternion::IDENTITY;}

	virtual void setScale(const Ogre::Vector3& scale) {}
	virtual const Ogre::Vector3 & getScale(void) const {return Ogre::Vector3::UNIT_SCALE;}

protected:
	uint	mId;
};

#endif