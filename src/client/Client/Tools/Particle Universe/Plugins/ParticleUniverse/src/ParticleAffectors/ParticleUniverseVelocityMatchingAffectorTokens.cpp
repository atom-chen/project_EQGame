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
#include "ParticleAffectors/ParticleUniverseVelocityMatchingAffector.h"
#include "ParticleAffectors/ParticleUniverseVelocityMatchingAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String VelocityMatchingAffectorTokens::KEYWORD_RADIUS = "velocity_matching_radius";

	//-----------------------------------------------------------------------
	void VelocityMatchingAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RADIUS, &mRadiusToken);
	}
	//-----------------------------------------------------------------------
	void VelocityMatchingAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a VelocityMatchingAffector
		const VelocityMatchingAffector* affector = static_cast<const VelocityMatchingAffector*>(element);

		// Write the header of the VelocityMatchingAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getRadius() != VelocityMatchingAffector::DEFAULT_RADIUS) serializer->writeLine(
			KEYWORD_RADIUS, Ogre::StringConverter::toString(affector->getRadius()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void VelocityMatchingAffectorTokens::RadiusToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "VelocityMatchingAffectorTokens::RadiusToken::parse");
		VelocityMatchingAffector* affector = static_cast<VelocityMatchingAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real radius;
		deserializer->parseReal(radius);
		affector->setRadius(radius);
	}

}
