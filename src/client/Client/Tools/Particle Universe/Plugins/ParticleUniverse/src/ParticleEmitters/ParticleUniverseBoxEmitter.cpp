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

#include "ParticleEmitters/ParticleUniverseBoxEmitter.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real BoxEmitter::DEFAULT_WIDTH = 100.0f;
	const Ogre::Real BoxEmitter::DEFAULT_HEIGHT = 100.0f;
	const Ogre::Real BoxEmitter::DEFAULT_DEPTH = 100.0f;

	//-----------------------------------------------------------------------
	BoxEmitter::BoxEmitter(void) : 
		ParticleEmitter(),
		mWidth(DEFAULT_WIDTH),
		mHeight(DEFAULT_HEIGHT),
		mDepth(DEFAULT_DEPTH),
		mXRange(0.5 * DEFAULT_WIDTH),
		mYRange(0.5 * DEFAULT_HEIGHT),
		mZRange(0.5 * DEFAULT_DEPTH)
	{
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxEmitter::getHeight(void) const
	{
		return mHeight;
	}
	//-----------------------------------------------------------------------
	void BoxEmitter::setHeight(const Ogre::Real height)
	{
		mHeight = height;
		mYRange = 0.5 * height;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxEmitter::getWidth(void) const
	{
		return mWidth;
	}
	//-----------------------------------------------------------------------
	void BoxEmitter::setWidth(const Ogre::Real width)
	{
		mWidth = width;
		mXRange = 0.5 * width;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxEmitter::getDepth(void) const
	{
		return mDepth;
	}
	//-----------------------------------------------------------------------
	void BoxEmitter::setDepth(const Ogre::Real depth)
	{
		mDepth = depth;
		mZRange = 0.5 * depth;
	}
	//-----------------------------------------------------------------------
	void BoxEmitter::_initParticlePosition(Particle* particle)
	{
		ParticleSystem* sys = mParentTechnique->getParentSystem();
		if (sys)
		{
			particle->position = getDerivedPosition() + 
				sys->getDerivedOrientation() *
				(_mEmitterScale *
				Ogre::Vector3(Ogre::Math::SymmetricRandom() * mXRange,
					Ogre::Math::SymmetricRandom() * mYRange,
					Ogre::Math::SymmetricRandom() * mZRange));
		}
		else
		{
			particle->position = getDerivedPosition() + 
				_mEmitterScale *
				Ogre::Vector3(Ogre::Math::SymmetricRandom() * mXRange,
					Ogre::Math::SymmetricRandom() * mYRange,
					Ogre::Math::SymmetricRandom() * mZRange);
		}

		particle->originalPosition = particle->position;
	}
	//-----------------------------------------------------------------------
	void BoxEmitter::copyAttributesTo (ParticleEmitter* emitter)
	{
		ParticleEmitter::copyAttributesTo(emitter);

		BoxEmitter* boxEmitter = static_cast<BoxEmitter*>(emitter);
		boxEmitter->mHeight = mHeight;
		boxEmitter->mWidth = mWidth;
		boxEmitter->mDepth = mDepth;
		boxEmitter->mXRange = mXRange;
		boxEmitter->mYRange = mYRange;
		boxEmitter->mZRange = mZRange;
	}
}
