/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LINE_EMITTER_TOKEN_H__
#define __PU_LINE_EMITTER_TOKEN_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the LineEmitter.
    */
	class _ParticleUniverseExport LineEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_END;
			static const Ogre::String KEYWORD_MAX_INCREMENT;
			static const Ogre::String KEYWORD_MIN_INCREMENT;
			static const Ogre::String KEYWORD_MAX_DEVIATION;

			LineEmitterTokens(void) {};
			virtual ~LineEmitterTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);

		protected:
			// Tokens which get called during parsing.
			class LineEndToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mLineEndToken;
			class MaxIncrementToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaxIncrementToken;
			class MinIncrementToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMinIncrementToken;
			class MaxDeviationToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaxDeviationToken;
	};

}
#endif