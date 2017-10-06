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
#include "ParticleObservers/ParticleUniverseOnClearObserver.h"
#include "ParticleObservers/ParticleUniverseOnClearObserverTokens.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void OnClearObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnClearObserver
		const OnClearObserver* observer = static_cast<const OnClearObserver*>(element);

		// Write the header of the OnClearObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
