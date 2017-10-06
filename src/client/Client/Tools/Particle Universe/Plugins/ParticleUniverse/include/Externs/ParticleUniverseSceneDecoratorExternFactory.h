/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SCENE_DECORATOR_FACTORY_H__
#define __PU_SCENE_DECORATOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniverseSceneDecoratorExtern.h"
#include "ParticleUniverseSceneDecoratorExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the SceneDecoratorExtern.
    */
	class _ParticleUniverseExport SceneDecoratorExternFactory : public ExternFactory
	{
		public:
			SceneDecoratorExternFactory(void) : 
				ExternFactory(){};
	        virtual ~SceneDecoratorExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "SceneDecorator";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<SceneDecoratorExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mSceneDecoratorExternTokens
				mSceneDecoratorExternTokens.setupTokenDefinitions(tokenRegister);
			};

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mSceneDecoratorExternTokens
				mSceneDecoratorExternTokens.write(serializer, element);
			}


		protected:
			SceneDecoratorExternTokens mSceneDecoratorExternTokens;
	};

}
#endif
