/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RENDERER_FACTORY_H__
#define __PU_RENDERER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRenderer.h"
#include "ParticleUniverseRendererTokens.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This is the base factory of all ParticleRenderer implementations.
    */
	class _ParticleUniverseExport ParticleRendererFactory : public ITokenBind, public Ogre::FactoryAlloc
	{
	    protected:

			/** 
	        */
			template <class T>
			ParticleRenderer* _createRenderer(void)
			{
				ParticleRenderer* particleRenderer = OGRE_NEW_T(T, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
				particleRenderer->setRendererType(getRendererType());
				return particleRenderer;
			};

	public:
			ParticleRendererFactory(void){};
			virtual ~ParticleRendererFactory(void){};

		    /** Returns the type of the factory, which identifies the particle Renderer type this factory creates. */
			virtual Ogre::String getRendererType() const = 0;

			/** Creates a new renderer instance.
		    @remarks
	        */
		    virtual ParticleRenderer* createRenderer(void) = 0;

			/** Delete a renderer.
	        */
			void destroyRenderer (ParticleRenderer* renderer)
			{
				if (renderer)
					OGRE_DELETE_T(renderer, ParticleRenderer, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			};
	};

}
#endif
