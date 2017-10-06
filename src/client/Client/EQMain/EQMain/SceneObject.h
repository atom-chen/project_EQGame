/******************************************************************************
* ����: Y3
* ʱ��: 2009-07-10
* ����: ��������Ļ���
******************************************************************************/
#ifndef SceneObject_H
#define SceneObject_H

#include "EQMainPrerequisites.h"

#include "PickObject.h"

/** ��������Ļ���. 
@remarks Ŀǰ��ָ�뷿�ݳ����޹صĶ���. 
*/
class SceneObject : public PickObject
{
public:
	SceneObject() : mId(0) {}
	virtual ~SceneObject() {}

	uint getId() {return mId;}
	
	/// �벻Ҫ�����޸����ֵ
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