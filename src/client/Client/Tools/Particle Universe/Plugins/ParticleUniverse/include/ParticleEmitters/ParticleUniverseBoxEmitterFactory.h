/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BOX_EMITTER_FACTORY_H__
#define __PU_BOX_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterFactory.h"
#include "ParticleUniverseBoxEmitter.h"
#include "ParticleUniverseBoxEmitterTokens.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a BoxEmitter.
    */
	class _ParticleUniverseExport BoxEmitterFactory : public ParticleEmitterFactory
	{
		protected:
			BoxEmitterTokens mBoxEmitterTokens;

		public:
			BoxEmitterFactory(void) {};
	        virtual ~BoxEmitterFactory(void) {};

			/** See ParticleEmitterFactory */
			Ogre::String getEmitterType() const
			{
				return "Box";
			}

			/** See ParticleEmitterFactory */
			ParticleEmitter* createEmitter(void)
			{
				return _createEmitter<BoxEmitter>();
			}

			/** See ParticleEmitterFactory
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mBoxEmitterTokens
				mBoxEmitterTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleEmitterFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mBoxEmitterTokens
				mBoxEmitterTokens.write(serializer, element);
			}

	};

}
#endif
