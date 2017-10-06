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

#include "ParticleEmitters/ParticleUniverseCircleEmitter.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real CircleEmitter::DEFAULT_RADIUS = 100.0f;
	const Ogre::Real CircleEmitter::DEFAULT_STEP = 0.1f;
	const Ogre::Real CircleEmitter::DEFAULT_ANGLE = 0.0f;
	const bool CircleEmitter::DEFAULT_RANDOM = true;
	const Ogre::Vector3 CircleEmitter::DEFAULT_NORMAL = Ogre::Vector3::ZERO;

	//-----------------------------------------------------------------------
	CircleEmitter::CircleEmitter(void) : 
		ParticleEmitter(),
		mRadius(DEFAULT_RADIUS),
		mCircleAngle(DEFAULT_ANGLE),
		mOriginalCircleAngle(DEFAULT_ANGLE),
		mStep(DEFAULT_STEP),
		mRandom(DEFAULT_RANDOM),
		mOrientation(Ogre::Quaternion::IDENTITY),
		mNormal(DEFAULT_NORMAL),
		mX(0.0f),
		mZ(0.0f)
	{
	}
	//-----------------------------------------------------------------------
	const Ogre::Real CircleEmitter::getRadius(void) const
	{
		return mRadius;
	}
	//-----------------------------------------------------------------------
	void CircleEmitter::setRadius(const Ogre::Real radius)
	{
		mRadius = radius;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real CircleEmitter::getCircleAngle(void) const
	{
		return mOriginalCircleAngle;
	}
	//-----------------------------------------------------------------------
	void CircleEmitter::setCircleAngle(const Ogre::Real circleAngle)
	{
		mOriginalCircleAngle = circleAngle;
		mCircleAngle = circleAngle;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real CircleEmitter::getStep(void) const
	{
		return mStep;
	}
	//-----------------------------------------------------------------------
	void CircleEmitter::setStep(const Ogre::Real step)
	{
		mStep = step;
	}
	//-----------------------------------------------------------------------
	const bool CircleEmitter::isRandom(void) const
	{
		return mRandom;
	}
	//-----------------------------------------------------------------------
	void CircleEmitter::setRandom(const bool random)
	{
		mRandom = random;
	}
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& CircleEmitter::getOrientation(void) const
	{
		return mOrientation;
	}
	//----------------------------------------------------------------------- 
	const Ogre::Vector3& CircleEmitter::getNormal(void) const
	{ 
		return mNormal;
	} 
	//----------------------------------------------------------------------- 
	void CircleEmitter::setNormal(const Ogre::Vector3& normal) 
	{ 
		mOrientation = Ogre::Vector3::UNIT_Y.getRotationTo(normal, Ogre::Vector3::UNIT_X);
		mNormal = normal;
	}
	//-----------------------------------------------------------------------
	void CircleEmitter::_notifyStart (void)
	{
		// Reset the attributes to allow a restart.
		ParticleEmitter::_notifyStart();
		mCircleAngle = mOriginalCircleAngle;
	}
	//----------------------------------------------------------------------- 
	void CircleEmitter::_initParticlePosition(Particle* particle)
	{
		Ogre::Real angle = 0;
		if (mRandom)
		{
			// Choose a random position on the circle.
			angle = Ogre::Math::RangeRandom(0, Ogre::Math::TWO_PI);
		}
		else
		{
			// Follow the contour of the circle.
			mCircleAngle += mStep;
			mCircleAngle = mCircleAngle > Ogre::Math::TWO_PI ? mCircleAngle - Ogre::Math::TWO_PI : mCircleAngle;
			angle = mCircleAngle;
		}

		mX = Ogre::Math::Cos(angle);
		mZ = Ogre::Math::Sin(angle);
		ParticleSystem* sys = mParentTechnique->getParentSystem();
		if (sys)
		{
			// Take both orientation of the node and its own orientation, based on the normal, into account
			particle->position = getDerivedPosition() + 
				sys->getDerivedOrientation() * mOrientation * (_mEmitterScale * Ogre::Vector3(mX * mRadius, 0, mZ * mRadius));
		}
		else
		{
			particle->position = getDerivedPosition() + _mEmitterScale * ( mOrientation * Ogre::Vector3(mX * mRadius, 0, mZ * mRadius) );
		}
		particle->originalPosition = particle->position;
	}
	//-----------------------------------------------------------------------
    void CircleEmitter::_initParticleDirection(Particle* particle)
    {
		if (mAutoDirection)
		{
			// The value of the direction vector that has been set does not have a meaning for
			// the circle emitter.
			Ogre::Radian angle;
			_generateAngle(angle);
			if (angle != Ogre::Radian(0))
			{
				particle->direction = (mOrientation * Ogre::Vector3(mX, 0, mZ) ).randomDeviant(angle, mUpVector);
				particle->originalDirection = particle->direction;
			}
			else
			{
				particle->direction = Ogre::Vector3(mX, 0, mZ);
				particle->direction = mOrientation * Ogre::Vector3(mX, 0, mZ);
			}
		}
		else
		{
			// Use the standard way
			ParticleEmitter::_initParticleDirection(particle);
		}
    }
	//-----------------------------------------------------------------------
	void CircleEmitter::copyAttributesTo (ParticleEmitter* emitter)
	{
		ParticleEmitter::copyAttributesTo(emitter);

		CircleEmitter* circleEmitter = static_cast<CircleEmitter*>(emitter);
		circleEmitter->mRadius = mRadius;
		circleEmitter->mCircleAngle = mCircleAngle;
		circleEmitter->mOriginalCircleAngle = mOriginalCircleAngle;
		circleEmitter->mStep = mStep;
		circleEmitter->mRandom = mRandom;
		circleEmitter->mNormal = mNormal;
		circleEmitter->mOrientation = mOrientation; 
	}
}
