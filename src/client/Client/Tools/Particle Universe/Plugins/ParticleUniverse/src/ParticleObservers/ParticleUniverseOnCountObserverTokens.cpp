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
#include "ParticleObservers/ParticleUniverseOnCountObserver.h"
#include "ParticleObservers/ParticleUniverseOnCountObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnCountObserverTokens::KEYWORD_COUNT_THRESHOLD = "count_threshold";

	//-----------------------------------------------------------------------
	void OnCountObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_COUNT_THRESHOLD, &mCountToken);
	}
	//-----------------------------------------------------------------------
	void OnCountObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnCountObserver
		const OnCountObserver* observer = static_cast<const OnCountObserver*>(element);

		// Write the header of the OnCountObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		Ogre::String compare = KEYWORD_CO_GREATER_THAN;
		if (observer->getCompare() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
		serializer->writeLine(KEYWORD_COUNT_THRESHOLD, compare, Ogre::StringConverter::toString(observer->getThreshold()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnCountObserverTokens::CountToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnCountObserverTokens::CountToken::parse");
		OnCountObserver* observer = static_cast<OnCountObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setCompare(CO_LESS_THAN);
		}
		else
		{
			observer->setCompare(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::uint threshold;
		deserializer->parseUint(threshold);
		observer->setThreshold(threshold);
	}

}
