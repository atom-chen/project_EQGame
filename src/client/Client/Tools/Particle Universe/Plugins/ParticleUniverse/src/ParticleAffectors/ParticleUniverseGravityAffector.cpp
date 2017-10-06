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

#include "ParticleAffectors/ParticleUniverseGravityAffector.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real GravityAffector::DEFAULT_GRAVITY = 1.0f;

	//-----------------------------------------------------------------------
	GravityAffector::GravityAffector(void) : 
		ParticleAffector(),
		mGravity(DEFAULT_GRAVITY)
	{
	}
	//-----------------------------------------------------------------------
	void GravityAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		GravityAffector* gravityAffector = static_cast<GravityAffector*>(affector);
		gravityAffector->mGravity = mGravity;
	}
	//-----------------------------------------------------------------------
	void GravityAffector::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		// Calculate the affectors' position so it is also safe to use mDerivedPosition.
		getDerivedPosition();
	}
	//-----------------------------------------------------------------------
	inline void GravityAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		/** Applying Newton's law of universal gravitation.	*/
		Ogre::Vector3 distance = mDerivedPosition - particle->position;
		Ogre::Real length = distance.squaredLength();
		if (length > 0)
		{
			Ogre::Real force = (mGravity * particle->mass * mass) / length;
			particle->direction += force * distance * timeElapsed * _calculateAffectSpecialisationFactor(particle);
		}
	}
	//-----------------------------------------------------------------------
	Ogre::Real GravityAffector::getGravity(void) const
	{
		return mGravity;
	}
	//-----------------------------------------------------------------------
	void GravityAffector::setGravity(Ogre::Real gravity)
	{
		mGravity = gravity;
	}
}
