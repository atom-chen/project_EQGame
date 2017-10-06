/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_POSITION_EMITTER_TOKEN_H__
#define __PU_POSITION_EMITTER_TOKEN_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the PositionEmitter.
    */
	class _ParticleUniverseExport PositionEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_POSITION;
			static const Ogre::String KEYWORD_RANDOMIZE;

			PositionEmitterTokens(void) {};
			virtual ~PositionEmitterTokens(void) {};

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
			class RandomizedToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRandomizedToken;
			class PositionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPositionToken;
	};

}
#endif
