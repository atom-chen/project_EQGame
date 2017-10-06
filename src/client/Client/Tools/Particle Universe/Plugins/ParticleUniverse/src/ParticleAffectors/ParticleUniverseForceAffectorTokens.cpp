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
#include "ParticleAffectors/ParticleUniverseForceAffector.h"
#include "ParticleAffectors/ParticleUniverseForceAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ForceAffectorTokens::KEYWORD_FORCE_VECTOR = "force_aff_vector";
	const Ogre::String ForceAffectorTokens::KEYWORD_APPLICATION = "force_aff_application";
	const Ogre::String ForceAffectorTokens::KEYWORD_ADD = "add";
	const Ogre::String ForceAffectorTokens::KEYWORD_AVERAGE = "average";

	//-----------------------------------------------------------------------
	void ForceAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_FORCE_VECTOR, &mForceVectorToken);
		tokenRegister->addLexemeAction(KEYWORD_APPLICATION, &mForceApplicationToken);

		// Register token id's
		tokenRegister->addTokenId(KEYWORD_ADD, ITokenRegister::ID_FA_ADD);
		tokenRegister->addTokenId(KEYWORD_AVERAGE, ITokenRegister::ID_FA_AVERAGE);
	}
	//-----------------------------------------------------------------------
	void ForceAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ForceAffector
		const ForceAffector* affector = static_cast<const ForceAffector*>(element);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getForceVector() != ForceAffector::DEFAULT_FORCE_VECTOR) serializer->writeLine(
			KEYWORD_FORCE_VECTOR, Ogre::StringConverter::toString(affector->getForceVector()), 12);

		if (affector->getForceApplication() != ForceAffector::DEFAULT_FORCE_APPL) 
		{
			Ogre::String application = KEYWORD_ADD;
			if (affector->getForceApplication() == ForceAffector::FA_AVERAGE) application = KEYWORD_AVERAGE;
			serializer->writeLine(KEYWORD_APPLICATION, application, 12);
		}
	}
	//-----------------------------------------------------------------------
	void ForceAffectorTokens::ForceVectorToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ForceAffectorTokens::ForceVectorToken::parse");
		ForceAffector* affector = static_cast<ForceAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 forceVector;
		deserializer->parseVector3(forceVector);
		affector->setForceVector(forceVector);
	}
	//-----------------------------------------------------------------------
	void ForceAffectorTokens::ForceApplicationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ForceAffectorTokens::ForceApplicationToken::parse");
		ForceAffector* affector = static_cast<ForceAffector*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_FA_ADD))
		{
			affector->setForceApplication(ForceAffector::FA_ADD);
		}
		else
		{
			affector->setForceApplication(ForceAffector::FA_AVERAGE);
		}
		deserializer->getNextTokenID();
	}

}
