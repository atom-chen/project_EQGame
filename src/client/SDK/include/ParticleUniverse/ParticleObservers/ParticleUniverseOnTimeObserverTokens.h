/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONTIME_OBSERVER_TOKENS_H__
#define __PU_ONTIME_OBSERVER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the OnTimeObserver.
    */
	class _ParticleUniverseExport OnTimeObserverTokens : public ParticleObserverTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_ON_TIME;
			static const Ogre::String KEYWORD_SINCE_START_SYSTEM;

			OnTimeObserverTokens(void) {};
			virtual ~OnTimeObserverTokens(void) {};

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
			class TimeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTimeToken;
			class SinceStartSystemToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSinceStartSystemToken;
	};

}
#endif
