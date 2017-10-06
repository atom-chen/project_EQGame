/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BILLBOARD_RENDERER_TOKENS_H__
#define __PU_BILLBOARD_RENDERER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for a BillboardRenderer.
    */
	class _ParticleUniverseExport BillboardRendererTokens : public ParticleRendererTokens
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_BILLBOARD_TYPE;
			static const Ogre::String KEYWORD_BILLBOARD_ORIGIN;
			static const Ogre::String KEYWORD_ROTATION_TYPE;
			static const Ogre::String KEYWORD_COMMON_DIRECTION;
			static const Ogre::String KEYWORD_COMMON_UP_VECTOR;
			static const Ogre::String KEYWORD_POINT_RENDERING;
			static const Ogre::String KEYWORD_ACCURATE_FACING;
			static const Ogre::String KEYWORD_POINT;
			static const Ogre::String KEYWORD_ORIENTED_COMMON;
			static const Ogre::String KEYWORD_ORIENTED_SELF;
			static const Ogre::String KEYWORD_ORIENTED_SHAPE;
			static const Ogre::String KEYWORD_PERPENDICULAR_COMMON;
			static const Ogre::String KEYWORD_PERPENDICULAR_SELF;
			static const Ogre::String KEYWORD_TOP_LEFT;
			static const Ogre::String KEYWORD_TOP_CENTER;
			static const Ogre::String KEYWORD_TOP_RIGHT;
			static const Ogre::String KEYWORD_CENTER_LEFT;
			static const Ogre::String KEYWORD_CENTER_RIGHT;
			static const Ogre::String KEYWORD_CENTER;
			static const Ogre::String KEYWORD_BOTTON_LEFT;
			static const Ogre::String KEYWORD_BOTTOM_CENTER;
			static const Ogre::String KEYWORD_BOTTOM_RIGHT;
			static const Ogre::String KEYWORD_VERTEX;
			static const Ogre::String KEYWORD_TEXCOORD;

			BillboardRendererTokens(void) {};
			virtual ~BillboardRendererTokens(void) {};

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
			class BillboardTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mBillboardTypeToken;
			class BillboardOriginToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mBillboardOriginToken;
			class BillboardRotationTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mBillboardRotationTypeToken;
			class CommonDirectionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mCommonDirectionToken;
			class CommonUpVectorToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mCommonUpVectorToken;
			class PointRenderingToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPointRenderingToken;
			class AccurateFacingToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAccurateFacingToken;
	};

}
#endif
