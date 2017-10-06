/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BOX_EMITTER_TOKEN_H__
#define __PU_BOX_EMITTER_TOKEN_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the BoxEmitter.
    */
	class _ParticleUniverseExport BoxEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_HEIGHT;
			static const Ogre::String KEYWORD_WIDTH;
			static const Ogre::String KEYWORD_DEPTH;

			BoxEmitterTokens(void) {};
			virtual ~BoxEmitterTokens(void) {};

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
			class HeightToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mHeightToken;
			class WidthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mWidthToken;
			class DepthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDepthToken;
	};

}
#endif
