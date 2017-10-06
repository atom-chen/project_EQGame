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

#include "ParticleObservers/ParticleUniverseOnRandomObserver.h"
#include "ParticleUniverseParticle.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real OnRandomObserver::DEFAULT_THRESHOLD = 0.5f;

	//-----------------------------------------------------------------------
	OnRandomObserver::OnRandomObserver(void) : 
		ParticleObserver(),
		mThreshold(DEFAULT_THRESHOLD)
	{
	}
	//-----------------------------------------------------------------------
	void OnRandomObserver::_preProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		// Call parent
		ParticleObserver::_preProcessParticles(technique, timeElapsed);

		// Handle the _observe here
		_handleObserve(technique, 0, timeElapsed);
	}

	//-----------------------------------------------------------------------
	bool OnRandomObserver::_observe (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		return (Ogre::Math::UnitRandom() > mThreshold);
	}
	//-----------------------------------------------------------------------
	void OnRandomObserver::copyAttributesTo(ParticleObserver* observer)
	{
		ParticleObserver::copyAttributesTo(observer);
		OnRandomObserver* onRandomObserver = static_cast<OnRandomObserver*>(observer);
		onRandomObserver->mThreshold = mThreshold;
	}
}
