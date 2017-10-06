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
#include "ParticleObservers/ParticleUniverseOnEmissionObserver.h"
#include "ParticleObservers/ParticleUniverseOnEmissionObserverTokens.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void OnEmissionObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnEmissionObserver
		const OnEmissionObserver* observer = static_cast<const OnEmissionObserver*>(element);

		// Write the header of the OnEmissionObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
