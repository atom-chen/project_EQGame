/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LINEAR_FORCE_AFFECTOR_H__
#define __PU_LINEAR_FORCE_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffector.h"

namespace ParticleUniverse
{
	/** Adds a force to particles.
    */
	class _ParticleUniverseExport LinearForceAffector : public ForceAffector
	{
		public:
			LinearForceAffector(void) : 
				ForceAffector()
			{
			};
	        virtual ~LinearForceAffector(void) {};

			/** 
	        */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** 
	        */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed);

			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

		protected:
	};

}
#endif
