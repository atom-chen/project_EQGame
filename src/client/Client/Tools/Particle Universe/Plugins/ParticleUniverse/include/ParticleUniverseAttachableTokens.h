/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ATTACHABLE_TOKENS_H__
#define __PU_ATTACHABLE_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a Attachable level.
    */
	class _ParticleUniverseExport AttachableTokens : public ITokenBind
	{
		public:
			static const Ogre::String KEYWORD_DISTANCE_THRESHOLD;
			
			AttachableTokens(void) {};
			virtual ~AttachableTokens(void) {};

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
			class DistanceThresholdToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDistanceThresholdToken;
	};

}
#endif
