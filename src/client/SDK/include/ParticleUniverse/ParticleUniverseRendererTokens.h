/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RENDERER_TOKENS_H__
#define __PU_RENDERER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for a Renderer.
    */
	class _ParticleUniverseExport ParticleRendererTokens : public ITokenBind
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_RENDERER;
			static const Ogre::String KEYWORD_RENDER_Q_GROUP;
			static const Ogre::String KEYWORD_SORTING;
			static const Ogre::String KEYWORD_TEXCOORDS_DEFINE;
			static const Ogre::String KEYWORD_TEXCOORDS_ROWS;
			static const Ogre::String KEYWORD_TEXCOORDS_SET;
			static const Ogre::String KEYWORD_TEXCOORDS_COLUMNS;
			static const Ogre::String KEYWORD_USE_SOFT_PARTICLES;
			static const Ogre::String KEYWORD_SOFT_PARTICLES_CONTRAST_POWER;
			static const Ogre::String KEYWORD_SOFT_PARTICLES_SCALE;
			static const Ogre::String KEYWORD_SOFT_PARTICLES_DELTA;

			ParticleRendererTokens(void) {};
			virtual ~ParticleRendererTokens(void) {};

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
			class RenderQueueGroupToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRenderQueueGroupToken;
			class SortingToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSortingToken;
			class TextureCoordsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsToken;
			class TextureCoordsSetToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsSetToken;
			class TextureCoordsRowsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsRowsToken;
			class TextureCoordsColumnsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTextureCoordsColumnsToken;
			class UseSoftParticlesToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mUseSoftParticlesToken;
			class SoftParticlesContrastPowerToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSoftParticlesContrastPowerToken;
			class SoftParticlesScaleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSoftParticlesScaleToken;
			class SoftParticlesDeltaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSoftParticlesDeltaToken;

	};

}
#endif
