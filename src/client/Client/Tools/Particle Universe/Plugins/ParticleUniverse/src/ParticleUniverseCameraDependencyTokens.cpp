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
#include "ParticleUniverseCameraDependencyTokens.h"
#include "ParticleUniverseCameraDependency.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String CameraDependencyTokens::KEYWORD_DISTANCE_THRESHOLD = "distance_threshold";
	const Ogre::String CameraDependencyTokens::KEYWORD_INCREASE = "increase";

	//-----------------------------------------------------------------------
	void CameraDependencyTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_DISTANCE_THRESHOLD, &mThresholdToken);
		tokenRegister->addLexemeAction(KEYWORD_INCREASE, &mIncreaseToken);
	}
	//-----------------------------------------------------------------------
	void CameraDependencyTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a CameraDependency
		const CameraDependency* dependency = static_cast<const CameraDependency*>(element);

		// Write attributes
		if (dependency->getThreshold() != CameraDependency::DEFAULT_DISTANCE_THRESHOLD) serializer->writeLine(
			KEYWORD_DISTANCE_THRESHOLD, Ogre::StringConverter::toString(dependency->getThreshold()), 12);
		if (dependency->isIncrease() != CameraDependency::DEFAULT_INCREASE) serializer->writeLine(
			KEYWORD_INCREASE, Ogre::StringConverter::toString(dependency->isIncrease()), 12);
	}
	//-----------------------------------------------------------------------
	void CameraDependencyTokens::ThresholdToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(DEPENDENCY, "ThresholdToken::parse");
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real distanceFactor;
		deserializer->parseReal(distanceFactor);
		cameraDependency->setThreshold(distanceFactor);
	}
	//-----------------------------------------------------------------------
	void CameraDependencyTokens::IncreaseToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(DEPENDENCY, "IncreaseToken::parse");
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getCurrentSectionElement());
		bool increase;
		deserializer->parseBool(increase);
		cameraDependency->setIncrease(increase);
	}

}
