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

#include "ParticleEmitters/ParticleUniversePointEmitter.h"
#include "ParticleEmitters/ParticleUniversePointEmitterTokens.h"
#include "ParticleUniverseScriptSerializer.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	void PointEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a PointEmitter
		const PointEmitter* emitter = static_cast<const PointEmitter*>(element);

		// Write the header of the PointEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
