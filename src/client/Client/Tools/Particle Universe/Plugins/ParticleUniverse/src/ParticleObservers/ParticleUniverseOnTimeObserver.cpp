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

#include "ParticleObservers/ParticleUniverseOnTimeObserver.h"
#include "ParticleUniverseParticle.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real OnTimeObserver::DEFAULT_THRESHOLD = 0.0f;
	const bool OnTimeObserver::DEFAULT_SINCE_START_SYSTEM = false;

	//-----------------------------------------------------------------------
	OnTimeObserver::OnTimeObserver(void) : ParticleObserver(),
		mThreshold(DEFAULT_THRESHOLD),
		mCompare(CO_GREATER_THAN),
		mSinceStartSystem(DEFAULT_SINCE_START_SYSTEM)
	{
	};
	//-----------------------------------------------------------------------
	void OnTimeObserver::_preProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		// Call parent
		ParticleObserver::_preProcessParticles(technique, timeElapsed);

		// Also observe if there are no particles emitted, because some of the event handlers do not only
		// perform an action on a particle.
		if (technique->getNumberOfEmittedParticles() == 0)
		{
			_handleObserve(technique, 0, timeElapsed);
		}
	}
	//-----------------------------------------------------------------------
	bool OnTimeObserver::_observe (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (mCompare == CO_GREATER_THAN)
		{
			if (mSinceStartSystem)
			{
				// Validate whether time since start of the particle system > threshold
				if (mParentTechnique->getParentSystem()->getTimeElapsedSinceStart() > mThreshold)
				{
					return true;
				}
			}
			else
			{
				// Validate whether time since start of the particle emission > threshold
				if (particle && (particle->totalTimeToLive - particle->timeToLive) > mThreshold)
				{
					return true;
				}
			}
		}
		else
		{
			if (mSinceStartSystem)
			{
				// Validate whether time since start of the particle system < threshold
				if (mParentTechnique->getParentSystem()->getTimeElapsedSinceStart() < mThreshold)
				{
					return true;
				}
			}
			else
			{
				// Validate whether time since start of the particle emission < threshold
				if (particle && (particle->totalTimeToLive - particle->timeToLive) < mThreshold)
				{
					return true;
				}
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------
	void OnTimeObserver::copyAttributesTo(ParticleObserver* observer)
	{
		ParticleObserver::copyAttributesTo(observer);

		OnTimeObserver* onTimeObserver = static_cast<OnTimeObserver*>(observer);
		onTimeObserver->mThreshold = mThreshold;
		onTimeObserver->mCompare = mCompare;
		onTimeObserver->mSinceStartSystem = mSinceStartSystem;
	}
}
