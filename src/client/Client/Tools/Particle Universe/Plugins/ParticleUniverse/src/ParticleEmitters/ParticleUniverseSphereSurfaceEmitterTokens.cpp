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
#include "ParticleEmitters/ParticleUniverseSphereSurfaceEmitter.h"
#include "ParticleEmitters/ParticleUniverseSphereSurfaceEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SphereSurfaceEmitterTokens::KEYWORD_RADIUS = "sphere_surface_em_radius";

	//-----------------------------------------------------------------------
	void SphereSurfaceEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RADIUS, &mRadiusToken);
	}
	//-----------------------------------------------------------------------
	void SphereSurfaceEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SphereSurfaceEmitter
		const SphereSurfaceEmitter* emitter = static_cast<const SphereSurfaceEmitter*>(element);

		// Write the header of the SphereSurfaceEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getRadius() != SphereSurfaceEmitter::DEFAULT_RADIUS) serializer->writeLine(
			KEYWORD_RADIUS, Ogre::StringConverter::toString(emitter->getRadius()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void SphereSurfaceEmitterTokens::RadiusToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "SphereSurfaceEmitterTokens::HeightToken::parse");
		SphereSurfaceEmitter* emitter = static_cast<SphereSurfaceEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real radius;
		deserializer->parseReal(radius);
		emitter->setRadius(radius);
	}

}
