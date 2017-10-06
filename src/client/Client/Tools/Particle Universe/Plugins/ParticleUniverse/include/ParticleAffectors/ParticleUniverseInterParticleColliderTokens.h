/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_INTERPARTICLE_COLLIDER_TOKENS_H__
#define __PU_INTERPARTICLE_COLLIDER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the InterParticleCollider.
    */
	class _ParticleUniverseExport InterParticleColliderTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_ADJUSTMENT;
			static const Ogre::String KEYWORD_COLLISION_RESPONSE;
			static const Ogre::String KEYWORD_AVERAGE_VELOCITY;
			static const Ogre::String KEYWORD_ANGLE_BASED_VELOCITY;

			InterParticleColliderTokens(void) {};
			virtual ~InterParticleColliderTokens(void) {};

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
			class AdjustmentToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAdjustmentToken;
			class CollisionResponseToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mCollisionResponseToken;
	};

}
#endif
