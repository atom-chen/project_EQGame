/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_OBSERVER_TOKENS_H__
#define __PU_OBSERVER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a ParticleObserver level.
    */
	class _ParticleUniverseExport ParticleObserverTokens : public ITokenBind
	{
		public:
			static const Ogre::String KEYWORD_OBSERVER;
			static const Ogre::String KEYWORD_OBSERVE_PARTICLE_TYPE;
			static const Ogre::String KEYWORD_OBSERVE_INTERVAL;
			static const Ogre::String KEYWORD_OBSERVE_UNTIL_EVENT;
			static const Ogre::String KEYWORD_ENABLED;
			static const Ogre::String KEYWORD_VISUAL_PARTICLE;
			static const Ogre::String KEYWORD_EMITTER_PARTICLE;
			static const Ogre::String KEYWORD_TECHNIQUE_PARTICLE;
			static const Ogre::String KEYWORD_AFFECTOR_PARTICLE;
			static const Ogre::String KEYWORD_SYSTEM_PARTICLE;
			static const Ogre::String KEYWORD_LESS_THAN;
			static const Ogre::String KEYWORD_CO_GREATER_THAN;

			ParticleObserverTokens(void) {};
			virtual ~ParticleObserverTokens(void) {};

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
			class ParticleTypeToObserveToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mParticleTypeToObserveToken;
			class IntervalToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mIntervalToken;
			class ObserveUntilEventToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mObserveUntilEventToken;
	};

}
#endif
