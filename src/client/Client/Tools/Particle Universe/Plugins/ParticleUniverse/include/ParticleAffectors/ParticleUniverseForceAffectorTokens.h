/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_FORCE_AFFECTOR_TOKENS_H__
#define __PU_FORCE_AFFECTOR_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the ForceAffector.
    */
	class _ParticleUniverseExport ForceAffectorTokens : public ParticleAffectorTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_FORCE_VECTOR;
			static const Ogre::String KEYWORD_APPLICATION;
			static const Ogre::String KEYWORD_ADD;
			static const Ogre::String KEYWORD_AVERAGE;

			ForceAffectorTokens(void) {};
			virtual ~ForceAffectorTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);

		protected:
			// Tokens which get called during parsing.
			class ForceVectorToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mForceVectorToken;
			class ForceApplicationToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mForceApplicationToken;
	};

}
#endif
