/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_ENABLE_COMPONENT_EVENT_HANDLER_TOKENS_H__
#define __PU_DO_ENABLE_COMPONENT_EVENT_HANDLER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEventHandlerTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a DoEnableComponentEventHandler level.
    */
	class _ParticleUniverseExport DoEnableComponentEventHandlerTokens : public ParticleEventHandlerTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_ENABLE_COMPONENT;
			static const Ogre::String KEYWORD_EMITTER_COMPONENT;
			static const Ogre::String KEYWORD_AFFECTOR_COMPONENT;
			static const Ogre::String KEYWORD_TECHNIQUE_COMPONENT;
			static const Ogre::String KEYWORD_OBSERVER_COMPONENT;

			DoEnableComponentEventHandlerTokens(void) {};
			virtual ~DoEnableComponentEventHandlerTokens(void) {};

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
			class EnableComponentToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEnableComponentToken;
	};

}
#endif
