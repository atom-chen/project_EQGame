#include "EQMainPCH.h"

#include "SceneEntityObject.h"
#include "SceneManager.h"
#include "HouseScene.h"
#include "SystemGcs.h"
#include "SceneInfo.h"

#include "GraphicsTable.h"
#include "ZoneLightMgr.h"
#include "ParticleMgr.h"

const std::string AUTO_ANIMATION = "auto";


//-----------------------------------------------------------------------
const Ogre::AxisAlignedBox& SceneEntityObject::Spirit::_getWorldAABB(void) const
{
	if ( owner->mSceneNode )
		return owner->mSceneNode->_getWorldAABB();
	else
		return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------
void SceneEntityObject::Spirit::notifyLastCheck()
{
	owner->setVisible(false);
}
//-----------------------------------------------------------------------
void SceneEntityObject::Spirit::notifyCheck()
{
	owner->setVisible(true);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
SceneEntityObject::SceneEntityObject() :
mSceneNode(NULL),
mEntity(NULL),
mLightId(0),
mAutoAnimationState(NULL),
mPos(Ogre::Vector3::ZERO),
mRot(Ogre::Quaternion::IDENTITY),
mScale(Ogre::Vector3::UNIT_SCALE),
mCutDistanceLevel(0),
mAutoAnimationName(AUTO_ANIMATION),
mIsStatic(false),
mIsInCutState(false),
mEntityObjId(0),
mParticle(NULL),
mParticleId(0),
mLoop(0),
mInteractId(0),
mSelfIllumEnable(false)
{
	mProxy.setObject(this);
	mSpirite.owner = this;
}
//-----------------------------------------------------------------------
SceneEntityObject::~SceneEntityObject()
{

	if(mLights.size() > 0)
	{
		for(int i=0; i<(int)mLights.size(); i++)
			ZoneLightMgr::getSingleton().destroyZoneLight(mLights[i]);
	}
	mLights.clear();

	destroyInstWithoutLight();

	if(mParticle)
		ParticleMgr::getSingleton().removeParticle(mParticle);
}
//-----------------------------------------------------------------------
const std::string& SceneEntityObject::getClassName()
{
	static const std::string g_className = "SceneEntityObject";
	return g_className;
}
//-----------------------------------------------------------------------
void SceneEntityObject::setEntityObjId(uint id)
{ 
	mEntityObjId = id;

	if ( mEntityObjId == 0 )
		return;

	const SceneEntityObjRow* row = GraphicsTableManager::getSingleton().getSceneEntityObjRow(id);
	if (row == NULL)
	{
		EQ_EXCEPT("SceneEntityObjRow id no found - " + Ogre::StringConverter::toString(id),
			"SceneEntityObject::setEntityObjId");
	}

	setMesh(row->mesh);
	setMaterial(row->material);
	setLightId(row->light);
	setParticleId(row->particle);
	setLoopId(row->loop);

	int cut = 0;
	if ( row->cutLevel == "f" )
		cut = CDL_FAR;
	else if ( row->cutLevel == "m" )
		cut = CDL_MID;
	else if ( row->cutLevel == "c" )
		cut = CDL_CLOSE;
	setCutDistanceLevel(cut);
}
//-----------------------------------------------------------------------
void SceneEntityObject::createInst()
{
	assert( !mIsStatic );
	assert( !mMesh.empty() );
	assert( mEntity == NULL );

	Ogre::SceneNode* parentNode = EQSceneManager::getSingleton().getSceneInfo()->getSceneObjectNode();
	mSceneNode = parentNode->createChildSceneNode();
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();

	//����Ӧ�ô�����ģ����ص�����, ������������ض�λ

	mEntity = mgr->createEntity(mSceneNode->getName(), mMesh);

	mEntity->setUserObject(&mProxy);
	mSceneNode->attachObject(mEntity);

		

	//�������
	changeMaterial();
	//�ƹ�����ݶ���һ����ģ�͵Ĳ���
	mLightMat = mEntity->getSubEntity(0)->getMaterialName();

	//���ÿռ�任
	mSceneNode->setPosition(mPos);
	mSceneNode->setOrientation(mRot);
	mSceneNode->setScale(mScale);

	//ͬ������
	assert( mAutoAnimationState == NULL );
	sysnAnimation();

	if(mLightId)
	{
		const LightRow* l = GraphicsTableManager::getSingleton().getLightRow(mLightId);
		mLights = ZoneLightMgr::getSingleton().createZoneLights(l, false);
		for(uint i=0; i<mLights.size(); i++)
		{
			//Ogre::SceneNode* lnode = parentNode->createChildSceneNode();
			//lnode->setPosition(mPos);
			//lnode->setOrientation(mRot);
			//lnode->setScale(mScale);
			mLights[i]->setNode(mSceneNode->createChildSceneNode());
		}
	}

	if(mParticleId)
	{
		mParticle = ParticleMgr::getSingleton().createParticle(mParticleId);

		TransformInfo trans;
		switch(mParticle->getTrans())
		{
		case TRANS_NONE:
			break;
		case TRANS_POSITION:
			trans.mPosition = mSceneNode->_getDerivedPosition();
			break;
		case TRANS_ORIENTATION:
			trans.mRotation = mSceneNode->_getDerivedOrientation();
			break;
		case TRANS_POSORI:
			trans.mRotation = mSceneNode->_getDerivedOrientation();
			trans.mPosition = mSceneNode->_getDerivedPosition();
			break;
		}
		mParticle->setTransform(trans);

		if(mParticle->isStartImmediately())
			mParticle->start();
	}

	//����ʹ��lightmap���ر���Ӱ
	mEntity->setCastShadows(false);
}
//-----------------------------------------------------------------------
void SceneEntityObject::destroyInstWithoutLight()
{
	if ( mEntity )
	{
		mSceneNode->detachObject(mEntity);
		SystemGcs::getSingleton().getSceneManager()->destroyEntity(mEntity);
		mEntity = NULL;
		mAutoAnimationState = NULL;
	}

	if ( mSceneNode && mLights.empty() )	//û�еƹ��ɾ��
	{
		mSceneNode->removeAndDestroyAllChildren();
		mSceneNode->getCreator()->destroySceneNode(mSceneNode);
		mSceneNode = NULL;
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::sysnAnimation()
{
	//�Զ�����
	if ( mAutoAnimationState )
	{
		mAutoAnimationState->setEnabled(false);
		mAutoAnimationState = NULL;
	}

	if ( !mAutoAnimationName.empty() && mEntity
		&& mEntity->getAllAnimationStates()
		&& mEntity->getAllAnimationStates()->hasAnimationState(mAutoAnimationName))
	{
		mAutoAnimationState = mEntity->getAnimationState(mAutoAnimationName);
		mAutoAnimationState->setTimePosition(Ogre::Math::UnitRandom() * mAutoAnimationState->getLength());
		mAutoAnimationState->setEnabled(true);
		mAutoAnimationState->setLoop(true);
	}

}
//-----------------------------------------------------------------------
void SceneEntityObject::setAutoAnimationName(const std::string& name)
{
	if ( mIsStatic )
		return;

	if ( mAutoAnimationName == name )
		return;

	mAutoAnimationName = name;
	sysnAnimation();
}
//-----------------------------------------------------------------------
void SceneEntityObject::updateAnimation(float delta)
{
	if ( mEntity && mEntity->getVisible() )
	{
		//���ļ���ʱ��Ÿ��¶���
		if ( mAutoAnimationState )
			mAutoAnimationState->addTime(delta);
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::changeMaterial()
{
	if ( mMaterial.empty() )
		return;

	Ogre::Entity* ent = mEntity;

	//������ʲ�����, ��ô���Զ����ð�ɫ����, �������ڵ���
	std::vector<std::string> matList = Ogre::StringUtil::split(mMaterial);
	if ( matList.size() == 1 && !matList[0].empty() )
	{
		ent->setMaterialName(matList[0]);
	}
	else if ( matList.size() > 1 )
	{
		if ( ent->getNumSubEntities() < matList.size() )
			matList.resize(ent->getNumSubEntities());

		for ( int i = 0; i < (int)matList.size(); ++i )
		{
			ent->getSubEntity(i)->setMaterialName(matList[i]);
		}
	}	
	else
	{
		//���������
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::showBoundingBox(bool show)
{

	if ( mSceneNode )
	{	
		mSceneNode->showBoundingBox(show);
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::setPosition(const Ogre::Vector3& position)
{
	mPos = position;

	if ( mSceneNode )
	{
		mSceneNode->setPosition(mPos);

		//������������ƶ���������Ϊ�˱༭���������ƶ�
		if(mParticle)
		{
			TransformInfo trans;
			trans.mRotation = mSceneNode->_getDerivedOrientation();
			trans.mPosition = mSceneNode->_getDerivedPosition();
			mParticle->setTransform(trans);
		}
	}
}
//-----------------------------------------------------------------------
const Ogre::Vector3& SceneEntityObject::getPosition() const
{
	return mPos;
}
//-----------------------------------------------------------------------
const Ogre::Vector3 & SceneEntityObject::_getDerivedPosition(void) const
{
	if ( !mSceneNode )
		return Ogre::Vector3::ZERO;

	return mSceneNode->_getDerivedPosition();
}
//-----------------------------------------------------------------------
void SceneEntityObject::yaw(const Ogre::Radian& angle)
{
	Ogre::Quaternion q;
	q.FromAngleAxis(angle,Ogre::Vector3::UNIT_Y);

	mRot = mRot * q;

	if ( mSceneNode )
	{
		mSceneNode->setOrientation(mRot);

		//�������������ת��������Ϊ�˱༭����������ת
		if(mParticle)
		{
			TransformInfo trans;
			trans.mRotation = mSceneNode->_getDerivedOrientation();
			trans.mPosition = mSceneNode->_getDerivedPosition();
			mParticle->setTransform(trans);
		}
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::setOrientation(const Ogre::Quaternion& qua)
{
	mRot = qua;

	if ( mSceneNode )
	{
		mSceneNode->setOrientation(mRot);

		//�������������ת��������Ϊ�˱༭����������ת
		if(mParticle)
		{
			TransformInfo trans;
			trans.mRotation = mSceneNode->_getDerivedOrientation();
			trans.mPosition = mSceneNode->_getDerivedPosition();
			mParticle->setTransform(trans);
		}
	}
}
//-----------------------------------------------------------------------
const Ogre::Quaternion& SceneEntityObject::getOrientation() const
{
	return mRot;
}
//-----------------------------------------------------------------------
void SceneEntityObject::setScale(const Ogre::Vector3& scale)
{
	mScale = scale;

	if ( mSceneNode )
	{
		mSceneNode->setScale(mScale);

		//��������������ţ�������Ϊ�˱༭������������
		if(mParticle)
		{
			mParticle->setScale(mScale);
		}
	}
}
//-----------------------------------------------------------------------
const Ogre::Vector3 & SceneEntityObject::getScale(void) const
{
	return mScale;
}
//-----------------------------------------------------------------------
bool SceneEntityObject::bakeStaticGeometry(Ogre::StaticGeometry* staticGeometry)
{
	assert(!mIsInCutState);

	if ( mIsStatic )
		return mIsStatic;

	// ������Զ�����, �򲻼����Զ�״̬
	if ( mAutoAnimationState != NULL )
		return mIsStatic;

	staticGeometry->addSceneNode(mEntity->getParentSceneNode());
	destroyInstWithoutLight();
	mIsStatic = true;


	return mIsStatic;
}
//-----------------------------------------------------------------------
void SceneEntityObject::goinCutDistance(EQOctreeNode* n)
{
	assert(!mIsStatic);

	if ( mIsInCutState )
		return;

	mIsInCutState = true;

	n->attach(&mSpirite);

	if ( mCutDistanceLevel != CDL_NONE )
	{
		setVisible(false);
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::gooutCutDistance()
{
	if ( !mIsInCutState )
		return;

	mIsInCutState = false;

	//����Ҫunattach, ϵͳ���Զ�����

	if ( mCutDistanceLevel != CDL_NONE )
	{
		setVisible(true);
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::setVisible(bool v)
{
	mEntity->setVisible(v);

	for(uint i=0; i<mLights.size(); i++)
	{
		mLights[i]->setMute(!v);
	}

	if(mParticle)
		mParticle->setVisible(v);
}
//-----------------------------------------------------------------------
void SceneEntityObject::turnOnLight()
{
	if(mLights.size() > 0)
	{
		for(uint i=0; i<mLights.size(); i++)
			mLights[i]->turnOn();

		std::string newName = mLightMat + "_on";
		if(Ogre::MaterialManager::getSingleton().resourceExists(newName))
		{
			Ogre::MaterialPtr newMat = Ogre::MaterialManager::getSingleton().getByName(newName);
			mEntity->getSubEntity(0)->setMaterial(newMat);
		}
	}
}

//-----------------------------------------------------------------------
void SceneEntityObject::turnOffLight()
{
	if(mLights.size() > 0)
	{
		for(uint i=0; i<mLights.size(); i++)
			mLights[i]->turnOff();

		std::string newName = mLightMat + "_on";
		if(Ogre::MaterialManager::getSingleton().resourceExists(newName))
		{
			Ogre::MaterialPtr oriMat = Ogre::MaterialManager::getSingleton().getByName(mLightMat);
			mEntity->getSubEntity(0)->setMaterial(oriMat);
		}
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::setSelfIllum(const Ogre::ColourValue& color)
{
	if(!mSelfIllumEnable)
	{
		uint c = mEntity->getNumSubEntities();
		for ( uint i = 0; i < c; ++i )
		{
			Ogre::SubEntity* sub = mEntity->getSubEntity(i);
			Ogre::MaterialPtr mat = sub->getMaterial();

			std::string newName;
			do {
				newName = mat->getName() + "-auto-" + Ogre::StringConverter::toString(Ogre::Math::UnitRandom());
			} while (Ogre::MaterialManager::getSingleton().resourceExists(newName));
			Ogre::MaterialPtr cloneMat = mat->clone(newName);

			sub->setMaterial(cloneMat);

			Ogre::Technique* tec = cloneMat->getBestTechnique();
			Ogre::Pass* pass = tec->getPass("BassPass");
			if(pass)
			{
				pass->setSelfIllumination(pass->getSelfIllumination() + color);
			}
		}

		mSelfIllumEnable = true;
	}
}
//-----------------------------------------------------------------------
void SceneEntityObject::clearSelfIllum()
{
	if(mSelfIllumEnable)
	{
		changeMaterial();

		mSelfIllumEnable = false;
	}
}
//-----------------------------------------------------------------------