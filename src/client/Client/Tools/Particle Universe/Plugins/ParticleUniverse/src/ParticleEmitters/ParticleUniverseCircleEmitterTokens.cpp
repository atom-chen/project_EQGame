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
#include "ParticleEmitters/ParticleUniverseCircleEmitter.h"
#include "ParticleEmitters/ParticleUniverseCircleEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String CircleEmitterTokens::KEYWORD_RADIUS = "circle_em_radius";
	const Ogre::String CircleEmitterTokens::KEYWORD_STEP = "circle_em_step";
	const Ogre::String CircleEmitterTokens::KEYWORD_ANGLE = "circle_em_angle";
	const Ogre::String CircleEmitterTokens::KEYWORD_RANDOM = "circle_em_random";
	const Ogre::String CircleEmitterTokens::KEYWORD_NORMAL = "circle_em_normal";

	//-----------------------------------------------------------------------
	void CircleEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RADIUS, &mRadiusToken);
		tokenRegister->addLexemeAction(KEYWORD_STEP, &mStepToken);
		tokenRegister->addLexemeAction(KEYWORD_ANGLE, &mCircleAngleToken);
		tokenRegister->addLexemeAction(KEYWORD_RANDOM, &mRandomToken);
		tokenRegister->addLexemeAction(KEYWORD_NORMAL, &mNormalToken);
	}
	//-----------------------------------------------------------------------
	void CircleEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a CircleEmitter
		const CircleEmitter* emitter = static_cast<const CircleEmitter*>(element);

		// Write the header of the CircleEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getRadius() != CircleEmitter::DEFAULT_RADIUS) serializer->writeLine(
			KEYWORD_RADIUS, Ogre::StringConverter::toString(emitter->getRadius()), 12);
		if (emitter->getStep() != CircleEmitter::DEFAULT_STEP) serializer->writeLine(
			KEYWORD_STEP, Ogre::StringConverter::toString(emitter->getStep()), 12);
		if (emitter->getCircleAngle() != CircleEmitter::DEFAULT_ANGLE) serializer->writeLine(
			KEYWORD_ANGLE, Ogre::StringConverter::toString(emitter->getCircleAngle()), 12);
		if (emitter->isRandom() != CircleEmitter::DEFAULT_RANDOM) serializer->writeLine(
			KEYWORD_RANDOM, Ogre::StringConverter::toString(emitter->isRandom()), 12);
		if (emitter->getNormal() != CircleEmitter::DEFAULT_NORMAL) serializer->writeLine(
			KEYWORD_NORMAL, Ogre::StringConverter::toString(emitter->getNormal()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void CircleEmitterTokens::RadiusToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "CircleEmitterTokens::RadiusToken::parse");
		CircleEmitter* emitter = static_cast<CircleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real radius;
		deserializer->parseReal(radius);
		emitter->setRadius(radius);
	}
	//----------------------------------------------------------------------- 
	void CircleEmitterTokens::NormalToken::parse(ParticleScriptDeserializer* deserializer) 
	{ 
		deserializer->context.validateCurrentSectionName(EMITTER, "CircleEmitterTokens::NormalToken::parse"); 
		CircleEmitter* emitter = static_cast<CircleEmitter*>(deserializer->context.getCurrentSectionElement()); 
		Ogre::Vector3 normal; 
		deserializer->parseVector3(normal); 
		emitter->setNormal(normal); 
	}
	//-----------------------------------------------------------------------
	void CircleEmitterTokens::CircleAngleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "CircleEmitterTokens::CircleAngleToken::parse");
		CircleEmitter* emitter = static_cast<CircleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real circleAngle;
		deserializer->parseReal(circleAngle);
		emitter->setCircleAngle(circleAngle);
	}
	//-----------------------------------------------------------------------
	void CircleEmitterTokens::StepToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "CircleEmitterTokens::StepToken::parse");
		CircleEmitter* emitter = static_cast<CircleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real step;
		deserializer->parseReal(step);
		emitter->setStep(step);
	}
	//-----------------------------------------------------------------------
	void CircleEmitterTokens::RandomToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "CircleEmitterTokens::RandomToken::parse");
		CircleEmitter* emitter = static_cast<CircleEmitter*>(deserializer->context.getCurrentSectionElement());
		bool random;
		deserializer->parseBool(random);
		emitter->setRandom(random);
	}

}
