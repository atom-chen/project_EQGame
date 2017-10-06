/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DYNAMIC_ATTRIBUTE_TOKENS_H__
#define __PU_DYNAMIC_ATTRIBUTE_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a DynamicAttribute level.
    */
	class _ParticleUniverseExport DynamicAttributeTokens : public ITokenBind
	{
		public:
			static const Ogre::String KEYWORD_CONTROL_POINT;
			static const Ogre::String KEYWORD_MIN;
			static const Ogre::String KEYWORD_MAX;
			static const Ogre::String KEYWORD_OSCILLATE_FREQUENCY;
			static const Ogre::String KEYWORD_OSCILLATE_PHASE;
			static const Ogre::String KEYWORD_OSCILLATE_BASE;
			static const Ogre::String KEYWORD_OSCILLATE_AMPLITUDE;
			static const Ogre::String KEYWORD_OSCILLATE_TYPE;
			static const Ogre::String KEYWORD_DYN_OSCILLATE;
			static const Ogre::String KEYWORD_DYN_RANDOM;
			static const Ogre::String KEYWORD_DYN_CURVED_LINEAR;
			static const Ogre::String KEYWORD_DYN_CURVED_SPLINE;
			static const Ogre::String KEYWORD_SINE;
			static const Ogre::String KEYWORD_SQUARE;

			DynamicAttributeTokens(void) {};
			virtual ~DynamicAttributeTokens(void) {};

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
			class ControlPointToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mControlPointToken;
			class MinimumToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMinimumToken;
			class MaximumToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaximumToken;
			class FrequencyToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mFrequencyToken;
			class PhaseToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPhaseToken;
			class BaseToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mBaseToken;
			class AmplitudeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAmplitudeToken;
			class TypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTypeToken;
	};

}
#endif
