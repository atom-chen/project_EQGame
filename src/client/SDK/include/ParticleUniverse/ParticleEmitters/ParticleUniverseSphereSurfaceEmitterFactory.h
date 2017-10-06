/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SPHERE_SURFACE_EMITTER_FACTORY_H__
#define __PU_SPHERE_SURFACE_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterFactory.h"
#include "ParticleUniverseSphereSurfaceEmitterTokens.h"
#include "ParticleUniverseSphereSurfaceEmitter.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a SphereSurfaceEmitter.
    */
	class _ParticleUniverseExport SphereSurfaceEmitterFactory : public ParticleEmitterFactory
	{
		protected:
			SphereSurfaceEmitterTokens mSphereSurfaceEmitterTokens;

		public:
			SphereSurfaceEmitterFactory(void) {};
	        virtual ~SphereSurfaceEmitterFactory(void) {};

			/** See ParticleEmitterFactory */
			Ogre::String getEmitterType() const
			{
				return "SphereSurface";
			}

			/** See ParticleEmitterFactory */
			ParticleEmitter* createEmitter(void)
			{
				return _createEmitter<SphereSurfaceEmitter>();
			}

			/** See ParticleEmitterFactory
			@remarks
				The Point emitter doesn't have other attributes than the base attributes */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mSphereSurfaceEmitterTokens
				mSphereSurfaceEmitterTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleEmitterFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mSphereSurfaceEmitterTokens
				mSphereSurfaceEmitterTokens.write(serializer, element);
			}
	};

}
#endif
