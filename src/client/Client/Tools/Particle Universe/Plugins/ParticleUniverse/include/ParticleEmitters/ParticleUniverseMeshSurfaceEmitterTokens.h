/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_MESH_SURFACE_EMITTER_TOKENS_H__
#define __PU_MESH_SURFACE_EMITTER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the MeshSurfaceEmitter.
    */
	class _ParticleUniverseExport MeshSurfaceEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_MESH_NAME;
			static const Ogre::String KEYWORD_DISTRIBUTION;
			static const Ogre::String KEYWORD_SCALE;
			static const Ogre::String KEYWORD_EDGE;
			static const Ogre::String KEYWORD_HETEROGENEOUS_1;
			static const Ogre::String KEYWORD_HETEROGENEOUS_2;
			static const Ogre::String KEYWORD_HOMOGENEOUS;
			static const Ogre::String KEYWORD_VERTEX;

			MeshSurfaceEmitterTokens(void) {};
			virtual ~MeshSurfaceEmitterTokens(void) {};

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
			class DistributionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mDistributionToken;
			class ScaleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleToken;
	};

}
#endif
