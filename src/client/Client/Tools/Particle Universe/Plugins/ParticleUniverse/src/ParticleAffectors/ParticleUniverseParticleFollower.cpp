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

#include "ParticleAffectors/ParticleUniverseParticleFollower.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real ParticleFollower::DEFAULT_MAX_DISTANCE = Ogre::Math::POS_INFINITY;
	const Ogre::Real ParticleFollower::DEFAULT_MIN_DISTANCE = 10.0f;

	//-----------------------------------------------------------------------
	ParticleFollower::ParticleFollower(void) : 
		ParticleAffector(),
		mMinDistance(DEFAULT_MIN_DISTANCE),
		mMaxDistance(DEFAULT_MAX_DISTANCE),
		mPositionPreviousParticle(Ogre::Vector3::ZERO),
		mFirst(false)
	{
	}
	//-----------------------------------------------------------------------
	void ParticleFollower::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		ParticleFollower* particleFollower = static_cast<ParticleFollower*>(affector);
		particleFollower->mMaxDistance = mMaxDistance;
		particleFollower->mMinDistance = mMinDistance;
	}
	//-----------------------------------------------------------------------
	void ParticleFollower::_firstParticle(ParticleTechnique* particleTechnique, 
		Particle* particle, 
		Ogre::Real timeElapsed)
	{
		// The first particle should go its own way
		mFirst = true;
	}
	//-----------------------------------------------------------------------
	void ParticleFollower::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (!mFirst)
		{
			// Only proceed if it isn´t the first one. Compensate for scaling.
			Ogre::Real distance = (particle->position / _mAffectorScale).distance(mPositionPreviousParticle / _mAffectorScale);
			if (distance > mMinDistance && distance < mMaxDistance)
			{
				// This particle drifts too much from the previous one; correct it!
				Ogre::Real f = mMinDistance/distance;
				particle->position = mPositionPreviousParticle + f * (particle->position - mPositionPreviousParticle);
			}
		}

		mPositionPreviousParticle = particle->position;
		mFirst = false;
	}
	//-----------------------------------------------------------------------
	Ogre::Real ParticleFollower::getMaxDistance(void) const
	{
		return mMaxDistance;
	}
	//-----------------------------------------------------------------------
	void ParticleFollower::setMaxDistance(Ogre::Real maxDistance)
	{
		mMaxDistance = maxDistance;
	}
	//-----------------------------------------------------------------------
	Ogre::Real ParticleFollower::getMinDistance(void) const
	{
		return mMinDistance;
	}
	//-----------------------------------------------------------------------
	void ParticleFollower::setMinDistance(Ogre::Real minDistance)
	{
		mMinDistance = minDistance;
	}
}
