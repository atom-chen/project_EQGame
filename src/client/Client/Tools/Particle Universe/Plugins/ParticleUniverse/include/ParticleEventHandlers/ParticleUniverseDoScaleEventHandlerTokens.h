/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_SCALE_EVENT_HANDLER_TOKENS_H__
#define __PU_DO_SCALE_EVENT_HANDLER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEventHandlerTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a DoScaleEventHandler level.
    */
	class _ParticleUniverseExport DoScaleEventHandlerTokens : public ParticleEventHandlerTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_SCALE_FRACTION;
			static const Ogre::String KEYWORD_SCALE_TYPE;
			static const Ogre::String KEYWORD_SCALE_TIME_TO_LIVE;
			static const Ogre::String KEYWORD_SCALE_VELOCITY;

			DoScaleEventHandlerTokens(void) {};
			virtual ~DoScaleEventHandlerTokens(void) {};

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
			class ScaleFractionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleFractionToken;
			class ScaleTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleTypeToken;
	};

}
#endif
