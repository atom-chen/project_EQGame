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
#include "ParticleObservers/ParticleUniverseOnPositionObserver.h"
#include "ParticleObservers/ParticleUniverseOnPositionObserverTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String OnPositionObserverTokens::KEYWORD_POSITION_X = "position_x";
	const Ogre::String OnPositionObserverTokens::KEYWORD_POSITION_Y = "position_y";
	const Ogre::String OnPositionObserverTokens::KEYWORD_POSITION_Z = "position_z";

	//-----------------------------------------------------------------------
	void OnPositionObserverTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_POSITION_X, &mPositionXThreshold);
		tokenRegister->addLexemeAction(KEYWORD_POSITION_Y, &mPositionYThreshold);
		tokenRegister->addLexemeAction(KEYWORD_POSITION_Z, &mPositionZThreshold);
	}
	//-----------------------------------------------------------------------
	void OnPositionObserverTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a OnPositionObserver
		const OnPositionObserver* observer = static_cast<const OnPositionObserver*>(element);

		// Write the header of the OnPositionObserver
		serializer->writeLine(KEYWORD_OBSERVER, observer->getObserverType(), observer->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleObserverTokens::write(serializer, element);

		// Write own attributes
		if (observer->isPositionXThresholdSet())
		{
			Ogre::String compare = KEYWORD_CO_GREATER_THAN;
			if (observer->getComparePositionX() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
			serializer->writeLine(KEYWORD_POSITION_X, compare, Ogre::StringConverter::toString(observer->getPositionXThreshold()), 12);
		}
		if (observer->isPositionYThresholdSet())
		{
			Ogre::String compare = KEYWORD_CO_GREATER_THAN;
			if (observer->getComparePositionY() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
			serializer->writeLine(KEYWORD_POSITION_Y, compare, Ogre::StringConverter::toString(observer->getPositionYThreshold()), 12);
		}
		if (observer->isPositionZThresholdSet())
		{
			Ogre::String compare = KEYWORD_CO_GREATER_THAN;
			if (observer->getComparePositionZ() == CO_LESS_THAN) compare = KEYWORD_LESS_THAN;
			serializer->writeLine(KEYWORD_POSITION_Z, compare, Ogre::StringConverter::toString(observer->getPositionZThreshold()), 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void OnPositionObserverTokens::PositionXThreshold::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnPositionObserverTokens::PositionXThreshold::parse");
		OnPositionObserver* observer = static_cast<OnPositionObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setComparePositionX(CO_LESS_THAN);
		}
		else
		{
			observer->setComparePositionX(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setPositionXThreshold(threshold);
	}
	//-----------------------------------------------------------------------
	void OnPositionObserverTokens::PositionYThreshold::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnPositionObserverTokens::PositionYThreshold::parse");
		OnPositionObserver* observer = static_cast<OnPositionObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setComparePositionY(CO_LESS_THAN);
		}
		else
		{
			observer->setComparePositionY(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setPositionYThreshold(threshold);
	}
	//-----------------------------------------------------------------------
	void OnPositionObserverTokens::PositionZThreshold::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(OBSERVER, "OnPositionObserverTokens::PositionZThreshold::parse");
		OnPositionObserver* observer = static_cast<OnPositionObserver*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_COMPARE_LESS_THAN))
		{
			observer->setComparePositionZ(CO_LESS_THAN);
		}
		else
		{
			observer->setComparePositionZ(CO_GREATER_THAN);
		}

		deserializer->getNextTokenID();
		Ogre::Real threshold;
		deserializer->parseReal(threshold);
		observer->setPositionZThreshold(threshold);
	}

}
