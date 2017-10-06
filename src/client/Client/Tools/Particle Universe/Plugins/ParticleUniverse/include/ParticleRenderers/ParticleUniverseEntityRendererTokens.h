/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ENTITY_RENDERER_TOKENS_H__
#define __PU_ENTITY_RENDERER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for a EntityRenderer.
    */
	class _ParticleUniverseExport EntityRendererTokens : public ParticleRendererTokens
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_MESH_NAME;
			static const Ogre::String KEYWORD_ORIENTATION_TYPE;
			static const Ogre::String KEYWORD_ORIENTED_SELF;
			static const Ogre::String KEYWORD_ORIENTED_SELF_MIRRORED;
			static const Ogre::String KEYWORD_ORIENTED_SHAPE;

			EntityRendererTokens(void) {};
			virtual ~EntityRendererTokens(void) {};

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
			class MeshNameToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMeshNameToken;
			class EntityOrientationTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEntityOrientationTypeToken;
	};

}
#endif
