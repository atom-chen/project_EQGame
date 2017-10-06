/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "Externs/ParticleUniverseSceneDecoratorExtern.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	SceneDecoratorExtern::SceneDecoratorExtern(void) :  
		Extern(),
		mEntity(0),
		mEntitySet(false),
		mMeshName(Ogre::StringUtil::BLANK),
		mEntityName(Ogre::StringUtil::BLANK),
		mMaterialName("BaseWhite"),
		mMaterialNameSet(false),
		mScale(Ogre::Vector3::UNIT_SCALE),
		mSubnode(0),
		mPosition(Ogre::Vector3::ZERO)
	{
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::_prepare(ParticleTechnique* technique)
	{
		if (mEntitySet)
		{
			if (!mParentTechnique->getParentSystem())
			{
				return;
			}
			
			// Attach entity to a child scenenode. If the parent system is attached to a TagPoint, the entity cannot be attached.
			Ogre::SceneNode* sceneNode = mParentTechnique->getParentSystem()->getParentSceneNode();
			if (sceneNode)
			{
				mSubnode = sceneNode->createChildSceneNode();
				mSubnode->setScale(mScale);
				mSubnode->setPosition(mPosition);
				if (!mEntity)
				{
					createEntity();
					mSubnode->attachObject(mEntity);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::_unprepare(ParticleTechnique* technique)
	{
		// Todo
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::_interface(ParticleTechnique* technique, 
		Particle* particle, 
		Ogre::Real timeElapsed)
	{
		// Nothing to do yet
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::_notifyStart (void)
	{
		// Scale the node
		if (!mSubnode && mParentTechnique->getParentSystem())
		{
			Ogre::SceneNode* sceneNode = mParentTechnique->getParentSystem()->getParentSceneNode();
			mSubnode = sceneNode->createChildSceneNode();
		}

		if (!mEntity)
		{
			createEntity();
			mSubnode->attachObject(mEntity);
		}

		mSubnode->setScale(mScale);
		mSubnode->setPosition(mPosition);
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::_notifyStop (void)
	{
		if (mSubnode && mParentTechnique->getParentSystem())
		{
			Ogre::SceneNode* sceneNode = mParentTechnique->getParentSystem()->getParentSceneNode();
			Ogre::SceneNode::ChildNodeIterator it = sceneNode->getChildIterator();
			unsigned int i = 0;
			while (it.hasMoreElements()) 
			{ 
				Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(it.getNext());
				if (child == mSubnode)
				{
					sceneNode->removeAndDestroyChild(i);
					mSubnode = 0;
				}
				++i;
			}

			destroyEntity();
		}
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::createEntity(void)
	{
		if (!mEntity)
		{
			Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
			if (sceneManager)
			{
				std::stringstream ss;
				ss << this;
				mEntityName = mMeshName + ss.str();
				mEntity = sceneManager->createEntity(mEntityName, mMeshName);
				if (mMaterialNameSet)
				{
					mEntity->setMaterialName(mMaterialName);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::destroyEntity(void)
	{
		if (mEntity)
		{
			Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
			if (sceneManager)
			{
				sceneManager->destroyEntity(mEntityName);
				mEntity = 0;
				mEntityName = Ogre::StringUtil::BLANK;
			}
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::String& SceneDecoratorExtern::getMeshName(void) const
	{
		return mMeshName;
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::setMeshName(const Ogre::String& meshName)
	{
		mMeshName = meshName;
		mEntitySet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& SceneDecoratorExtern::getMaterialName(void) const
	{
		return mMaterialName;
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::setMaterialName(const Ogre::String& materialName)
	{
		mMaterialName = materialName;
		mMaterialNameSet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& SceneDecoratorExtern::getScale(void) const
	{
		return mScale;
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::setScale(Ogre::Vector3& scale)
	{
		mScale = scale;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& SceneDecoratorExtern::getPosition(void) const
	{
		return mPosition;
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::setPosition(Ogre::Vector3& position)
	{
		mPosition = position;
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExtern::copyAttributesTo (Extern* externObject)
	{
		Extern::copyAttributesTo(externObject);
		SceneDecoratorExtern* sceneDecoratorExtern = static_cast<SceneDecoratorExtern*>(externObject);
		sceneDecoratorExtern->setMaterialName(mMaterialName);
		sceneDecoratorExtern->setMeshName(mMeshName);
		sceneDecoratorExtern->mScale = mScale;
		sceneDecoratorExtern->mPosition = mPosition;
	}
}
