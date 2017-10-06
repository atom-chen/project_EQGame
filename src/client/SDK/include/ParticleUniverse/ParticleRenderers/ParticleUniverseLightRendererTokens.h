/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LIGHT_RENDERER_TOKENS_H__
#define __PU_LIGHT_RENDERER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for a LightRenderer.
    */
	class _ParticleUniverseExport LightRendererTokens : public ParticleRendererTokens
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_LIGHT_TYPE;
			static const Ogre::String KEYWORD_RENDER_QUEUE;
			static const Ogre::String KEYWORD_DIFFUSE;
			static const Ogre::String KEYWORD_SPECULAR;
			static const Ogre::String KEYWORD_ATT_RANGE;
			static const Ogre::String KEYWORD_ATT_CONSTANT;
			static const Ogre::String KEYWORD_ATT_LINEAR;
			static const Ogre::String KEYWORD_ATT_QUADRATIC;
			static const Ogre::String KEYWORD_SPOT_INNER;
			static const Ogre::String KEYWORD_SPOT_OUTER;
			static const Ogre::String KEYWORD_FALLOFF;
			static const Ogre::String KEYWORD_POWERSCALE;
			static const Ogre::String KEYWORD_POINT;
			static const Ogre::String KEYWORD_SPOT;

			LightRendererTokens(void) {};
			virtual ~LightRendererTokens(void) {};

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
			class LightTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mLightTypeToken;
			class QueueIdToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mQueueIdToken;
			class DiffuseColourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDiffuseColourToken;
			class SpecularColourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpecularColourToken;
			class AttenuationRangeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAttenuationRangeToken;
			class AttenuationConstantToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAttenuationConstantToken;
			class AttenuationLinearToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAttenuationLinearToken;
			class AttenuationQuadraticToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAttenuationQuadraticToken;
			class SpotlightInnerAngleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpotlightInnerAngleToken;
			class SpotlightOuterAngleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpotlightOuterAngleToken;
			class SpotlightFalloffToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpotlightFalloffToken;
			class PowerScaleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPowerScaleToken;
	};

}
#endif
