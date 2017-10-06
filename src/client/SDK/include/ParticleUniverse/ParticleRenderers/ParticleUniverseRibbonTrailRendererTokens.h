/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RIBBONTRAIL_RENDERER_TOKENS_H__
#define __PU_RIBBONTRAIL_RENDERER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for a RibbonTrailRenderer.
    */
	class _ParticleUniverseExport RibbonTrailRendererTokens : public ParticleRendererTokens
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_VERTEX_COLOURS;
			static const Ogre::String KEYWORD_MAX_ELEMENTS;
			static const Ogre::String KEYWORD_LENGTH;
			static const Ogre::String KEYWORD_WIDTH;
			static const Ogre::String KEYWORD_RANDOM_INITIAL_COLOUR;
			static const Ogre::String KEYWORD_INITIAL_COLOUR;
			static const Ogre::String KEYWORD_COLOUR_CHANGE;

			RibbonTrailRendererTokens(void) {};
			virtual ~RibbonTrailRendererTokens(void) {};

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
			class VertexColoursToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mVertexColoursToken;
			class MaxChainElementsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaxChainElementsToken;
			class LengthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mLengthToken;
			class WidthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mWidthToken;
			class RandomInitialColourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRandomInitialColourToken;
			class InitialColourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mInitialColourToken;
			class ColourChangeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mColourChangeToken;
	};

}
#endif
