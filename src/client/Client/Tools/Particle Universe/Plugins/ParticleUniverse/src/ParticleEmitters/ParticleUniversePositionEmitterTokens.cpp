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
#include "ParticleEmitters/ParticleUniversePositionEmitter.h"
#include "ParticleEmitters/ParticleUniversePositionEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String PositionEmitterTokens::KEYWORD_POSITION = "add_position";
	const Ogre::String PositionEmitterTokens::KEYWORD_RANDOMIZE = "random_position";

	//-----------------------------------------------------------------------
	void PositionEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_POSITION, &mPositionToken);
		tokenRegister->addLexemeAction(KEYWORD_RANDOMIZE, &mRandomizedToken);
	}
	//-----------------------------------------------------------------------
	void PositionEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a PositionEmitter
		const PositionEmitter* emitter = static_cast<const PositionEmitter*>(element);

		// Write the header of the PositionEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->isRandomized() != PositionEmitter::DEFAULT_RANDOMIZE) serializer->writeLine(
			KEYWORD_RANDOMIZE, Ogre::StringConverter::toString(emitter->isRandomized()), 12);

		// List of positions
		std::vector<Ogre::Vector3> positionList = emitter->getPositions();
		if(!positionList.empty())
		{
			std::vector<Ogre::Vector3>::const_iterator it;
			std::vector<Ogre::Vector3>::const_iterator itEnd = positionList.end();
			for (it = positionList.begin(); it != itEnd; ++it)
			{
				serializer->writeLine(KEYWORD_POSITION, Ogre::StringConverter::toString(*it), 12);
			}
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void PositionEmitterTokens::RandomizedToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "PositionEmitterTokens::RandomizedToken::parse");
		PositionEmitter* emitter = static_cast<PositionEmitter*>(deserializer->context.getCurrentSectionElement());
		bool randomized;
		deserializer->parseBool(randomized);
		emitter->setRandomized(randomized);
	}
	//-----------------------------------------------------------------------
	void PositionEmitterTokens::PositionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "PositionEmitterTokens::PositionToken::parse");
		PositionEmitter* emitter = static_cast<PositionEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 position;
		deserializer->parseVector3(position);
		emitter->addPosition(position);
	}

}
