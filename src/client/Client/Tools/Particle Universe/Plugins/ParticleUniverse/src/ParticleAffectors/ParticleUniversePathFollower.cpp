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

#include "ParticleAffectors/ParticleUniversePathFollower.h"
#include "ParticleUniverseVisualParticle.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void PathFollower::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);
		PathFollower* pathFollower = static_cast<PathFollower*>(affector);
		unsigned short numPoints = mSpline.getNumPoints();
		pathFollower->mSpline.clear();
		for (unsigned short i = 0; i < numPoints; ++i)
		{
			pathFollower->mSpline.addPoint(mSpline.getPoint(i));
		}
	}
	//-----------------------------------------------------------------------
	void PathFollower::addPoint(const Ogre::Vector3& point)
	{
		mSpline.addPoint(point);
	}
	//-----------------------------------------------------------------------
	void PathFollower::clearPoints (void)
	{
		mSpline.clear();
	}
	//-----------------------------------------------------------------------
	void PathFollower::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (mSpline.getNumPoints() > 0)
		{
			Ogre::Real timeFraction = (particle->totalTimeToLive - particle->timeToLive) / particle->totalTimeToLive;
			Ogre::Real timeFractionPlusDelta = timeFraction + timeElapsed;
			timeFractionPlusDelta = timeFractionPlusDelta < 1.0 ? timeFractionPlusDelta : 1.0;
			particle->position += _mAffectorScale * (mSpline.interpolate(timeFraction) - mSpline.interpolate(timeFractionPlusDelta));
		}
	}
}
