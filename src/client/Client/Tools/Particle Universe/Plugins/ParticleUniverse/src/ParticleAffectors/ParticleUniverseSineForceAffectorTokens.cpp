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
#include "ParticleAffectors/ParticleUniverseSineForceAffector.h"
#include "ParticleAffectors/ParticleUniverseSineForceAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SineForceAffectorTokens::KEYWORD_MIN_FREQUENCY = "sinef_aff_frequency_min";
	const Ogre::String SineForceAffectorTokens::KEYWORD_MAX_FREQUENCY = "sinef_aff_frequency_max";

	//-----------------------------------------------------------------------
	void SineForceAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MIN_FREQUENCY, &mFrequencyMinToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_FREQUENCY, &mFrequencyMaxToken);
	}
	//-----------------------------------------------------------------------
	void SineForceAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SineForceAffector
		const SineForceAffector* affector = static_cast<const SineForceAffector*>(element);

		// Write the header of the SineForceAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ForceAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getFrequencyMin() != SineForceAffector::DEFAULT_FREQ_MIN) serializer->writeLine(
			KEYWORD_MIN_FREQUENCY, Ogre::StringConverter::toString(affector->getFrequencyMin()), 12);
		if (affector->getFrequencyMax() != SineForceAffector::DEFAULT_FREQ_MAX) serializer->writeLine(
			KEYWORD_MAX_FREQUENCY, Ogre::StringConverter::toString(affector->getFrequencyMax()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void SineForceAffectorTokens::FrequencyMinToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "SineForceAffectorTokens::FrequencyMinToken::parse");
		SineForceAffector* affector = static_cast<SineForceAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real frequencyMin;
		deserializer->parseReal(frequencyMin);
		affector->setFrequencyMin(frequencyMin);
	}
	//-----------------------------------------------------------------------
	void SineForceAffectorTokens::FrequencyMaxToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "SineForceAffectorTokens::FrequencyMaxToken::parse");
		SineForceAffector* affector = static_cast<SineForceAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real frequencyMax;
		deserializer->parseReal(frequencyMax);
		affector->setFrequencyMax(frequencyMax);
	}

}
