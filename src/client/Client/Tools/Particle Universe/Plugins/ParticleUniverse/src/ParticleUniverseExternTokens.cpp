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
#include "ParticleUniverseExternTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ExternTokens::KEYWORD_EXTERN = "extern";

	//-----------------------------------------------------------------------
	void ExternTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		// Nothing yet
	}
	//-----------------------------------------------------------------------
	void ExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Nothing yet
	}

}
