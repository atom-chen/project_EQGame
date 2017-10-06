/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TECHNIQUE_TOKENS_H__
#define __PU_TECHNIQUE_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a Technique level.
    */
	class _ParticleUniverseExport ParticleTechniqueTokens : public ITokenBind
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_ENABLED;
			static const Ogre::String KEYWORD_POSITION;
			static const Ogre::String KEYWORD_KEEP_LOCAL;
			static const Ogre::String KEYWORD_TECHNIQUE;
			static const Ogre::String KEYWORD_RENDERER;
			static const Ogre::String KEYWORD_EMITTER;
			static const Ogre::String KEYWORD_AFFECTOR;
			static const Ogre::String KEYWORD_OBSERVER;
			static const Ogre::String KEYWORD_HANDLER;
			static const Ogre::String KEYWORD_BEHAVIOUR;
			static const Ogre::String KEYWORD_EXTERN;
			static const Ogre::String KEYWORD_VISUAL_PARTICLE_QUOTA;
			static const Ogre::String KEYWORD_EMITTED_EMITTER_QUOTA;
			static const Ogre::String KEYWORD_EMITTED_AFFECTOR_QUOTA;
			static const Ogre::String KEYWORD_EMITTED_TECHNIQUE_QUOTA;
			static const Ogre::String KEYWORD_EMITTED_SYSTEM_QUOTA;
			static const Ogre::String KEYWORD_MATERIAL;
			static const Ogre::String KEYWORD_LOD_INDEX;
			static const Ogre::String KEYWORD_DEFAULT_PARTICLE_WIDTH;
			static const Ogre::String KEYWORD_DEFAULT_PARTICLE_HEIGHT;
			static const Ogre::String KEYWORD_DEFAULT_PARTICLE_DEPTH;
			static const Ogre::String KEYWORD_SPHASHING_CELL_DIMENSION;
			static const Ogre::String KEYWORD_SPHASHING_CELL_OVERLAP;
			static const Ogre::String KEYWORD_SPHASHING_SIZE;
			static const Ogre::String KEYWORD_SPHASHING_UPDATE_INTERVAL;
			static const Ogre::String KEYWORD_MAX_VELOCITY;

			ParticleTechniqueTokens(void) {};
			virtual ~ParticleTechniqueTokens(void) {};

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
			class RendererToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRendererToken;
			class EmitterToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmitterToken;
			class AffectorToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAffectorToken;
			class ObserverToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mObserverToken;
			class EventHandlerToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEventHandlerToken;
			class BehaviourToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mBehaviourToken;
			class ExternToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mExternToken;
			class VisualParticleQuotaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mVisualParticleQuotaToken;
			class EmittedEmitterQuotaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmittedEmitterQuotaToken;
			class EmittedTechniqueQuotaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmittedTechniqueQuotaToken;
			class EmittedAffectorQuotaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmittedAffectorQuotaToken;
			class EmittedSystemQuotaToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEmittedSystemQuotaToken;
			class MaterialToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaterialToken;
			class LodIndexToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mLodIndexToken;
			class SpatialHashingCellDimensionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpatialHashingCellDimensionToken;
			class SpatialHashingCellOverlapToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpatialHashingCellOverlapToken;
			class SpatialHashTableSizeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpatialHashTableSizeToken;
			class SpatialHashingUpdateIntervalToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSpatialHashingUpdateIntervalToken;
			class MaxVelocityToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMaxVelocityToken;

			class WidthToken : public IToken
			{
				virtual void parse(ParticleScriptDeserializer* deserializer);
				virtual void postParse(ParticleScriptDeserializer* deserializer);
			} mWidthToken;

			class HeightToken : public IToken
			{
				virtual void parse(ParticleScriptDeserializer* deserializer);
				virtual void postParse(ParticleScriptDeserializer* deserializer);
			} mHeightToken;
			
			class DepthToken : public IToken
			{
				virtual void parse(ParticleScriptDeserializer* deserializer);
				virtual void postParse(ParticleScriptDeserializer* deserializer);
			} mDepthToken;
	};

}
#endif
