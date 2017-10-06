/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_CIRCLE_EMITTER_TOKEN_H__
#define __PU_CIRCLE_EMITTER_TOKEN_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the CircleEmitter.
    */
	class _ParticleUniverseExport CircleEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_RADIUS;
			static const Ogre::String KEYWORD_STEP;
			static const Ogre::String KEYWORD_ANGLE;
			static const Ogre::String KEYWORD_RANDOM;
			static const Ogre::String KEYWORD_NORMAL;

			CircleEmitterTokens(void) {};
			virtual ~CircleEmitterTokens(void) {};

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
			class RadiusToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRadiusToken;
			class CircleAngleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mCircleAngleToken;
			class StepToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mStepToken;
			class RandomToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRandomToken;
			class NormalToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mNormalToken;
	};

}
#endif
