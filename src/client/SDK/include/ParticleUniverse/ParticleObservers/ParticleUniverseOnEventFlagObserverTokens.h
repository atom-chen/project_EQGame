/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONEVENTFLAG_OBSERVER_TOKENS_H__
#define __PU_ONEVENTFLAG_OBSERVER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the OnEventFlagObserver.
    */
	class _ParticleUniverseExport OnEventFlagObserverTokens : public ParticleObserverTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_EVENT_FLAG;

			OnEventFlagObserverTokens(void) {};
			virtual ~OnEventFlagObserverTokens(void) {};

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
			class EventFlagToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEventFlagToken;
	};

}
#endif
