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
#include "ParticleEventHandlers/ParticleUniverseDoFreezeEventHandler.h"
#include "ParticleEventHandlers/ParticleUniverseDoFreezeEventHandlerTokens.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void DoFreezeEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a DoFreezeEventHandler
		const DoFreezeEventHandler* eventHandler = static_cast<const DoFreezeEventHandler*>(element);

		// Write the header of the DoFreezeEventHandler
		serializer->writeLine(KEYWORD_HANDLER, eventHandler->getEventHandlerType(), eventHandler->getName(), 12);
		serializer->writeLine("{", 12);

		// Write base attributes
		ParticleEventHandlerTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 12);
	}

}
