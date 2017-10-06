/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ENTITY_RENDERER_FACTORY_H__
#define __PU_ENTITY_RENDERER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererFactory.h"
#include "ParticleUniverseEntityRenderer.h"
#include "ParticleUniverseEntityRendererTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating a EntityRenderer.
    */
	class _ParticleUniverseExport EntityRendererFactory : public ParticleRendererFactory
	{
		public:
			EntityRendererFactory(void) {};
	        virtual ~EntityRendererFactory(void) {};

			/** See ParticleRendererFactory */
			Ogre::String getRendererType() const
			{
				return "Entity";
			}

			/** See ParticleRendererFactory */
			ParticleRenderer* createRenderer(void)
			{
				return _createRenderer<EntityRenderer>();
			}

			/** See ParticleRendererFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mEntityRendererTokens
				mEntityRendererTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleRendererFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mEntityRendererTokens
				mEntityRendererTokens.write(serializer, element);
			}


		protected:
			EntityRendererTokens mEntityRendererTokens;
	};

}
#endif
