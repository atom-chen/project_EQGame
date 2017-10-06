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

#include "ParticleAffectors/ParticleUniverseRandomiser.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 Randomiser::DEFAULT_MAX_DEVIATION = Ogre::Vector3::ZERO;
	const Ogre::Real Randomiser::DEFAULT_TIME_STEP = 0.0f;
	const bool Randomiser::DEFAULT_RANDOM_DIRECTION = true;

	//-----------------------------------------------------------------------
	Randomiser::Randomiser(void) : 
		ParticleAffector(),
		mMaxDeviationX(DEFAULT_MAX_DEVIATION.x),
		mMaxDeviationY(DEFAULT_MAX_DEVIATION.y),
		mMaxDeviationZ(DEFAULT_MAX_DEVIATION.z),
		mTimeSinceLastUpdate(0.0f),
		mTimeStep(DEFAULT_TIME_STEP),
		mUpdate(true),
		mRandomDirection(DEFAULT_RANDOM_DIRECTION)
	{
	}
	//-----------------------------------------------------------------------
	Ogre::Real Randomiser::getMaxDeviationX(void) const
	{
		return mMaxDeviationX;
	}
	//-----------------------------------------------------------------------
	void Randomiser::setMaxDeviationX(Ogre::Real maxDeviationX)
	{
		mMaxDeviationX = maxDeviationX;
	}
	//-----------------------------------------------------------------------
	Ogre::Real Randomiser::getMaxDeviationY(void) const
	{
		return mMaxDeviationY;
	}
	//-----------------------------------------------------------------------
	void Randomiser::setMaxDeviationY(Ogre::Real maxDeviationY)
	{
		mMaxDeviationY = maxDeviationY;
	}
	//-----------------------------------------------------------------------
	Ogre::Real Randomiser::getMaxDeviationZ(void) const
	{
		return mMaxDeviationZ;
	}
	//-----------------------------------------------------------------------
	void Randomiser::setMaxDeviationZ(Ogre::Real maxDeviationZ)
	{
		mMaxDeviationZ = maxDeviationZ;
	}
	//-----------------------------------------------------------------------
	Ogre::Real Randomiser::getTimeStep(void) const
	{
		return mTimeStep;
	}
	//-----------------------------------------------------------------------
	void Randomiser::setTimeStep(Ogre::Real timeStep)
	{
		mTimeStep = timeStep;
		mTimeSinceLastUpdate = timeStep;
	}
	//-----------------------------------------------------------------------
	bool Randomiser::isRandomDirection(void) const
	{
		return mRandomDirection;
	}
	//-----------------------------------------------------------------------
	void Randomiser::setRandomDirection(bool randomDirection)
	{
		mRandomDirection = randomDirection;
	}
	//-----------------------------------------------------------------------
	void Randomiser::_preProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		if (technique->getNumberOfEmittedParticles() > 0)
		{
			mTimeSinceLastUpdate += timeElapsed;
			if (mTimeSinceLastUpdate > mTimeStep)
			{
				mTimeSinceLastUpdate -= mTimeStep;
				mUpdate = true;
			}
		}
	}
	//-----------------------------------------------------------------------
	void Randomiser::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (mUpdate)
		{
			if (mRandomDirection)
			{
				// Random direction: Change the direction after each update
				particle->direction += Ogre::Vector3(Ogre::Math::RangeRandom(-mMaxDeviationX, mMaxDeviationX),
					Ogre::Math::RangeRandom(-mMaxDeviationY, mMaxDeviationY),
					Ogre::Math::RangeRandom(-mMaxDeviationZ, mMaxDeviationZ));
			}
			else
			{
				// Explicitly check on 'freezed', because it passes the techniques' validation.
				if (particle->isFreezed())
					return;

				// Random position: Add the position deviation after each update
				particle->position += _mAffectorScale * Ogre::Vector3(Ogre::Math::RangeRandom(-mMaxDeviationX, mMaxDeviationX),
					Ogre::Math::RangeRandom(-mMaxDeviationY, mMaxDeviationY),
					Ogre::Math::RangeRandom(-mMaxDeviationZ, mMaxDeviationZ));
			}
		}
	}
	//-----------------------------------------------------------------------
	void Randomiser::_postProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		mUpdate = false;
	}
	//-----------------------------------------------------------------------
	void Randomiser::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		Randomiser* randomiser = static_cast<Randomiser*>(affector);
		randomiser->mMaxDeviationX = mMaxDeviationX;
		randomiser->mMaxDeviationY = mMaxDeviationY;
		randomiser->mMaxDeviationZ = mMaxDeviationZ;
		randomiser->setTimeStep(mTimeStep); // Also sets time since last update to appropriate value
		randomiser->mRandomDirection = mRandomDirection;
	}

}
