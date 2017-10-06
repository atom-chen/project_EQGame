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

#include "ParticleAffectors/ParticleUniverseSineForceAffector.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real SineForceAffector::DEFAULT_FREQ_MIN = 1.0f;
	const Ogre::Real SineForceAffector::DEFAULT_FREQ_MAX = 1.0f;

	//-----------------------------------------------------------------------
	SineForceAffector::SineForceAffector(void) : 
		ForceAffector(),
		mAngle(361),
		mFrequencyMin(DEFAULT_FREQ_MIN),
		mFrequencyMax(DEFAULT_FREQ_MAX),
		mFrequency(1.0f)
	{
	}
	//-----------------------------------------------------------------------
	void SineForceAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ForceAffector::copyAttributesTo(affector);

		SineForceAffector* sineForceAffector = static_cast<SineForceAffector*>(affector);
		sineForceAffector->mFrequencyMin = mFrequencyMin;
		sineForceAffector->mFrequencyMax = mFrequencyMax;
		sineForceAffector->mFrequency = mFrequency;
		sineForceAffector->mAngle = mAngle;
	}
	//-----------------------------------------------------------------------
	void SineForceAffector::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		// Scale by time
		mAngle += mFrequency * timeElapsed;
		Ogre::Real sineValue = Ogre::Math::Sin(Ogre::Radian(mAngle));
		mScaledVector = mForceVector * timeElapsed * sineValue;

		if (mAngle > Ogre::Math::TWO_PI)
		{
			mAngle = 0.0f;

			if (mFrequencyMin != mFrequencyMax)
			{
				mFrequency = Ogre::Math::RangeRandom(mFrequencyMin, mFrequencyMax);
			}
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real SineForceAffector::getFrequencyMin(void) const
	{
		return mFrequencyMin;
	}
	//-----------------------------------------------------------------------
	void SineForceAffector::setFrequencyMin(const Ogre::Real frequencyMin)
	{
		mFrequencyMin = frequencyMin;
		if (frequencyMin > mFrequencyMax)
		{
			mFrequency = frequencyMin;
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real SineForceAffector::getFrequencyMax(void) const
	{
		return mFrequencyMax;
	}
	//-----------------------------------------------------------------------
	void SineForceAffector::setFrequencyMax(const Ogre::Real frequencyMax)
	{
		mFrequencyMax = frequencyMax;
		mFrequency = frequencyMax;
	}
	//-----------------------------------------------------------------------
	void SineForceAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		// Affect the direction
		if (mForceApplication == FA_ADD)
		{
			particle->direction += mScaledVector;
		}
		else
		{
			particle->direction = (particle->direction + mForceVector) / 2;
		}
	}

}
