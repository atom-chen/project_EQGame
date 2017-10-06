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

#include "ParticleObservers/ParticleUniverseOnCollisionObserver.h"
#include "ParticleUniverseParticle.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	bool OnCollisionObserver::_observe (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		return particle->hasEventFlags(Particle::PEF_COLLIDED);
	}
}
