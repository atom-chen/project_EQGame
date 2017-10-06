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
#include "ParticleUniverseAttachableTokens.h"
#include "ParticleUniverseAttachable.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String AttachableTokens::KEYWORD_DISTANCE_THRESHOLD = "attachable_distance_threshold";

	//-----------------------------------------------------------------------
	void AttachableTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction("attachable_distance_threshold", &mDistanceThresholdToken);
	}
	//-----------------------------------------------------------------------
	void AttachableTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a AlignAffector
		const Attachable* attachable = static_cast<const Attachable*>(element);

		if (attachable->getDistanceThreshold() != std::numeric_limits<float>::max()) serializer->writeLine(
			KEYWORD_DISTANCE_THRESHOLD, Ogre::StringConverter::toString(attachable->getDistanceThreshold()), 12);
	}
	//-----------------------------------------------------------------------
	void AttachableTokens::DistanceThresholdToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EXTERN, "ThresholdToken::parse");
		Attachable* attachable = static_cast<Attachable*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real distanceThreshold;
		deserializer->parseReal(distanceThreshold);
		attachable->setDistanceThreshold(distanceThreshold);
	}

}
