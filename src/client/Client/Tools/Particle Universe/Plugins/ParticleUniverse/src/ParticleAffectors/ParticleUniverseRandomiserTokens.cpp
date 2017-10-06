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
#include "ParticleAffectors/ParticleUniverseRandomiser.h"
#include "ParticleAffectors/ParticleUniverseRandomiserTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String RandomiserTokens::KEYWORD_MAX_DEVIATION_X = "rand_aff_max_deviation_x";
	const Ogre::String RandomiserTokens::KEYWORD_MAX_DEVIATION_Y = "rand_aff_max_deviation_y";
	const Ogre::String RandomiserTokens::KEYWORD_MAX_DEVIATION_Z = "rand_aff_max_deviation_z";
	const Ogre::String RandomiserTokens::KEYWORD_TIME_STEP = "rand_aff_time_step";
	const Ogre::String RandomiserTokens::KEYWORD_RANDOM_DIRECTION = "rand_aff_direction";

	//-----------------------------------------------------------------------
	void RandomiserTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MAX_DEVIATION_X, &mDeviationXToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_DEVIATION_Y, &mDeviationYToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_DEVIATION_Z, &mDeviationZToken);
		tokenRegister->addLexemeAction(KEYWORD_TIME_STEP, &mTimeStepToken);
		tokenRegister->addLexemeAction(KEYWORD_RANDOM_DIRECTION, &mRandomDirectionToken);
		
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a Randomiser
		const Randomiser* affector = static_cast<const Randomiser*>(element);

		// Write the header of the Randomiser
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getMaxDeviationX() != Randomiser::DEFAULT_MAX_DEVIATION.x) serializer->writeLine(
			KEYWORD_MAX_DEVIATION_X, Ogre::StringConverter::toString(affector->getMaxDeviationX()), 12);
		if (affector->getMaxDeviationY() != Randomiser::DEFAULT_MAX_DEVIATION.y) serializer->writeLine(
			KEYWORD_MAX_DEVIATION_Y, Ogre::StringConverter::toString(affector->getMaxDeviationY()), 12);
		if (affector->getMaxDeviationZ() != Randomiser::DEFAULT_MAX_DEVIATION.z) serializer->writeLine(
			KEYWORD_MAX_DEVIATION_Z, Ogre::StringConverter::toString(affector->getMaxDeviationZ()), 12);
		if (affector->getTimeStep() != Randomiser::DEFAULT_TIME_STEP) serializer->writeLine(
			KEYWORD_TIME_STEP, Ogre::StringConverter::toString(affector->getTimeStep()), 12);
		if (affector->isRandomDirection() != Randomiser::DEFAULT_RANDOM_DIRECTION) serializer->writeLine(
			KEYWORD_RANDOM_DIRECTION, Ogre::StringConverter::toString(affector->isRandomDirection()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::DeviationXToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "RandomiserTokens::DeviationToken::parse");
		Randomiser* affector = static_cast<Randomiser*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real deviationX;
		deserializer->parseReal(deviationX);
		affector->setMaxDeviationX(deviationX);
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::DeviationYToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "RandomiserTokens::DeviationToken::parse");
		Randomiser* affector = static_cast<Randomiser*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real deviationY;
		deserializer->parseReal(deviationY);
		affector->setMaxDeviationY(deviationY);
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::DeviationZToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "RandomiserTokens::DeviationToken::parse");
		Randomiser* affector = static_cast<Randomiser*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real deviationZ;
		deserializer->parseReal(deviationZ);
		affector->setMaxDeviationZ(deviationZ);
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::RandomDirectionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "RandomiserTokens::RandomDirectionToken::parse");
		Randomiser* affector = static_cast<Randomiser*>(deserializer->context.getCurrentSectionElement());
		bool randomDirection;
		deserializer->parseBool(randomDirection);
		affector->setRandomDirection(randomDirection);
	}
	//-----------------------------------------------------------------------
	void RandomiserTokens::TimeStepToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "RandomiserTokens::TimeStepToken::parse");
		Randomiser* affector = static_cast<Randomiser*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real timeStepToken;
		deserializer->parseReal(timeStepToken);
		affector->setTimeStep(timeStepToken);
	}

}
