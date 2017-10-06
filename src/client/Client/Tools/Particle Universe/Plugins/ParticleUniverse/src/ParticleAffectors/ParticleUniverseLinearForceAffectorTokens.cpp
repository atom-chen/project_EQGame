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
#include "ParticleAffectors/ParticleUniverseLinearForceAffector.h"
#include "ParticleAffectors/ParticleUniverseLinearForceAffectorTokens.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void LinearForceAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a LinearForceAffector
		const LinearForceAffector* affector = static_cast<const LinearForceAffector*>(element);

		// Write the header of the LinearForceAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ForceAffectorTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
