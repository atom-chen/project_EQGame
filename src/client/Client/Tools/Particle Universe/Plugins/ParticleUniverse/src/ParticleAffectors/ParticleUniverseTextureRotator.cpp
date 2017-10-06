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

#include "ParticleUniverseVisualParticle.h"
#include "ParticleAffectors/ParticleUniverseTextureRotator.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const bool TextureRotator::DEFAULT_USE_OWN_SPEED = false;
	const Ogre::Real TextureRotator::DEFAULT_ROTATION_SPEED = 10.0f;
	const Ogre::Real TextureRotator::DEFAULT_ROTATION = 0.0f;

	//-----------------------------------------------------------------------
	TextureRotator::TextureRotator(void) : 
		ParticleAffector(),
		mUseOwnRotationSpeed(DEFAULT_USE_OWN_SPEED),
		mScaledRotationSpeed(Ogre::Radian(0)),
		twoPiRad(Ogre::Radian(Ogre::Math::TWO_PI))
	{
		mDynRotation = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynRotation))->setValue(DEFAULT_ROTATION);
		mDynRotationSpeed = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynRotationSpeed))->setValue(DEFAULT_ROTATION_SPEED);
	}
	//-----------------------------------------------------------------------
	TextureRotator::~TextureRotator(void)
	{
		if (mDynRotation)
		{
			OGRE_DELETE_T(mDynRotation, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynRotation = 0;
		}

		if (mDynRotationSpeed)
		{
			OGRE_DELETE_T(mDynRotationSpeed, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mDynRotationSpeed = 0;
		}
	}
	//-----------------------------------------------------------------------
	bool TextureRotator::useOwnRotationSpeed (void) const
	{
		return mUseOwnRotationSpeed;
	}
	//-----------------------------------------------------------------------
	void TextureRotator::setUseOwnRotationSpeed (bool useOwnRotationSpeed)
	{
		mUseOwnRotationSpeed = useOwnRotationSpeed;
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* TextureRotator::getRotation(void) const
	{
		return mDynRotation;
	}
	//-----------------------------------------------------------------------
	void TextureRotator::setRotation(DynamicAttribute* dynRotation)
	{
		if (mDynRotation)
			OGRE_DELETE_T(mDynRotation, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynRotation = dynRotation;
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* TextureRotator::getRotationSpeed(void) const
	{
		return mDynRotationSpeed;
	}
	//-----------------------------------------------------------------------
	void TextureRotator::setRotationSpeed(DynamicAttribute* dynRotationSpeed)
	{
		if (mDynRotationSpeed)
			OGRE_DELETE_T(mDynRotationSpeed, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynRotationSpeed = dynRotationSpeed;
	}
	//-----------------------------------------------------------------------
	Ogre::Radian TextureRotator::_calculateRotation(void)
	{
		return Ogre::Radian(mDynamicAttributeHelper.calculate(mDynRotation, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart()));
	}
	//-----------------------------------------------------------------------
	Ogre::Radian TextureRotator::_calculateRotationSpeed(Particle* particle)
	{
		return Ogre::Radian(mDynamicAttributeHelper.calculate(mDynRotationSpeed, particle->timeFraction));
	}
	//-----------------------------------------------------------------------
	void TextureRotator::_initParticleForEmission(Particle* particle)
	{
		// Only continue if the particle is a visual particle
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		// Set initial random zRotation
		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		visualParticle->zRotation = _calculateRotation();
		if (particle->parentEmitter->getParentTechnique()->getRenderer())
		{
			// Assume that all parents exist. That must be the case otherwise particles are not emitted.
			particle->parentEmitter->getParentTechnique()->getRenderer()->_notifyParticleZRotated();
		}

		// Set the zRotationSpeed
		visualParticle->zRotationSpeed = _calculateRotationSpeed(particle);
	}
	//-----------------------------------------------------------------------
	void TextureRotator::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		// Only continue if the particle is a visual particle
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		if (mUseOwnRotationSpeed)
		{
			// Use scaled rotationspeed and adjust the speed according to the velocity
			mScaledRotationSpeed = visualParticle->zRotationSpeed * timeElapsed;
		}
		else
		{
			// Scale speed
			mScaledRotationSpeed = _calculateRotationSpeed(particle) * timeElapsed;
		}

		visualParticle->zRotation += mScaledRotationSpeed;
		visualParticle->zRotation = visualParticle->zRotation > twoPiRad ? visualParticle->zRotation - twoPiRad : visualParticle->zRotation;
		if (particleTechnique->getRenderer())
		{
			particleTechnique->getRenderer()->_notifyParticleZRotated();
		}
	}
	//-----------------------------------------------------------------------
	void TextureRotator::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		TextureRotator* textureRotator = static_cast<TextureRotator*>(affector);
		textureRotator->setRotation(mDynamicAttributeFactory.cloneDynamicAttribute(getRotation()));
		textureRotator->setRotationSpeed(mDynamicAttributeFactory.cloneDynamicAttribute(getRotationSpeed()));
		textureRotator->mUseOwnRotationSpeed = mUseOwnRotationSpeed;
	}

}
