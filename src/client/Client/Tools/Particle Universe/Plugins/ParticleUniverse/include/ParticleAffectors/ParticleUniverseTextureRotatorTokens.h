/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TEXTURE_ROTATOR_TOKENS_H__
#define __PU_TEXTURE_ROTATOR_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the TextureRotator.
    */
	class _ParticleUniverseExport TextureRotatorTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_USE_OWN_ROTATION;
			static const Ogre::String KEYWORD_ROTATION_SPEED;
			static const Ogre::String KEYWORD_ROTATION;

			TextureRotatorTokens(void) {};
			virtual ~TextureRotatorTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);

		protected:
			/** Tokens which get called during parsing.
			*/
			class UseOwnRotationSpeedToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mUseOwnRotationSpeedToken;
			class Rotation : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRotation;
			class RotationSpeed : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRotationSpeed;
	};

}
#endif
