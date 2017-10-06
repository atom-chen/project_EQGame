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

#include "ParticleAffectors/ParticleUniverseLineAffector.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real LineAffector::DEFAULT_MAX_DEVIATION = 1.0f;
	const Ogre::Real LineAffector::DEFAULT_TIME_STEP = 0.1f;
	const Ogre::Vector3 LineAffector::DEFAULT_END = Ogre::Vector3::ZERO;
	const Ogre::Real LineAffector::DEFAULT_DRIFT = 0.0f;

	//-----------------------------------------------------------------------
	LineAffector::LineAffector(void) : 
		ParticleAffector(),
		mMaxDeviation(DEFAULT_MAX_DEVIATION),
		_mScaledMaxDeviation(1.0f),
		mEnd(DEFAULT_END),
		mTimeSinceLastUpdate(0.0f),
		mTimeStep(DEFAULT_TIME_STEP),
		mUpdate(true),
		mFirst(true),
		mDrift(DEFAULT_DRIFT),
		mOneMinusDrift(1.0f)
	{
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineAffector::getMaxDeviation(void) const
	{
		return mMaxDeviation;
	}
	//-----------------------------------------------------------------------
	void LineAffector::setMaxDeviation(Ogre::Real maxDeviation)
	{
		mMaxDeviation = maxDeviation;
		_mScaledMaxDeviation = mMaxDeviation * _mAffectorScale.length();
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& LineAffector::getEnd(void) const
	{
		return mEnd;
	}
	//-----------------------------------------------------------------------
	void LineAffector::setEnd(const Ogre::Vector3& end)
	{
		mEnd = end;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineAffector::getTimeStep(void) const
	{
		return mTimeStep;
	}
	//-----------------------------------------------------------------------
	void LineAffector::setTimeStep(Ogre::Real timeStep)
	{
		mTimeStep = timeStep;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineAffector::getDrift(void) const
	{
		return mDrift;
	}
	//-----------------------------------------------------------------------
	void LineAffector::setDrift(Ogre::Real drift)
	{
		mDrift = drift;
		mOneMinusDrift = 1.0f - drift;
	}
	//-----------------------------------------------------------------------
	void LineAffector::_notifyRescaled(const Ogre::Vector3& scale)
	{
		ParticleAffector::_notifyRescaled(scale);
		_mScaledMaxDeviation = mMaxDeviation * scale.length();
	}
	//-----------------------------------------------------------------------
	void LineAffector::_firstParticle(ParticleTechnique* particleTechnique, 
		Particle* particle, 
		Ogre::Real timeElapsed)
	{
		// The first particle should stay on its place
		mFirst = true;
	}
	//-----------------------------------------------------------------------
	void LineAffector::_preProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		if (technique->getNumberOfEmittedParticles() > 0)
		{
			mTimeSinceLastUpdate += timeElapsed;
			while (mTimeSinceLastUpdate > mTimeStep)
			{
				mTimeSinceLastUpdate -= mTimeStep;
				mUpdate = true;
			}
		}
		mParentTechnique->getParentSystem()->rotationOffset(mEnd); // Always update
	}
	//-----------------------------------------------------------------------
	void LineAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		mParentTechnique->getParentSystem()->rotationOffset(particle->originalPosition); // Always update
		if (mUpdate && Ogre::Math::UnitRandom() > 0.5 && !mFirst)
		{
			// Generate a random vector perpendicular on the line
			Ogre::Vector3 perpendicular = mEnd.crossProduct(Ogre::Vector3(Ogre::Math::RangeRandom(-1, 1), 
				Ogre::Math::RangeRandom(-1, 1), 
				Ogre::Math::RangeRandom(-1, 1)));
			perpendicular.normalise();

			// Determine a random point near the line.
			Ogre::Vector3 targetPosition = particle->originalPosition + _mScaledMaxDeviation * Ogre::Math::UnitRandom() * perpendicular;

			/** Set the new position.
			@remarks
				This affector already takes rotational offset of the particle system into account. This means that there is no need
				to set the particle system to keep_local to 'true'. The reason is that this is a specialized affector that calculates
				a new particle position instead of a direction.
			*/
			particle->position = mDrift * targetPosition + mOneMinusDrift * particle->position;
			mParentTechnique->getParentSystem()->rotationOffset(particle->position);
		}
		mFirst = false;
	}
	//-----------------------------------------------------------------------
	void LineAffector::_postProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		mUpdate = false;
	}
	//-----------------------------------------------------------------------
	void LineAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		LineAffector* lineAffector = static_cast<LineAffector*>(affector);

		lineAffector->setMaxDeviation(mMaxDeviation);
		lineAffector->mEnd = mEnd;
		lineAffector->mTimeStep = mTimeStep;
		lineAffector->mDrift = mDrift;
		lineAffector->mOneMinusDrift = mOneMinusDrift;
	}
}
