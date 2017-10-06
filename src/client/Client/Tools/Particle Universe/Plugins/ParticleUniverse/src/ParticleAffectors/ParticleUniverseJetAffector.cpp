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

#include "ParticleAffectors/ParticleUniverseJetAffector.h"
#include "ParticleUniverseVisualParticle.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real JetAffector::DEFAULT_ACCELERATION = 1.0f;

	//-----------------------------------------------------------------------
	JetAffector::JetAffector (void) : 
		ParticleAffector(),
		mScaled(0.0f)
	{
		mDynAcceleration = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynAcceleration))->setValue(DEFAULT_ACCELERATION);
	}
	//-----------------------------------------------------------------------
	JetAffector::~JetAffector (void)
	{
		OGRE_DELETE_T(mDynAcceleration, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		mDynAcceleration = 0;
	}
	//-----------------------------------------------------------------------
	void JetAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		JetAffector* jetAffector = static_cast<JetAffector*>(affector);
		jetAffector->setDynAcceleration(mDynamicAttributeFactory.cloneDynamicAttribute(getDynAcceleration()));
	}
	//-----------------------------------------------------------------------
	void JetAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		mScaled = timeElapsed * (mDynAcceleration->getValue(particle->timeFraction));
		if (particle->direction == Ogre::Vector3::ZERO)
		{
			// Existing direction is zero, so use original direction
			particle->direction += (particle->originalDirection * mScaled);
		}
		else
		{
			particle->direction += (particle->direction * mScaled);
		}
	}
	//-----------------------------------------------------------------------
	void JetAffector::setDynAcceleration(DynamicAttribute* dynAcceleration)
	{
		if (mDynAcceleration)
			OGRE_DELETE_T(mDynAcceleration, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynAcceleration = dynAcceleration;
	}
}
