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

#include "ParticleAffectors/ParticleUniverseScaleAffector.h"
#include "ParticleUniverseVisualParticle.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real ScaleAffector::DEFAULT_X_SCALE = 1.0f;
	const Ogre::Real ScaleAffector::DEFAULT_Y_SCALE = 1.0f;
	const Ogre::Real ScaleAffector::DEFAULT_Z_SCALE = 1.0f;
	const Ogre::Real ScaleAffector::DEFAULT_XYZ_SCALE = 1.0f;

	//-----------------------------------------------------------------------
	ScaleAffector::ScaleAffector(void) : 
		ParticleAffector(),
		mDynScaleXSet(false),
		mDynScaleYSet(false),
		mDynScaleZSet(false),
		mDynScaleXYZSet(false)
	{
		mDynScaleX = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		mDynScaleY = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		mDynScaleZ = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		mDynScaleXYZ = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynScaleX))->setValue(DEFAULT_X_SCALE);
		(static_cast<DynamicAttributeFixed*>(mDynScaleY))->setValue(DEFAULT_Y_SCALE);
		(static_cast<DynamicAttributeFixed*>(mDynScaleZ))->setValue(DEFAULT_Z_SCALE);
		(static_cast<DynamicAttributeFixed*>(mDynScaleXYZ))->setValue(DEFAULT_XYZ_SCALE);
		mLatestTimeElapsed = 1.0f;
	}
	//-----------------------------------------------------------------------
	ScaleAffector::~ScaleAffector(void)
	{
		OGRE_DELETE_T(mDynScaleX, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		OGRE_DELETE_T(mDynScaleY, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		OGRE_DELETE_T(mDynScaleZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		OGRE_DELETE_T(mDynScaleXYZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		// Only continue if the particle is a visual particle
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		Ogre::Real ds = 0;
		Ogre::Real width = 0;
		Ogre::Real height = 0;
		Ogre::Real depth = 0;

		if (mDynScaleXYZSet)
		{
			ds = _calculateScale(mDynScaleXYZ, particle) * timeElapsed * _calculateAffectSpecialisationFactor(particle);
			if (visualParticle->width > 0)
			{
				width = visualParticle->width + ds * _mAffectorScale.x;
			}
			if (visualParticle->height > 0)
			{
				height = visualParticle->height + ds * _mAffectorScale.y;
			}
			if (visualParticle->depth > 0)
			{
				depth = visualParticle->depth + ds * _mAffectorScale.z;
			}
			visualParticle->setOwnDimensions(width, height, depth);
		}
		else
		{
			if (mDynScaleXSet)
			{
				ds = _calculateScale(mDynScaleX, particle) * timeElapsed;
				if (visualParticle->width > 0)
				{
					width = visualParticle->width + ds * _mAffectorScale.x;
				}
			}
			if (mDynScaleYSet)
			{
				ds = _calculateScale(mDynScaleY, particle) * timeElapsed;
				if (visualParticle->height > 0)
				{
					height = visualParticle->height + ds * _mAffectorScale.y;
				}
			}
			if (mDynScaleZSet)
			{
				ds = _calculateScale(mDynScaleZ, particle) * timeElapsed;
				if (visualParticle->depth > 0)
				{
					depth = visualParticle->depth + ds * _mAffectorScale.z;
				}
			}
			visualParticle->setOwnDimensions(width, height, depth);
		}
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::setDynScaleX(DynamicAttribute* dynScaleX)
	{
		if (mDynScaleX)
			OGRE_DELETE_T(mDynScaleX, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynScaleX = dynScaleX;
		mDynScaleXSet = true;
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::resetDynScaleX(bool resetToDefault)
	{
		if (resetToDefault)
		{
			OGRE_DELETE_T(mDynScaleX, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynScaleX = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			(static_cast<DynamicAttributeFixed*>(mDynScaleX))->setValue(DEFAULT_X_SCALE);
			mDynScaleXSet = false;
		}
		else
		{
			mDynScaleXSet = true;
		}
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::setDynScaleY(DynamicAttribute* dynScaleY)
	{
		if (mDynScaleY)
			OGRE_DELETE_T(mDynScaleY, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynScaleY = dynScaleY;
		mDynScaleYSet = true;
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::resetDynScaleY(bool resetToDefault)
	{
		if (resetToDefault)
		{
			OGRE_DELETE_T(mDynScaleY, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynScaleY = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			(static_cast<DynamicAttributeFixed*>(mDynScaleY))->setValue(DEFAULT_X_SCALE);
			mDynScaleYSet = false;
		}
		else
		{
			mDynScaleYSet = true;
		}
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::setDynScaleZ(DynamicAttribute* dynScaleZ)
	{
		if (mDynScaleZ)
			OGRE_DELETE_T(mDynScaleZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynScaleZ = dynScaleZ;
		mDynScaleZSet = true;
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::resetDynScaleZ(bool resetToDefault)
	{
		if (resetToDefault)
		{
			OGRE_DELETE_T(mDynScaleZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynScaleZ = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			(static_cast<DynamicAttributeFixed*>(mDynScaleZ))->setValue(DEFAULT_X_SCALE);
			mDynScaleYSet = false;
		}
		else
		{
			mDynScaleYSet = true;
		}
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::setDynScaleXYZ(DynamicAttribute* dynScaleXYZ)
	{
		if (mDynScaleXYZ)
			OGRE_DELETE_T(mDynScaleXYZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynScaleXYZ = dynScaleXYZ;
		mDynScaleXYZSet = true;
	}
	//-----------------------------------------------------------------------
	void ScaleAffector::resetDynScaleXYZ(bool resetToDefault)
	{
		if (resetToDefault)
		{
			OGRE_DELETE_T(mDynScaleXYZ, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynScaleXYZ = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			(static_cast<DynamicAttributeFixed*>(mDynScaleXYZ))->setValue(DEFAULT_XYZ_SCALE);
			mDynScaleXYZSet = false;
		}
		else
		{
			mDynScaleXYZSet = true;
		}
	}
	//-----------------------------------------------------------------------
	Ogre::Real ScaleAffector::_calculateScale(DynamicAttribute* dynScale, Particle* particle)
    {
		if (!particle)
			return 0.0f;

		return mDynamicAttributeHelper.calculate(dynScale, particle->timeFraction);
    }
	//-----------------------------------------------------------------------
	void ScaleAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		ScaleAffector* scaleAffector = static_cast<ScaleAffector*>(affector);
		scaleAffector->setDynScaleX(mDynamicAttributeFactory.cloneDynamicAttribute(getDynScaleX()));
		scaleAffector->setDynScaleY(mDynamicAttributeFactory.cloneDynamicAttribute(getDynScaleY()));
		scaleAffector->setDynScaleZ(mDynamicAttributeFactory.cloneDynamicAttribute(getDynScaleZ()));
		scaleAffector->setDynScaleXYZ(mDynamicAttributeFactory.cloneDynamicAttribute(getDynScaleXYZ()));
		scaleAffector->mDynScaleXSet = mDynScaleXSet;
		scaleAffector->mDynScaleYSet = mDynScaleYSet;
		scaleAffector->mDynScaleZSet = mDynScaleZSet;
		scaleAffector->mDynScaleXYZSet = mDynScaleXYZSet;
	}
}
