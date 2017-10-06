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

#include "ParticleEmitters/ParticleUniverseLineEmitter.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 LineEmitter::DEFAULT_END = Ogre::Vector3::ZERO;
	const Ogre::Real LineEmitter::DEFAULT_MIN_INCREMENT = 0.0f;
	const Ogre::Real LineEmitter::DEFAULT_MAX_INCREMENT = 0.0f;
	const Ogre::Real LineEmitter::DEFAULT_MAX_DEVIATION = 0.0f;

	//-----------------------------------------------------------------------
	LineEmitter::LineEmitter(void) : 
		ParticleEmitter(),
		mEnd(DEFAULT_END),
		mPerpendicular(Ogre::Vector3::ZERO),
		mMaxDeviation(DEFAULT_MAX_DEVIATION),
		mMinIncrement(DEFAULT_MIN_INCREMENT),
		mMaxIncrement(DEFAULT_MAX_INCREMENT),
		mIncrement(0.0f),
		mLength(0.0f),
		mIncrementsLeft(true),
		mFirst(true),
		_mScaledEnd(Ogre::Vector3::ZERO),
		_mScaledMaxDeviation(0.0f),
		_mScaledMinIncrement(0.0f),
		_mScaledMaxIncrement(0.0f),
		_mScaledLength(0.0f)
	{
	}
	//-----------------------------------------------------------------------
	void LineEmitter::_notifyStart (void)
	{
		// Reset the incremental attributes to allow a restart.
		ParticleEmitter::_notifyStart();
		mIncrement = 0.0f;
		mIncrementsLeft = true;
		mFirst = true;
	}
	//-----------------------------------------------------------------------
	void LineEmitter::_notifyRescaled(const Ogre::Vector3& scale)
	{
		// Scale the internal attributes and use them, otherwise this results in too many calculations per particle
		ParticleEmitter::_notifyRescaled(scale);
		Ogre::Real scaleLength = scale.length();
		_mScaledEnd = mEnd * scale;
		_mScaledMaxDeviation = mMaxDeviation * scaleLength;
		_mScaledMinIncrement = mMinIncrement * scaleLength;
		_mScaledMaxIncrement = (mMaxIncrement - mMinIncrement) * scaleLength;
		_mScaledLength = _mScaledEnd.length();
	}
	//-----------------------------------------------------------------------
	unsigned short LineEmitter::_calculateRequestedParticles(Ogre::Real timeElapsed)
	{
		unsigned short requested = ParticleEmitter::_calculateRequestedParticles(timeElapsed);

		if (_mScaledMaxIncrement > 0)
		{
			// Do not create particles if there are no increments left
			if (!mIncrementsLeft)
			{
				// Set back to 0 again
				requested = 0;
			}

			// For duration + repeat/delay: Clear the particles and start all over again.
			if (!mEnabled)
			{
				_notifyStart();
				mParentTechnique->lockAllParticles(); // Needed to set the first generated particle also first in the list with particles
			}
		}

		return requested;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& LineEmitter::getEnd(void) const
	{
		return mEnd;
	}
	//-----------------------------------------------------------------------
	void LineEmitter::setEnd(const Ogre::Vector3& end)
	{
		mEnd = end;
		_mScaledEnd = mEnd * _mEmitterScale;
		mLength = mEnd.length();
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineEmitter::getMaxIncrement(void) const
	{
		return mMaxIncrement;
	}
	//-----------------------------------------------------------------------
	void LineEmitter::setMaxIncrement(Ogre::Real maxIncrement)
	{
		mMaxIncrement = maxIncrement;
		_mScaledMaxIncrement = mMaxIncrement * _mEmitterScale.length();
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineEmitter::getMinIncrement(void) const
	{
		return mMinIncrement;
	}
	//-----------------------------------------------------------------------
	void LineEmitter::setMinIncrement(Ogre::Real minIncrement)
	{
		mMinIncrement = minIncrement;
		_mScaledMinIncrement = mMinIncrement * _mEmitterScale.length();
	}
	//-----------------------------------------------------------------------
	Ogre::Real LineEmitter::getMaxDeviation(void) const
	{
		return mMaxDeviation;
	}
	//-----------------------------------------------------------------------
	void LineEmitter::setMaxDeviation(Ogre::Real maxDeviation)
	{
		mMaxDeviation = maxDeviation;
		_mScaledMaxDeviation = mMaxDeviation * _mEmitterScale.length();
	}
	//-----------------------------------------------------------------------
	void LineEmitter::_initParticlePosition(Particle* particle)
	{
		// Remark: Don´t take the orientation of the node into account. The mEnd position is leading.
		if (mAutoDirection || (_mScaledMaxDeviation > 0.0f && !mFirst))
		{
			// Generate a random vector perpendicular on the line if this is required
			mPerpendicular = mEnd.crossProduct(Ogre::Vector3(Ogre::Math::RangeRandom(-1, 1), 
				Ogre::Math::RangeRandom(-1, 1), 
				Ogre::Math::RangeRandom(-1, 1)));
			mPerpendicular.normalise();
		}

		// If mMaxIncrement has been set, the particle emission follows a trajectory path along the line
		Ogre::Real fraction = 0.0f;
		if (_mScaledMaxIncrement > 0.0f)
		{
			if (!mFirst)
			{
				mIncrement += (_mScaledMinIncrement + Ogre::Math::UnitRandom() * _mScaledMaxIncrement);
				if (mIncrement >= _mScaledLength)
				{
					mIncrementsLeft = false;
				}
				fraction = mIncrement / _mScaledLength;
			}
		}
		else
		{
			fraction = Ogre::Math::UnitRandom();
		}

		// If the deviation has been set, generate a position with a certain distance from the line
		getDerivedPosition();
		if (_mScaledMaxDeviation > 0.0f && mIncrementsLeft)
		{
			if (!mFirst)
			{
				Ogre::Vector3 basePosition = mDerivedPosition + fraction * _mScaledEnd;
				particle->position = basePosition + _mScaledMaxDeviation * Ogre::Math::UnitRandom() * mPerpendicular;
				particle->originalPosition = basePosition;	// Position is without deviation from the line,
															// to make affectors a bit faster/easier.
			}
			else
			{
				particle->position = mDerivedPosition;
				particle->originalPosition = particle->position;
			}
		}
		else
		{
			particle->position = mDerivedPosition + fraction * _mScaledEnd;
			particle->originalPosition = particle->position;
		}

		mFirst = false;
	}
	//-----------------------------------------------------------------------
    void LineEmitter::_initParticleDirection(Particle* particle)
    {
		if (mAutoDirection)
		{
			Ogre::Radian angle;
			_generateAngle(angle);
			if (angle != Ogre::Radian(0))
			{
				particle->direction = mPerpendicular.randomDeviant(angle, mUpVector);
				particle->originalDirection = particle->direction;
			}
			else
			{
				particle->direction = mPerpendicular;
				particle->originalDirection = particle->direction;
			}
		}
		else
		{
			// Use the standard way
			ParticleEmitter::_initParticleDirection(particle);
		}
    }
	//-----------------------------------------------------------------------
	void LineEmitter::copyAttributesTo (ParticleEmitter* emitter)
	{
		ParticleEmitter::copyAttributesTo(emitter);

		LineEmitter* lineEmitter = static_cast<LineEmitter*>(emitter);
		lineEmitter->setEnd(mEnd); // Force calculation of the length
		lineEmitter->mLength = mLength;
		lineEmitter->setMinIncrement(mMinIncrement);
		lineEmitter->setMaxIncrement(mMaxIncrement);
		lineEmitter->setMaxDeviation(mMaxDeviation);
	}

}
