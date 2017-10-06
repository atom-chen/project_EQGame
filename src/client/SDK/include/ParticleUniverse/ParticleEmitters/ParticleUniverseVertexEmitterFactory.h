/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VERTEX_EMITTER_FACTORY_H__
#define __PU_VERTEX_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterFactory.h"
#include "ParticleUniverseVertexEmitterTokens.h"
#include "ParticleUniverseVertexEmitter.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a VertexEmitter.
    */
	class _ParticleUniverseExport VertexEmitterFactory : public ParticleEmitterFactory
	{
		protected:
			VertexEmitterTokens mVertexEmitterTokens;

		public:
			VertexEmitterFactory(void) {};
	        virtual ~VertexEmitterFactory(void) {};

			/** See ParticleEmitterFactory */
			Ogre::String getEmitterType() const
			{
				return "Vertex";
			}

			/** See ParticleEmitterFactory */
			ParticleEmitter* createEmitter(void)
			{
				return _createEmitter<VertexEmitter>();
			}

			/** See ParticleEmitterFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mVertexEmitterTokens
				mVertexEmitterTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleEmitterFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mVertexEmitterTokens
				mVertexEmitterTokens.write(serializer, element);
			}
	};

}
#endif
