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

#include "Externs/ParticleUniverseVortexExtern.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void VortexExtern::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		if (isAttached())
		{
			position = getParentNode()->_getDerivedPosition();
			mDerivedPosition = position;
		}
	}
	//-----------------------------------------------------------------------
	void VortexExtern::_interface(ParticleTechnique* technique, 
		Particle* particle, 
		Ogre::Real timeElapsed)
	{
		// Setting the distance threshold is mandatory
		if (isAttached() && mDistanceThresholdSet)
		{
			Ogre::Real distance = mDerivedPosition.distance(particle->position);
			if (distance > mDistanceThreshold)
			{
				return;
			}
			else
			{
				// Rotate position, direction and orientation (visible particle only) based on distance between particle
				// and VortexExtern
				Ogre::Real scalar = 1 - (distance / mDistanceThreshold);
				if (mParentNode)
				{
					mRotation.FromAngleAxis((_calculateRotationSpeed() * timeElapsed * scalar), mParentNode->_getDerivedOrientation() * mRotationVector);
				}
				else
				{
					mRotation.FromAngleAxis((_calculateRotationSpeed() * timeElapsed * scalar), mRotationVector);
				}
				_affect(technique, particle, timeElapsed);
			}
		}
	}
	//-----------------------------------------------------------------------
	void VortexExtern::copyAttributesTo (Extern* externObject)
	{
		Attachable::copyAttributesTo(externObject);
		VortexExtern* vortexExtern = static_cast<VortexExtern*>(externObject);
		VortexAffector* vortexAffector = static_cast<VortexAffector*>(vortexExtern);
		VortexAffector::copyAttributesTo(vortexAffector);
	}
}
