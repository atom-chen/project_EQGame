/******************************************************************************
* 作者: Y3
* 时间: 2009-08-19
* 描述: 场景物件操作
******************************************************************************/
#ifndef SceneObjectOperator_H
#define SceneObjectOperator_H

#include "Operator.h"

class SceneEntityObject;
class SceneObject;

class CreateSceneEntityObjectOperator : public Operator
{
public:
	CreateSceneEntityObjectOperator();
	~CreateSceneEntityObjectOperator() {}

	void add(SceneEntityObject* obj, bool isCreate);

public:
	const std::string& getGroupName(void) const;
	std::string getDescription(void) const { return ""; }
	std::string getHelp(void) const { return ""; }
	void undo(void);
	void redo(void);

protected:
	void apply(bool reserve);

protected:
	bool				mIsCreate;
	uint				mSceneObjectId;
	uint				mEntityObjId;
	std::string			mMesh;
	std::string			mMaterial;
	Ogre::Vector3		mPos;
	Ogre::Quaternion	mRot;
	Ogre::Vector3		mScale;
	uint				mLightId;
	uint				mParticleID;
	int					mCutDistanceLevel;
};




class SceneObjectTransformOperator : public Operator
{
public:
	SceneObjectTransformOperator();
	~SceneObjectTransformOperator(){}

	void add(SceneObject* obj, const Ogre::Vector3& orgPos, const Ogre::Quaternion& orgRot, const Ogre::Vector3& orgScale);

public:
	const std::string& getGroupName(void) const;
	std::string getDescription(void) const { return ""; }
	std::string getHelp(void) const { return ""; }
	void undo(void);
	void redo(void);

protected:
	void apply(bool reserve);

protected:
	uint				mSceneObjectId;
	Ogre::Vector3		mPos;
	Ogre::Quaternion	mRot;
	Ogre::Vector3		mScale;
	Ogre::Vector3		mOrgPos;
	Ogre::Quaternion	mOrgRot;
	Ogre::Vector3		mOrgScale;

};


#endif

