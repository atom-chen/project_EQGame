/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TEXTURE_ANIMATION_TOKENS_H__
#define __PU_TEXTURE_ANIMATION_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the TextureAnimator.
    */
	class _ParticleUniverseExport TextureAnimatorTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_TIME_STEP;
			static const Ogre::String KEYWORD_TEXCOORDS_START;
			static const Ogre::String KEYWORD_TEXCOORDS_END;
			static const Ogre::String KEYWORD_ANIMATION_TYPE;
			static const Ogre::String KEYWORD_START_RANDOM;
			static const Ogre::String KEYWORD_LOOP;
			static const Ogre::String KEYWORD_UP_DOWN;
			static const Ogre::String KEYWORD_RANDOM;

			TextureAnimatorTokens(void) {};
			virtual ~TextureAnimatorTokens(void) {};

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
			class TimeStepTextureAnimationToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTimeStepTextureAnimationToken;
			class TextureCoordsStartToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsStartToken;
			class TextureCoordsEndToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsEndToken;
			class TextureAnimationTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureAnimationTypeToken;
			class TextureStartRandomToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureStartRandomToken;
	};

}
#endif
