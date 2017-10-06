/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SCENE_DECORATOR_EXTERN_TOKENS_H__
#define __PU_SCENE_DECORATOR_EXTERN_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternTokens.h"
#include "ParticleUniverseAttachableTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the SceneDecoratorExtern.
    */
	class _ParticleUniverseExport SceneDecoratorExternTokens : public AttachableTokens, ExternTokens
	{
		public:
			static const Ogre::String KEYWORD_MESH_NAME;
			static const Ogre::String KEYWORD_MATERIAL_NAME;
			static const Ogre::String KEYWORD_SCALE;
			static const Ogre::String KEYWORD_POSITION;

			SceneDecoratorExternTokens(void) {};
			virtual ~SceneDecoratorExternTokens(void) {};

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
			class MeshToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mMeshToken;
			class MaterialNameToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mMaterialNameToken;
			class ScaleToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mScaleToken;
			class PositionToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mPositionToken;
	};

}
#endif
