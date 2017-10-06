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
#include "ParticleObservers/ParticleUniverseOnVelocityObserver.h"
#include "ParticleObservers/ParticleUniverseOnVelocityObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnVelocityObserverTokens::KEYWORD_VELOCITY_THRESHOLD = "velocity_threshold";

	//-----------------------------------------------------------------------
	void OnVelocityObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_VELOCITY_THRESHOLD, &mVelocityToken);
	}
	//-----------------------------------------------------------------------
	void OnVelocityObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnVelocityObserver
		const OnVelocityObserver* observer = static_cast<const OnVelocityObserver*>(element);

		// Write the header of the OnVelocityObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		Ogre::String compare = KEYWORD_CO_GREATER_THAN;
		if (observer->getCompare() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
		serializer->writeLine(KEYWORD_VELOCITY_THRESHOLD, compare, Ogre::StringConverter::toString(observer->getThreshold()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnVelocityObserverTokens::VelocityToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnVelocityObserverTokens::VelocityToken::parse");
		OnVelocityObserver* observer = static_cast<OnVelocityObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setCompare(CO_LESS_THAN);
		}
		else
		{
			observer->setCompare(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setThreshold(threshold);
	}

}
