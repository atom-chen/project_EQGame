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
#include "ParticleUniverseObserver.h"
#include "ParticleUniverseObserverTokens.h"
#include "ParticleUniverseEventHandler.h"
#include "ParticleUniverseEventHandlerFactory.h"
#include "ParticleUniverseSystemManager.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleObserverTokens::KEYWORD_OBSERVER = "observer";
	const Ogre::String ParticleObserverTokens::KEYWORD_OBSERVE_PARTICLE_TYPE = "observe_particle_type";
	const Ogre::String ParticleObserverTokens::KEYWORD_OBSERVE_INTERVAL = "observe_interval";
	const Ogre::String ParticleObserverTokens::KEYWORD_OBSERVE_UNTIL_EVENT = "observe_until_event";
	const Ogre::String ParticleObserverTokens::KEYWORD_ENABLED = "enabled";
	const Ogre::String ParticleObserverTokens::KEYWORD_VISUAL_PARTICLE = "visual_particle";
	const Ogre::String ParticleObserverTokens::KEYWORD_EMITTER_PARTICLE = "emitter_particle";
	const Ogre::String ParticleObserverTokens::KEYWORD_TECHNIQUE_PARTICLE = "technique_particle";
	const Ogre::String ParticleObserverTokens::KEYWORD_AFFECTOR_PARTICLE = "affector_particle";
	const Ogre::String ParticleObserverTokens::KEYWORD_SYSTEM_PARTICLE = "system_particle";
	const Ogre::String ParticleObserverTokens::KEYWORD_LESS_THAN = "less_than";
	const Ogre::String ParticleObserverTokens::KEYWORD_CO_GREATER_THAN = "greater_than";

	//-----------------------------------------------------------------------
	void ParticleObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_OBSERVE_PARTICLE_TYPE, &mParticleTypeToObserveToken);
		tokenRegister->addLexemeAction(KEYWORD_OBSERVE_INTERVAL, &mIntervalToken);
		tokenRegister->addLexemeAction(KEYWORD_OBSERVE_UNTIL_EVENT, &mObserveUntilEventToken);
	}
	//-----------------------------------------------------------------------
	void ParticleObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleObserver
		const ParticleObserver* observer = static_cast<const ParticleObserver*>(element);

		// Write own attributes
		if (observer->_getOriginalEnabled() != ParticleObserver::DEFAULT_ENABLED) serializer->writeLine(
			KEYWORD_ENABLED, Ogre::StringConverter::toString(observer->_getOriginalEnabled()), 12);
		Ogre::String particleType = KEYWORD_VISUAL_PARTICLE;
		if (observer->getParticleTypeToObserve() == Particle::PT_EMITTER)
		{
			particleType = KEYWORD_EMITTER_PARTICLE;
		}
		else if (observer->getParticleTypeToObserve() == Particle::PT_TECHNIQUE)
		{
			particleType = KEYWORD_TECHNIQUE_PARTICLE;
		}
		else if (observer->getParticleTypeToObserve() == Particle::PT_AFFECTOR)
		{
			particleType = KEYWORD_AFFECTOR_PARTICLE;
		}
		else if (observer->getParticleTypeToObserve() == Particle::PT_SYSTEM)
		{
			particleType = KEYWORD_SYSTEM_PARTICLE;
		}
		if (observer->getParticleTypeToObserve() != ParticleObserver::DEFAULT_PARTICLE_TYPE) serializer->writeLine(
			KEYWORD_OBSERVE_PARTICLE_TYPE, particleType, 12);
		if (observer->getObserverInterval() != ParticleObserver::DEFAULT_INTERVAL) serializer->writeLine(
			KEYWORD_OBSERVE_INTERVAL, Ogre::StringConverter::toString(observer->getObserverInterval()), 12);
		if (observer->getObserveUntilEvent() != ParticleObserver::DEFAULT_UNTIL_EVENT) serializer->writeLine(
			KEYWORD_OBSERVE_UNTIL_EVENT, Ogre::StringConverter::toString(observer->getObserveUntilEvent()), 12);

		// Write eventHandlers
		serializer->context.beginSection(HANDLER, 0); // Don´t set the event handler
		size_t numerOfEventHandlers = observer->getNumEventHandlers();
		for (size_t i = 0; i < numerOfEventHandlers; ++i)
		{
			ParticleEventHandler* eventHandler = observer->getEventHandler(i);
			ParticleEventHandlerFactory* particleEventHandlerFactory = ParticleSystemManager::getSingletonPtr()->getEventHandlerFactory(eventHandler->getEventHandlerType());
			if (particleEventHandlerFactory)
			{
				particleEventHandlerFactory->write(serializer, eventHandler);
			}
		}
		serializer->context.endSection();
	}
	//-----------------------------------------------------------------------
	void ParticleObserverTokens::ParticleTypeToObserveToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "ParticleTypeToObserveToken::parse");
		ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_PT_VISUAL))
		{
			observer->setParticleTypeToObserve(Particle::PT_VISUAL);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_EMITTER))
		{
			observer->setParticleTypeToObserve(Particle::PT_EMITTER);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_AFFECTOR))
		{
			observer->setParticleTypeToObserve(Particle::PT_AFFECTOR);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_SYSTEM))
		{
			observer->setParticleTypeToObserve(Particle::PT_SYSTEM);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_TECHNIQUE))
		{
			observer->setParticleTypeToObserve(Particle::PT_TECHNIQUE);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleObserverTokens::IntervalToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "ParticleObserverTokens::IntervalToken::parse");
		ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real updateInterval;
		deserializer->parseReal(updateInterval);
		observer->setObserverInterval(updateInterval);
	}
	//-----------------------------------------------------------------------
	void ParticleObserverTokens::ObserveUntilEventToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "ParticleObserverTokens::ObserveUntilEventToken::parse");
		ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
		bool observeUntilEvent;
		deserializer->parseBool(observeUntilEvent);
		observer->setObserveUntilEvent(observeUntilEvent);
	}

}
