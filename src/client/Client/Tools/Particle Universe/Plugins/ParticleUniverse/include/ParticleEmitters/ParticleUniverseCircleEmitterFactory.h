/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_CIRCLE_EMITTER_FACTORY_H__
#define __PU_CIRCLE_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterFactory.h"
#include "ParticleUniverseCircleEmitter.h"
#include "ParticleUniverseCircleEmitterTokens.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a CircleEmitter.
    */
	class _ParticleUniverseExport CircleEmitterFactory : public ParticleEmitterFactory
	{
		protected:
			CircleEmitterTokens mCircleEmitterTokens;

		public:
			CircleEmitterFactory(void) {};
	        virtual ~CircleEmitterFactory(void) {};

			/** See ParticleEmitterFactory */
			Ogre::String getEmitterType() const
			{
				return "Circle";
			}

			/** See ParticleEmitterFactory */
			ParticleEmitter* createEmitter(void)
			{
				return _createEmitter<CircleEmitter>();
			}

			/** See ParticleEmitterFactory
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mCircleEmitterTokens
				mCircleEmitterTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleEmitterFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mCircleEmitterTokens
				mCircleEmitterTokens.write(serializer, element);
			}
	};

}
#endif
