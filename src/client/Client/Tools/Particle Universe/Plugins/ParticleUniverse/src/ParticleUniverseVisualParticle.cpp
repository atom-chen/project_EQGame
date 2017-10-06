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
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	VisualParticle::VisualParticle(void) : 
		Particle(),
		colour(Ogre::ColourValue::White),
		originalColour(Ogre::ColourValue::White),
		zRotation(Ogre::Radian(0)),
		zRotationSpeed(Ogre::Radian(0)),
		orientation(Ogre::Quaternion::IDENTITY),
		originalOrientation(Ogre::Quaternion::IDENTITY),
		rotationSpeed(0.0f),
		ownDimensions(false),
		width(1.0f),
		height(1.0f),
		depth(1.0f),
		radius(0.87f),
		textureAnimationTimeStep(0.1f),
		textureAnimationTimeStepCount(0.0f),
		textureCoordsCurrent(0),
		textureAnimationDirectionUp(true)
	{
		particleType = PT_VISUAL;
		mMarkedForEmission  = true; // Default is false, but visual particles are always emitted.
	}
	//-----------------------------------------------------------------------
	void VisualParticle::setOwnDimensions(Ogre::Real newWidth, Ogre::Real newHeight, Ogre::Real newDepth)
	{
		ownDimensions = true;
		if (newWidth)
			width = newWidth;
		if (newHeight)
			height = newHeight;
		if (newDepth)
			depth = newDepth;
		_calculateBoundingSphereRadius();
		parentEmitter->getParentTechnique()->_notifyParticleResized();
	}
	//-----------------------------------------------------------------------
	void VisualParticle::_initForEmission(void)
	{
		Particle::_initForEmission();
		textureAnimationTimeStep = 0.1f;
		textureAnimationTimeStepCount = 0.0f;
		textureCoordsCurrent = 0.0f;
		textureAnimationDirectionUp = true;
	}
	//-----------------------------------------------------------------------
	void VisualParticle::_initForExpiration(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		Particle::_initForExpiration(technique, timeElapsed);
		if (visualData)
		{
			visualData->setVisible(false);
		}
	}
	//-----------------------------------------------------------------------
	inline void VisualParticle::_calculateBoundingSphereRadius(void)
	{
		//radius = 0.5 * Ogre::Math::Sqrt(width*width + height*height + depth*depth);
		radius = 0.5 * std::max(depth, std::max(width, height)); // approximation
	}

}
