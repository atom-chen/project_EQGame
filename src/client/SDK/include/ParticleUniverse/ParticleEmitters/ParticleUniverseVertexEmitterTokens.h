/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VERTEX_EMITTER_TOKEN_H__
#define __PU_VERTEX_EMITTER_TOKEN_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the VertexEmitter.
    */
	class _ParticleUniverseExport VertexEmitterTokens : public ParticleEmitterTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_STEP;
			static const Ogre::String KEYWORD_SEGMENTS;
			static const Ogre::String KEYWORD_ITERATIONS;
			static const Ogre::String KEYWORD_MESH_NAME;

			VertexEmitterTokens(void) {};
			virtual ~VertexEmitterTokens(void) {};

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
			class StepToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mStepToken;
			class SegmentsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSegmentsToken;
			class IterationsToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mIterationsToken;
			class MeshNameToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMeshNameToken;
	};

}
#endif
