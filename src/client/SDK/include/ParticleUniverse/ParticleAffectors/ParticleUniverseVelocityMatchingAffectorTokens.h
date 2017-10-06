/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VELOCITY_MATCHING_AFFECTOR_TOKENS_H__
#define __PU_VELOCITY_MATCHING_AFFECTOR_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the VelocityMatchingAffector.
    */
	class _ParticleUniverseExport VelocityMatchingAffectorTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_RADIUS;

			VelocityMatchingAffectorTokens(void) {};
			virtual ~VelocityMatchingAffectorTokens(void) {};

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
			class RadiusToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mRadiusToken;
	};

}
#endif