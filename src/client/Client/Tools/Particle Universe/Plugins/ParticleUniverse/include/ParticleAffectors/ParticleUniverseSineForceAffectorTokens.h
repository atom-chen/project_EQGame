/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SINE_FORCE_AFFECTOR_TOKENS_H__
#define __PU_SINE_FORCE_AFFECTOR_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the SineForceAffector.
    */
	class _ParticleUniverseExport SineForceAffectorTokens : public ForceAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_MIN_FREQUENCY;
			static const Ogre::String KEYWORD_MAX_FREQUENCY;

			SineForceAffectorTokens(void) {};
			virtual ~SineForceAffectorTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);

		protected:
			// Tokens which get called during parsing.
			class FrequencyMinToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mFrequencyMinToken;
			class FrequencyMaxToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mFrequencyMaxToken;
	};

}
#endif
