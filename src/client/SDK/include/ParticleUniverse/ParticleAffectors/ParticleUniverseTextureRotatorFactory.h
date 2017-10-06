/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TEXTURE_ROTATOR_FACTORY_H__
#define __PU_TEXTURE_ROTATOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseTextureRotatorTokens.h"
#include "ParticleUniverseTextureRotator.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the TextureRotator.
    */
	class _ParticleUniverseExport TextureRotatorFactory : public ParticleAffectorFactory
	{
		public:
			TextureRotatorFactory(void){};
	        virtual ~TextureRotatorFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "TextureRotator";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<TextureRotator>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mTextureRotatorTokens
				mTextureRotatorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mTextureRotatorTokens
				mTextureRotatorTokens.write(serializer, element);
			}

		protected:
			TextureRotatorTokens mTextureRotatorTokens;
	};

}
#endif
