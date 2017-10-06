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
#include "ParticleAffectors/ParticleUniverseColourAffector.h"
#include "ParticleAffectors/ParticleUniverseColourAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ColourAffectorTokens::KEYWORD_TIME_COLOUR = "colour_aff_time_colour";
	const Ogre::String ColourAffectorTokens::KEYWORD_COLOUR_OPERATION = "colour_operation";
	const Ogre::String ColourAffectorTokens::KEYWORD_MULTIPLY = "multiply";
	const Ogre::String ColourAffectorTokens::KEYWORD_SET = "set";

	//-----------------------------------------------------------------------
	void ColourAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_TIME_COLOUR, &mTimeColourToken);
		tokenRegister->addLexemeAction(KEYWORD_COLOUR_OPERATION, &mColourOperationToken);
	}
	//-----------------------------------------------------------------------
	void ColourAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ColourAffector
		const ColourAffector* affector = static_cast<const ColourAffector*>(element);

		// Write the header of the ColourAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		ColourAffector::ColourMap colourMap = affector->getTimeAndColour();
		ColourAffector::ColourMap::const_iterator it;
		ColourAffector::ColourMap::const_iterator itEnd = colourMap.end();
		Ogre::Real time;
		Ogre::ColourValue colour;
		for (it = colourMap.begin(); it != itEnd; ++it)
		{
			time = it->first;
			colour = it->second;
			serializer->writeLine(KEYWORD_TIME_COLOUR, Ogre::StringConverter::toString(time), Ogre::String(" ") + Ogre::StringConverter::toString(colour), 12);
		}

		if (affector->getColourOperation() != ColourAffector::DEFAULT_COLOUR_OPERATION) 
		{
			Ogre::String colourOperation = KEYWORD_MULTIPLY;
			if (affector->getColourOperation() == ColourAffector::CAO_SET) colourOperation = KEYWORD_SET;
			serializer->writeLine(KEYWORD_COLOUR_OPERATION, colourOperation, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void ColourAffectorTokens::TimeColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ColourAffectorTokens::TimeColourToken::parse");
		ColourAffector* affector = static_cast<ColourAffector*>(deserializer->context.getCurrentSectionElement());

		// Add time + colour to the map
		Ogre::Real timeFraction;
		deserializer->parseReal(timeFraction);
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		affector->addColour(timeFraction, colour);
	}
	//-----------------------------------------------------------------------
	void ColourAffectorTokens::ColourOperationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ColourAffectorTokens::ColourOperationToken::parse");
		ColourAffector* affector = static_cast<ColourAffector*>(deserializer->context.getCurrentSectionElement());

		Ogre::String operation = deserializer->getNextTokenLabel();
		if (operation == KEYWORD_MULTIPLY)
		{
			affector->setColourOperation(ColourAffector::CAO_MULTIPLY);
		}
		else if (operation == KEYWORD_SET)
		{
			affector->setColourOperation(ColourAffector::CAO_SET);
		}
		else
		{
			affector->setColourOperation(ColourAffector::CAO_SET);
		}
	}

}
