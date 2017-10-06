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
#include "ParticleObservers/ParticleUniverseOnTimeObserver.h"
#include "ParticleObservers/ParticleUniverseOnTimeObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnTimeObserverTokens::KEYWORD_ON_TIME = "on_time";
	const Ogre::String OnTimeObserverTokens::KEYWORD_SINCE_START_SYSTEM = "since_start_system";

	//-----------------------------------------------------------------------
	void OnTimeObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_ON_TIME, &mTimeToken);
		tokenRegister->addLexemeAction(KEYWORD_SINCE_START_SYSTEM, &mSinceStartSystemToken);
	}
	//-----------------------------------------------------------------------
	void OnTimeObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnTimeObserver
		const OnTimeObserver* observer = static_cast<const OnTimeObserver*>(element);

		// Write the header of the OnTimeObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		if (observer->isSinceStartSystem() != OnTimeObserver::DEFAULT_SINCE_START_SYSTEM) serializer->writeLine(
			KEYWORD_SINCE_START_SYSTEM, Ogre::StringConverter::toString(observer->isSinceStartSystem()), 12);
		Ogre::String compare = KEYWORD_CO_GREATER_THAN;
		if (observer->getCompare() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
		serializer->writeLine(KEYWORD_ON_TIME, compare, Ogre::StringConverter::toString(observer->getThreshold()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnTimeObserverTokens::TimeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnTimeObserverTokens::TimeToken::parse");
		OnTimeObserver* observer = static_cast<OnTimeObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setCompare(CO_LESS_THAN);
		}
		else
		{
			observer->setCompare(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setThreshold(threshold);
	}
	//-----------------------------------------------------------------------
	void OnTimeObserverTokens::SinceStartSystemToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnTimeObserverTokens::TimeToken::parse");
		OnTimeObserver* observer = static_cast<OnTimeObserver*>(deserializer->context.getCurrentSectionElement());
		bool sinceStartSystem;
		deserializer->parseBool(sinceStartSystem);
		observer->setSinceStartSystem(sinceStartSystem);
	}

}
