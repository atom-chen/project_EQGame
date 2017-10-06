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
#include "ParticleEventHandlers/ParticleUniverseDoPlacementParticleEventHandler.h"
#include "ParticleEventHandlers/ParticleUniverseDoPlacementParticleEventHandlerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String DoPlacementParticleEventHandlerTokens::KEYWORD_FORCE_EMITTER = "force_emitter";
	const Ogre::String DoPlacementParticleEventHandlerTokens::KEYWORD_NUMBER_OF_PARTICLES = "number_of_particles";

	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandlerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_FORCE_EMITTER, &mForceEmitterToken);
		tokenRegister->addLexemeAction(KEYWORD_NUMBER_OF_PARTICLES, &mNumberOfParticlesToken);
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a DoPlacementParticleEventHandler
		const DoPlacementParticleEventHandler* eventHandler = static_cast<const DoPlacementParticleEventHandler*>(element);

		// Write the header of the DoPlacementParticleEventHandler
		serializer->writeLine(KEYWORD_HANDLER, eventHandler->getEventHandlerType(), eventHandler->getName(), 12);
		serializer->writeLine("{", 12);

		// Write base attributes
		ParticleEventHandlerTokens::write(serializer, element);

		// Write own attributes
		if (eventHandler->getForceEmitterName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_FORCE_EMITTER, eventHandler->getForceEmitterName(), 16);
		if (eventHandler->getNumberOfParticles() != DoPlacementParticleEventHandler::DEFAULT_NUMBER_OF_PARTICLES) serializer->writeLine(
			KEYWORD_NUMBER_OF_PARTICLES, Ogre::StringConverter::toString(eventHandler->getNumberOfParticles()), 16);

		// Write the close bracket
		serializer->writeLine("}", 12);
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandlerTokens::ForceEmitterToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoPlacementParticleEventHandlerTokens::ForceEmitterToken::parse");
		DoPlacementParticleEventHandler* handler = static_cast<DoPlacementParticleEventHandler*>(deserializer->context.getCurrentSectionElement());
		handler->setForceEmitterName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandlerTokens::NumberOfParticlesToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoPlacementParticleEventHandlerTokens::NumberOfParticlesToken::parse");
		DoPlacementParticleEventHandler* handler = static_cast<DoPlacementParticleEventHandler*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real numberOfParticles;
		deserializer->parseReal(numberOfParticles);
		handler->setNumberOfParticles(numberOfParticles);
	}

}
