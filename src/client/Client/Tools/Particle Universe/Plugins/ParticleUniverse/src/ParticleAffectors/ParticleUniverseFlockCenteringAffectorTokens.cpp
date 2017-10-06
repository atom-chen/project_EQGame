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
#include "ParticleAffectors/ParticleUniverseFlockCenteringAffector.h"
#include "ParticleAffectors/ParticleUniverseFlockCenteringAffectorTokens.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void FlockCenteringAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a FlockCenteringAffector
		const FlockCenteringAffector* affector = static_cast<const FlockCenteringAffector*>(element);

		// Write the header of the FlockCenteringAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
