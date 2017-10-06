/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_FLOCK_CENTER_AFFECTOR_TOKENS_H__
#define __PU_FLOCK_CENTER_AFFECTOR_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffectorTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the LinearForceAffector.
    */
	class _ParticleUniverseExport LinearForceAffectorTokens : public ForceAffectorTokens
	{
		public:
			LinearForceAffectorTokens(void) {};
			virtual ~LinearForceAffectorTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister){};

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};

}
#endif
