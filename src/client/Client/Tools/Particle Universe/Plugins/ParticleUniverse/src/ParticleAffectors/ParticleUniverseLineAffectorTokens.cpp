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
#include "ParticleAffectors/ParticleUniverseLineAffector.h"
#include "ParticleAffectors/ParticleUniverseLineAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String LineAffectorTokens::KEYWORD_MAX_DEVIATION = "line_aff_max_deviation";
	const Ogre::String LineAffectorTokens::KEYWORD_TIME_STEP = "line_aff_time_step";
	const Ogre::String LineAffectorTokens::KEYWORD_END = "line_aff_end";
	const Ogre::String LineAffectorTokens::KEYWORD_DRIFT = "line_aff_drift";

	//-----------------------------------------------------------------------
	void LineAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MAX_DEVIATION, &mDeviationToken);
		tokenRegister->addLexemeAction(KEYWORD_TIME_STEP, &mTimeStepToken);
		tokenRegister->addLexemeAction(KEYWORD_END, &mEndToken);
		tokenRegister->addLexemeAction(KEYWORD_DRIFT, &mDriftToken);

	}
	//-----------------------------------------------------------------------
	void LineAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a LineAffector
		const LineAffector* affector = static_cast<const LineAffector*>(element);

		// Write the header of the LineAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getMaxDeviation() != LineAffector::DEFAULT_MAX_DEVIATION) serializer->writeLine(
			KEYWORD_MAX_DEVIATION, Ogre::StringConverter::toString(affector->getMaxDeviation()), 12);
		if (affector->getTimeStep() != LineAffector::DEFAULT_TIME_STEP) serializer->writeLine(
			KEYWORD_TIME_STEP, Ogre::StringConverter::toString(affector->getTimeStep()), 12);
		if (affector->getEnd() != LineAffector::DEFAULT_END) serializer->writeLine(
			KEYWORD_END, Ogre::StringConverter::toString(affector->getEnd()), 12);
		if (affector->getDrift() != LineAffector::DEFAULT_DRIFT) serializer->writeLine(
			KEYWORD_DRIFT, Ogre::StringConverter::toString(affector->getDrift()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void LineAffectorTokens::DeviationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "LineAffectorTokens::DeviationToken::parse");
		LineAffector* affector = static_cast<LineAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real deviation;
		deserializer->parseReal(deviation);
		affector->setMaxDeviation(deviation);
	}
	//-----------------------------------------------------------------------
	void LineAffectorTokens::TimeStepToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "LineAffectorTokens::TimeStepToken::parse");
		LineAffector* affector = static_cast<LineAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real timeStep;
		deserializer->parseReal(timeStep);
		affector->setTimeStep(timeStep);
	}
	//-----------------------------------------------------------------------
	void LineAffectorTokens::EndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "LineAffectorTokens::EndToken::parse");
		LineAffector* affector = static_cast<LineAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 end;
		deserializer->parseVector3(end);
		affector->setEnd(end);
	}
	//-----------------------------------------------------------------------
	void LineAffectorTokens::DriftToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "LineAffectorTokens::DriftToken::parse");
		LineAffector* affector = static_cast<LineAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real drift;
		deserializer->parseReal(drift);
		affector->setDrift(drift);
	}

}
