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

#include "ParticleAffectors/ParticleUniverseVortexAffector.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 VortexAffector::DEFAULT_ROTATION_VECTOR = Ogre::Vector3::ZERO;
	const Ogre::Real VortexAffector::DEFAULT_ROTATION_SPEED = 1.0f;

	//-----------------------------------------------------------------------
	VortexAffector::VortexAffector(void) : 
		ParticleAffector(),
		mRotation(Ogre::Quaternion::IDENTITY),
		mRotationVector(DEFAULT_ROTATION_VECTOR)
	{
		mDynRotationSpeed = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynRotationSpeed))->setValue(DEFAULT_ROTATION_SPEED);
	}
	//-----------------------------------------------------------------------
	VortexAffector::~VortexAffector(void)
	{
		if (mDynRotationSpeed)
		{
			OGRE_DELETE_T(mDynRotationSpeed, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynRotationSpeed = 0;
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& VortexAffector::getRotationVector(void) const
	{
		return mRotationVector;
	}
	//-----------------------------------------------------------------------
	void VortexAffector::setRotationVector(const Ogre::Vector3& rotationVector)
	{
		mRotationVector = rotationVector;
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* VortexAffector::getRotationSpeed(void) const
	{
		return mDynRotationSpeed;
	}
	//-----------------------------------------------------------------------
	void VortexAffector::setRotationSpeed(DynamicAttribute* dynRotationSpeed)
	{
		if (mDynRotationSpeed)
			OGRE_DELETE_T(mDynRotationSpeed, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynRotationSpeed = dynRotationSpeed;
	}
	//-----------------------------------------------------------------------
	void VortexAffector::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		ParticleSystem* sys = mParentTechnique->getParentSystem();
		if (sys)
		{
			mRotation.FromAngleAxis(Ogre::Radian(_calculateRotationSpeed() * timeElapsed), sys->getDerivedOrientation() * mRotationVector);
		}
		else
		{
			mRotation.FromAngleAxis(Ogre::Radian(_calculateRotationSpeed() * timeElapsed), mRotationVector);
		}
		getDerivedPosition();
	}
	//-----------------------------------------------------------------------
	Ogre::Radian VortexAffector::_calculateRotationSpeed(void)
	{
		return Ogre::Radian(mDynamicAttributeHelper.calculate(mDynRotationSpeed, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart()));
	}
	//-----------------------------------------------------------------------
	void VortexAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		// Explicitly check on 'freezed', because it passes the techniques' validation.
		if (particle->isFreezed())
			return;

		// Rotate position, direction and orientation (visible particle only) and compensate for the affector position
		// Also take the affect specialisation into account
		Ogre::Vector3 local = particle->position - mDerivedPosition;
		particle->position = mDerivedPosition + mRotation * local;
		particle->direction = mRotation * particle->direction;

		if (particle->particleType == Particle::PT_VISUAL)
		{
			VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
			visualParticle->orientation = mRotation * visualParticle->orientation;
		}
	}
	//-----------------------------------------------------------------------
	void VortexAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		VortexAffector* vortexAffector = static_cast<VortexAffector*>(affector);
		vortexAffector->mRotation = mRotation;
		vortexAffector->mRotationVector = mRotationVector;
		vortexAffector->setRotationSpeed(mDynamicAttributeFactory.cloneDynamicAttribute(getRotationSpeed()));
	}

}
