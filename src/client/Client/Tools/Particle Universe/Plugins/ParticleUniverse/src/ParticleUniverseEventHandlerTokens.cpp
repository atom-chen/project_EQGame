/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ParticleUniverseScriptDeserializer.h"
#include "ParticleUniverseScriptSerializer.h"
#include "ParticleUniverseEventHandlerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleEventHandlerTokens::KEYWORD_HANDLER = "handler";

	//-----------------------------------------------------------------------
	void ParticleEventHandlerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		// Nothing yet
	}
	//-----------------------------------------------------------------------
	void ParticleEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Nothing yet
	}

}
