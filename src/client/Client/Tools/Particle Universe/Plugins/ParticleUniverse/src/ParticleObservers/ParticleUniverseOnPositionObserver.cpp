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

#include "ParticleObservers/ParticleUniverseOnPositionObserver.h"
#include "ParticleUniverseParticle.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 OnPositionObserver::DEFAULT_POSITION_THRESHOLD = Ogre::Vector3::ZERO;

	//-----------------------------------------------------------------------
	OnPositionObserver::OnPositionObserver(void) : 
		ParticleObserver(),
		mPositionXThreshold(DEFAULT_POSITION_THRESHOLD.x),
		mPositionYThreshold(DEFAULT_POSITION_THRESHOLD.y),
		mPositionZThreshold(DEFAULT_POSITION_THRESHOLD.z),
		mComparePositionX(CO_LESS_THAN),
		mComparePositionY(CO_LESS_THAN),
		mComparePositionZ(CO_LESS_THAN),
		mPositionXThresholdSet(false),
		mPositionYThresholdSet(false),
		mPositionZThresholdSet(false)
	{
	};
	//-----------------------------------------------------------------------
	bool OnPositionObserver::_observe (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (!particle)
			return false;

		if (mPositionXThresholdSet && mComparePositionX == CO_LESS_THAN && particle->position.x < mPositionXThreshold)
			return true;
		if (mPositionXThresholdSet && mComparePositionX == CO_GREATER_THAN && particle->position.x > mPositionXThreshold)
			return true;

		if (mPositionYThresholdSet && mComparePositionY == CO_LESS_THAN && particle->position.y < mPositionYThreshold)
			return true;
		if (mPositionYThresholdSet && mComparePositionY == CO_GREATER_THAN && particle->position.y > mPositionYThreshold)
			return true;

		if (mPositionZThresholdSet && mComparePositionZ == CO_LESS_THAN && particle->position.z < mPositionZThreshold)
			return true;
		if (mPositionZThresholdSet && mComparePositionZ == CO_GREATER_THAN && particle->position.z > mPositionZThreshold)
			return true;

		return false;
	}
	//-----------------------------------------------------------------------
	void OnPositionObserver::copyAttributesTo(ParticleObserver* observer)
	{
		ParticleObserver::copyAttributesTo(observer);

		OnPositionObserver* onPositionObserver = static_cast<OnPositionObserver*>(observer);
		onPositionObserver->mPositionXThreshold = mPositionXThreshold;
		onPositionObserver->mPositionYThreshold = mPositionYThreshold;
		onPositionObserver->mPositionZThreshold = mPositionZThreshold;
		onPositionObserver->mComparePositionX = mComparePositionX;
		onPositionObserver->mComparePositionY = mComparePositionY;
		onPositionObserver->mComparePositionZ = mComparePositionZ;
		onPositionObserver->mPositionXThresholdSet = mPositionXThresholdSet;
		onPositionObserver->mPositionYThresholdSet = mPositionYThresholdSet;
		onPositionObserver->mPositionZThresholdSet = mPositionZThresholdSet;
	}

}
