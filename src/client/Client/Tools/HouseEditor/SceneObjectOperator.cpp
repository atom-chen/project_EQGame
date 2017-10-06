#include "HouseEditorPCH.h"

#include "SceneObjectOperator.h"
#include "SceneEntityObject.h"
#include "SceneManager.h"
#include "SceneInfo.h"
//#include "RootManager.h"


//-----------------------------------------------------------------------
CreateSceneEntityObjectOperator::CreateSceneEntityObjectOperator() :
mIsCreate(true)
, mSceneObjectId(0)
{
}
//-----------------------------------------------------------------------
const std::string& CreateSceneEntityObjectOperator::getGroupName(void) const
{
	static const std::string groupName = "SceneObject";
	return groupName;
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectOperator::add(SceneEntityObject* obj, bool isCreate)
{
	mIsCreate = isCreate;
	mSceneObjectId = obj->getId();
	mEntityObjId = obj->getEntityObjId();
	mMesh = obj->getMesh();
	mMaterial = obj->getMaterial();
	mPos = obj->getPosition();
	mRot = obj->getOrientation();
	mScale = obj->getScale();
	mLightId = obj->getLightId();
	mParticleID = obj->getParticleId();

	mCutDistanceLevel = obj->getCutDistanceLevel();
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectOperator::undo(void)
{
	apply(true);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectOperator::redo(void)
{
	apply(false);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectOperator::apply(bool reserve)
{
	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();

	if ( reserve == mIsCreate )
	{
		scene->destroySceneObject(mSceneObjectId);
	}
	else
	{

		SceneEntityObject* obj = new SceneEntityObject();
		obj->setId(mSceneObjectId);			//!!!

		if (mEntityObjId == 0)
		{
			obj->setMesh(mMesh);
			obj->setMaterial(mMaterial);
			obj->setLightId(mLightId);
			obj->setCutDistanceLevel(mCutDistanceLevel);
			obj->setParticleId(mParticleID);
		}
		else
		{
			obj->setEntityObjId(mEntityObjId);
		}

		obj->setPosition(mPos);
		obj->setOrientation(mRot);
		obj->setScale(mScale);

		obj->createInst();
		scene->addSceneObject(obj);
	}
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
SceneObjectTransformOperator::SceneObjectTransformOperator()
: mSceneObjectId(0)
{

}
//-----------------------------------------------------------------------
const std::string& SceneObjectTransformOperator::getGroupName(void) const
{
	static const std::string groupName = "SceneObject";
	return groupName;
}
//-----------------------------------------------------------------------
void SceneObjectTransformOperator::undo(void)
{
	apply(true);
}
//-----------------------------------------------------------------------
void SceneObjectTransformOperator::redo(void)
{
	apply(false);
}
//-----------------------------------------------------------------------
void SceneObjectTransformOperator::add(SceneObject* obj, const Ogre::Vector3& orgPos, const Ogre::Quaternion& orgRot, const Ogre::Vector3& orgScale)
{
	mSceneObjectId = obj->getId();
	mPos = obj->getPosition();
	mRot = obj->getOrientation();
	mScale = obj->getScale();
	mOrgPos = orgPos;
	mOrgRot = orgRot;
	mOrgScale = orgScale;
}
//-----------------------------------------------------------------------
void SceneObjectTransformOperator::apply(bool reserve)
{
	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
	SceneObject* obj = scene->getSceneObject(mSceneObjectId);
	assert(obj);
	

	if ( reserve )
	{
		obj->setPosition(mOrgPos);
		obj->setOrientation(mOrgRot);
		obj->setScale(mOrgScale);
	}
	else
	{
		obj->setPosition(mPos);
		obj->setOrientation(mRot);
		obj->setScale(mScale);
	}
}

