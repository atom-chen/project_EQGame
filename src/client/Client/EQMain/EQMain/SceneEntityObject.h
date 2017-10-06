/******************************************************************************
* ����: Y3
* ʱ��: 2009-07-10
* ����: ������Entity���
******************************************************************************/
#ifndef SceneEntityObject_H
#define SceneEntityObject_H

#include "EQMainPrerequisites.h"

#include "SceneObject.h"
#include "PickObjectProxy.h"
#include "EQOctreeNode.h"



enum CUT_DISTANCE_LEVEL
{
	CDL_NONE = 0,
	CDL_FAR = 10,
	CDL_MID = 20,
	CDL_CLOSE = 30,
};

class ZoneLight;
class Particle;
class SceneEntityObject : public SceneObject
{
public:
	class Spirit : public EQOctreeSpirit
	{
	public:
		const Ogre::AxisAlignedBox& _getWorldAABB(void) const;
		void notifyLastCheck();
		void notifyCheck();

		SceneEntityObject* owner;
	};

public:
	SceneEntityObject();
	~SceneEntityObject();

	void setEntityObjId(uint id);
	const uint getEntityObjId() {return mEntityObjId;}

	void setMesh(const std::string& mesh) { mMesh = mesh; }
	const std::string& getMesh() { return mMesh; }

	void setMaterial(const std::string& materia) { mMaterial = materia; }
	const std::string& getMaterial() { return mMaterial; }

	void setLightId(uint l) { mLightId = l; }
	uint getLightId() {return mLightId;}

	void setParticleId(uint particle) { mParticleId = particle;} 
	uint getParticleId() { return mParticleId; }

	void setLoopId(uint loop) { mLoop = loop;} 
	uint getLoopId() { return mLoop; }

	void setInteractId(uint interact) { mInteractId = interact;} 
	uint getInteractId() { return mInteractId; }

	void setCutDistanceLevel(int c) { mCutDistanceLevel = c; }
	int getCutDistanceLevel() {return mCutDistanceLevel;}

	Ogre::Entity* getEntity() { return mEntity; }

	/// �����Զ�����������, ���Ϊ��, ��ʾȡ���Զ�����
	void setAutoAnimationName(const std::string& name);
	const std::string& getAutoAnimationName() {return mAutoAnimationName;}

	void updateAnimation(float delta);

	std::vector<ZoneLight*>& getLight() { return mLights; }
	void turnOnLight();
	void turnOffLight();

	void setSelfIllum(const Ogre::ColourValue& color);
	void clearSelfIllum();

public:
	const std::string& getClassName();

	void showBoundingBox(bool show);

	void setPosition(const Ogre::Vector3& position);
	const Ogre::Vector3& getPosition() const;
	const Ogre::Vector3 & _getDerivedPosition(void) const;

	void yaw(const Ogre::Radian& angle);
	void setOrientation(const Ogre::Quaternion& qua);
	const Ogre::Quaternion& getOrientation() const;

	void setScale(const Ogre::Vector3& scale);
	const Ogre::Vector3 & getScale(void) const;

	void setVisible(bool v);

public:
	void createInst();
	void destroyInstWithoutLight();
	void sysnAnimation();

	bool bakeStaticGeometry(Ogre::StaticGeometry* staticGeometry);

	void goinCutDistance(EQOctreeNode* n);
	void gooutCutDistance();


protected:
	void changeMaterial();

protected:
	Ogre::SceneNode*	mSceneNode;
	Ogre::Entity*		mEntity;
	PickObjectProxy		mProxy;
	Ogre::AnimationState*	mAutoAnimationState;
	Spirit				mSpirite;

	uint				mEntityObjId;
	std::string			mMesh;
	std::string			mMaterial;
	std::string			mLightMat;		//!< �ƹ����
	uint				mLightId;		//!< Я���ƹ�
	uint				mParticleId;	//!< Я������
	Ogre::Vector3		mPos;
	Ogre::Quaternion	mRot;
	Ogre::Vector3		mScale;
	int					mCutDistanceLevel;
	std::string			mAutoAnimationName;

	bool				mIsStatic;		//!< �Ƿ�����˾�̬ģ����
	bool				mIsInCutState;	//!< �Ƿ����ھ������״̬

	/// Я����Դ
	std::vector<ZoneLight*>	mLights;

	/// ����
	Particle*			mParticle;

	/// ѭ���˵�id
	uint				mLoop;
	/// ����id
	uint				mInteractId;

	/// �Է���
	bool				mSelfIllumEnable;
};

#endif