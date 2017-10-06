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
#include "ParticleEmitters/ParticleUniverseBoxEmitter.h"
#include "ParticleEmitters/ParticleUniverseBoxEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String BoxEmitterTokens::KEYWORD_HEIGHT = "box_em_height";
	const Ogre::String BoxEmitterTokens::KEYWORD_WIDTH = "box_em_width";
	const Ogre::String BoxEmitterTokens::KEYWORD_DEPTH = "box_em_depth";

	//-----------------------------------------------------------------------
	void BoxEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_HEIGHT, &mHeightToken);
		tokenRegister->addLexemeAction(KEYWORD_WIDTH, &mWidthToken);
		tokenRegister->addLexemeAction(KEYWORD_DEPTH, &mDepthToken);
	}
	//-----------------------------------------------------------------------
	void BoxEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a BoxEmitter
		const BoxEmitter* emitter = static_cast<const BoxEmitter*>(element);

		// Write the header of the BoxEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getWidth() != BoxEmitter::DEFAULT_WIDTH) serializer->writeLine(
			KEYWORD_WIDTH, Ogre::StringConverter::toString(emitter->getWidth()), 12);
		if (emitter->getHeight() != BoxEmitter::DEFAULT_HEIGHT) serializer->writeLine(
			KEYWORD_HEIGHT, Ogre::StringConverter::toString(emitter->getHeight()), 12);
		if (emitter->getDepth() != BoxEmitter::DEFAULT_DEPTH) serializer->writeLine(
			KEYWORD_DEPTH, Ogre::StringConverter::toString(emitter->getDepth()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void BoxEmitterTokens::HeightToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "BoxEmitterTokens::HeightToken::parse");
		BoxEmitter* emitter = static_cast<BoxEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real height;
		deserializer->parseReal(height);
		emitter->setHeight(height);
	}
	//-----------------------------------------------------------------------
	void BoxEmitterTokens::WidthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "BoxEmitterTokens::WidthToken::parse");
		BoxEmitter* emitter = static_cast<BoxEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real width;
		deserializer->parseReal(width);
		emitter->setWidth(width);
	}
	//-----------------------------------------------------------------------
	void BoxEmitterTokens::DepthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "BoxEmitterTokens::DepthToken::parse");
		BoxEmitter* emitter = static_cast<BoxEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real depth;
		deserializer->parseReal(depth);
		emitter->setDepth(depth);
	}

}
