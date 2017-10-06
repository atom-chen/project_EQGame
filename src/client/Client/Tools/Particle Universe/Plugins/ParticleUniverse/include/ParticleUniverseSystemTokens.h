/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_TOKENS_H__
#define __PU_SYSTEM_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"
#include "ParticleUniverseTechniqueTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a ParticleSystem level.
    */
	class _ParticleUniverseExport ParticleSystemTokens : public ITokenBind
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_SYSTEM;
			static const Ogre::String KEYWORD_TECHNIQUE;
			static const Ogre::String KEYWORD_KEEP_LOCAL;
			static const Ogre::String KEYWORD_ITERATION_INTERVAL;
			static const Ogre::String KEYWORD_FIXED_TIMEOUT;
			static const Ogre::String KEYWORD_NONVIS_UPDATE_TIMEOUT;
			static const Ogre::String KEYWORD_LOD_DISTANCES;
			static const Ogre::String KEYWORD_SMOOTH_LOD;
			static const Ogre::String KEYWORD_FAST_FORWARD;
			static const Ogre::String KEYWORD_MAIN_CAMERA_NAME;
			static const Ogre::String KEYWORD_SCALE_VELOCITY;
			static const Ogre::String KEYWORD_SCALE_TIME;
			static const Ogre::String KEYWORD_SCALE;
			static const Ogre::String KEYWORD_TIGHT_BOUNDING_BOX;

			ParticleSystemTokens(void) {};
			virtual ~ParticleSystemTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);

		protected:
			// Gets called during serialization
			ParticleTechniqueTokens mParticleTechniqueTokens;

			// Tokens which get called during parsing.
			class TechniqueToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTechniqueToken;
			class LodDistancesToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mLodDistancesToken;
			class SmoothLodToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSmoothLodToken;
			class IterationIntervalToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mIterationIntervalToken;
			class FixedTimeoutToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mFixedTimeoutToken;
			class NonVisibleUpdateTimeoutToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mNonVisibleUpdateTimeoutToken;
			class FastForwardToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mFastForwardToken;
			class MainCameraNameToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mMainCameraNameToken;
			class ScaleToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleToken;
			class ScaleVelocityToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleVelocityToken;
			class ScaleTimeToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mScaleTimeToken;
			class TightBoundingBoxToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mTightBoundingBoxToken;
	};

}
#endif
