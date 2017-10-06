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
#include "ParticleEmitters/ParticleUniverseLineEmitter.h"
#include "ParticleEmitters/ParticleUniverseLineEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String LineEmitterTokens::KEYWORD_END = "line_em_end";
	const Ogre::String LineEmitterTokens::KEYWORD_MAX_INCREMENT = "line_em_max_increment";
	const Ogre::String LineEmitterTokens::KEYWORD_MIN_INCREMENT = "line_em_min_increment";
	const Ogre::String LineEmitterTokens::KEYWORD_MAX_DEVIATION = "line_em_max_deviation";

	//-----------------------------------------------------------------------
	void LineEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_END, &mLineEndToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_INCREMENT, &mMaxIncrementToken);
		tokenRegister->addLexemeAction(KEYWORD_MIN_INCREMENT, &mMinIncrementToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_DEVIATION, &mMaxDeviationToken);
	}
	//-----------------------------------------------------------------------
	void LineEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a LineEmitter
		const LineEmitter* emitter = static_cast<const LineEmitter*>(element);

		// Write the header of the LineEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getEnd() != LineEmitter::DEFAULT_END) serializer->writeLine(
			KEYWORD_END, Ogre::StringConverter::toString(emitter->getEnd()), 12);
		if (emitter->getMinIncrement() != LineEmitter::DEFAULT_MIN_INCREMENT) serializer->writeLine(
			KEYWORD_MIN_INCREMENT, Ogre::StringConverter::toString(emitter->getMinIncrement()), 12);
		if (emitter->getMaxIncrement() != LineEmitter::DEFAULT_MAX_INCREMENT) serializer->writeLine(
			KEYWORD_MAX_INCREMENT, Ogre::StringConverter::toString(emitter->getMaxIncrement()), 12);
		if (emitter->getMaxDeviation() != LineEmitter::DEFAULT_MAX_DEVIATION) serializer->writeLine(
			KEYWORD_MAX_DEVIATION, Ogre::StringConverter::toString(emitter->getMaxDeviation()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void LineEmitterTokens::LineEndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "LineEmitterTokens::LineEndToken::parse");
		LineEmitter* emitter = static_cast<LineEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 end;
		deserializer->parseVector3(end);
		emitter->setEnd(end);
	}
	//-----------------------------------------------------------------------
	void LineEmitterTokens::MaxIncrementToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "LineEmitterTokens::MaxIncrementToken::parse");
		LineEmitter* emitter = static_cast<LineEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real max;
		deserializer->parseReal(max);
		emitter->setMaxIncrement(max);
	}
	//-----------------------------------------------------------------------
	void LineEmitterTokens::MinIncrementToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "LineEmitterTokens::MinIncrementToken::parse");
		LineEmitter* emitter = static_cast<LineEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real min;
		deserializer->parseReal(min);
		emitter->setMinIncrement(min);
	}
	//-----------------------------------------------------------------------
	void LineEmitterTokens::MaxDeviationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "LineEmitterTokens::MaxDeviationToken::parse");
		LineEmitter* emitter = static_cast<LineEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real deviation;
		deserializer->parseReal(deviation);
		emitter->setMaxDeviation(deviation);
	}

}
