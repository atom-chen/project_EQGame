/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_EMITTER_TOKENS_H__
#define __PU_EMITTER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on an Emitter level.
    */
	class _ParticleUniverseExport ParticleEmitterTokens : public ITokenBind
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_EMITTER;
			static const Ogre::String KEYWORD_ENABLED;
			static const Ogre::String KEYWORD_POSITION;
			static const Ogre::String KEYWORD_KEEP_LOCAL;
			static const Ogre::String KEYWORD_DIRECTION;
			static const Ogre::String KEYWORD_ORIENTATION;
			static const Ogre::String KEYWORD_ORIENTATION_RANGE_START;
			static const Ogre::String KEYWORD_ORIENTATION_RANGE_END;
			static const Ogre::String KEYWORD_VELOCITY;
			static const Ogre::String KEYWORD_DURATION;
			static const Ogre::String KEYWORD_REPEAT_DELAY;
			static const Ogre::String KEYWORD_EMITS;
			static const Ogre::String KEYWORD_ANGLE;
			static const Ogre::String KEYWORD_EMISSION_RATE;
			static const Ogre::String KEYWORD_TIME_TO_LIVE;
			static const Ogre::String KEYWORD_MASS;
			static const Ogre::String KEYWORD_START_TEXCOORDS;
			static const Ogre::String KEYWORD_END_TEXCOORDS;
			static const Ogre::String KEYWORD_TEXCOORDS;
			static const Ogre::String KEYWORD_START_COLOUR_RANGE;
			static const Ogre::String KEYWORD_END_COLOUR_RANGE;
			static const Ogre::String KEYWORD_COLOUR;
			static const Ogre::String KEYWORD_ALL_PARTICLE_DIM;
			static const Ogre::String KEYWORD_PARTICLE_WIDTH;
			static const Ogre::String KEYWORD_PARTICLE_HEIGHT;
			static const Ogre::String KEYWORD_PARTICLE_DEPTH;
			static const Ogre::String KEYWORD_AUTO_DIRECTION;
			static const Ogre::String KEYWORD_FORCE_EMISISON;
			static const Ogre::String KEYWORD_EMITTER_PARTICLE;
			static const Ogre::String KEYWORD_AFFECTOR_PARTICLE;
			static const Ogre::String KEYWORD_TECHNIQUE_PARTICLE;
			static const Ogre::String KEYWORD_SYSTEM_PARTICLE;

			ParticleEmitterTokens(void) {};
			virtual ~ParticleEmitterTokens(void) {};

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
			class DirectionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDirectionToken;
			class OrientationToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mOrientationToken;
			class OrientationRangeStartToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mOrientationRangeStartToken;
			class OrientationRangeEndToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mOrientationRangeEndToken;
			class EmitsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmitsToken;
			class AngleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAngleToken;

			class EmissionRateToken : public IToken
			{
				virtual void parse(ParticleScriptDeserializer* deserializer);
				virtual void postParse(ParticleScriptDeserializer* deserializer);
			} mEmissionRateToken;

			class TimeToLiveToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTimeToLiveToken;
			class MassToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMassToken;
			class VelocityToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mVelocityToken;
			class DurationToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDurationToken;
			class RepeatDelayToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRepeatDelayToken;
			class ParticleAllDimensionsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mParticleAllDimensionsToken;
			class ParticleWidthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mParticleWidthToken;
			class ParticleHeightToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mParticleHeightToken;
			class ParticleDepthToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mParticleDepthToken;
			class AutoDirectionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAutoDirectionToken;
			class ForceEmissionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mForceEmissionToken;
			class ColourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mColourToken;
			class ColourRangeStartToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mColourRangeStartToken;
			class ColourRangeEndToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mColourRangeEndToken;
			class TextureCoordsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsToken;
			class TextureCoordsRangeStartToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsRangeStartToken;
			class TextureCoordsRangeEndToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsRangeEndToken;
	};

}
#endif
