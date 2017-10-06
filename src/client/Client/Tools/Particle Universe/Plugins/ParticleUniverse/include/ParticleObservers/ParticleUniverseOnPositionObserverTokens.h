/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONPOSITION_OBSERVER_TOKENS_H__
#define __PU_ONPOSITION_OBSERVER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the OnPositionObserver.
    */
	class _ParticleUniverseExport OnPositionObserverTokens : public ParticleObserverTokens
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_POSITION_X;
			static const Ogre::String KEYWORD_POSITION_Y;
			static const Ogre::String KEYWORD_POSITION_Z;

			OnPositionObserverTokens(void) {};
			virtual ~OnPositionObserverTokens(void) {};

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
			class PositionXThreshold : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPositionXThreshold;
			class PositionYThreshold : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPositionYThreshold;
			class PositionZThreshold : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPositionZThreshold;
	};

}
#endif
