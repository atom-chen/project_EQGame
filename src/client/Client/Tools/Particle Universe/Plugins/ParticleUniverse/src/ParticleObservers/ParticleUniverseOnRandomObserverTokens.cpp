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
#include "ParticleObservers/ParticleUniverseOnRandomObserver.h"
#include "ParticleObservers/ParticleUniverseOnRandomObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnRandomObserverTokens::KEYWORD_RANDOM_THRESHOLD = "random_threshold";

	//-----------------------------------------------------------------------
	void OnRandomObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RANDOM_THRESHOLD, &mThresholdToken);
	}
	//-----------------------------------------------------------------------
	void OnRandomObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnRandomObserver
		const OnRandomObserver* observer = static_cast<const OnRandomObserver*>(element);

		// Write the header of the OnRandomObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		if (observer->getThreshold() != OnRandomObserver::DEFAULT_THRESHOLD) serializer->writeLine(
			KEYWORD_RANDOM_THRESHOLD, Ogre::StringConverter::toString(observer->getThreshold()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnRandomObserverTokens::ThresholdToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnRandomObserverTokens::ThresholdToken::parse");
		OnRandomObserver* observer = static_cast<OnRandomObserver*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setThreshold(threshold);
	}

}
