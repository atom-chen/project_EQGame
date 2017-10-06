/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_AFFECTOR_EVENT_HANDLER_TOKENS_H__
#define __PU_DO_AFFECTOR_EVENT_HANDLER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEventHandlerTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a DoAffectorEventHandler level.
    */
	class _ParticleUniverseExport DoAffectorEventHandlerTokens : public ParticleEventHandlerTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_FORCE_AFFECTOR;
			static const Ogre::String KEYWORD_PRE_POST;

			DoAffectorEventHandlerTokens(void) {};
			virtual ~DoAffectorEventHandlerTokens(void) {};

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
			class AffectorToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAffectorToken;
			class PrePostToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPrePostToken;
	};

}
#endif
