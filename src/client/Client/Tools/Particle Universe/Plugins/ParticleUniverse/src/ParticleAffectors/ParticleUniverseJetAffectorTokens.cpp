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
#include "ParticleAffectors/ParticleUniverseJetAffector.h"
#include "ParticleAffectors/ParticleUniverseJetAffectorTokens.h"

namespace ParticleUniverse
{
//	const Ogre::String JetAffectorTokens::KEYWORD_AFFECTOR = "affector";
	const Ogre::String JetAffectorTokens::KEYWORD_ACCELERATION = "jet_aff_accel";

	//-----------------------------------------------------------------------
	void JetAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_ACCELERATION, &mAccelerationToken);
	}
	//-----------------------------------------------------------------------
	void JetAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a JetAffector
		const JetAffector* affector = static_cast<const JetAffector*>(element);

		// Write the header of the JetAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(affector->getDynAcceleration()) != JetAffector::DEFAULT_ACCELERATION)
		{
			serializer->setKeyword(KEYWORD_ACCELERATION);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, affector->getDynAcceleration());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void JetAffectorTokens::AccelerationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "JetAffectorTokens::AccelerationToken::parse");
		JetAffector* affector = static_cast<JetAffector*>(deserializer->context.getCurrentSectionElement());
		affector->setDynAcceleration(deserializer->parseDynamicAttribute());
	}

}
