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
#include "ParticleObservers/ParticleUniverseOnEventFlagObserver.h"
#include "ParticleObservers/ParticleUniverseOnEventFlagObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnEventFlagObserverTokens::KEYWORD_EVENT_FLAG = "event_flag";

	//-----------------------------------------------------------------------
	void OnEventFlagObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_EVENT_FLAG, &mEventFlagToken);
	}
	//-----------------------------------------------------------------------
	void OnEventFlagObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnEventFlagObserver
		const OnEventFlagObserver* observer = static_cast<const OnEventFlagObserver*>(element);

		// Write the header of the OnEventFlagObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		if (observer->getEventFlag() != OnEventFlagObserver::DEFAULT_EVENT_FLAG) serializer->writeLine(
			KEYWORD_EVENT_FLAG, Ogre::StringConverter::toString(observer->getEventFlag()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnEventFlagObserverTokens::EventFlagToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnEventFlagObserverTokens::EventFlagToken::parse");
		OnEventFlagObserver* observer = static_cast<OnEventFlagObserver*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint32 flag;
		deserializer->parseUint32(flag);
		observer->setEventFlag(flag);
	}

}
