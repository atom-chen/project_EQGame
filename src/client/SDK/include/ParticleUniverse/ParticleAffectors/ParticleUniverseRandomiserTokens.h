/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RANDOMISER_TOKENS_H__
#define __PU_RANDOMISER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the Randomiser.
    */
	class _ParticleUniverseExport RandomiserTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_MAX_DEVIATION_X;
			static const Ogre::String KEYWORD_MAX_DEVIATION_Y;
			static const Ogre::String KEYWORD_MAX_DEVIATION_Z;
			static const Ogre::String KEYWORD_TIME_STEP;
			static const Ogre::String KEYWORD_RANDOM_DIRECTION;

			RandomiserTokens(void) {};
			virtual ~RandomiserTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);

		protected:
			/** Tokens which get called during parsing.
			*/
			class DeviationXToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDeviationXToken;
			class DeviationYToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDeviationYToken;
			class DeviationZToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDeviationZToken;
			class RandomDirectionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRandomDirectionToken;
			class TimeStepToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTimeStepToken;
	};

}
#endif
