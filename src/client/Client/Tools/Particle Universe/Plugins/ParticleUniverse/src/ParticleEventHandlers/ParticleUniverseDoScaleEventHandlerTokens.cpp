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
#include "ParticleEventHandlers/ParticleUniverseDoScaleEventHandler.h"
#include "ParticleEventHandlers/ParticleUniverseDoScaleEventHandlerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String DoScaleEventHandlerTokens::KEYWORD_SCALE_FRACTION = "scale_fraction";
	const Ogre::String DoScaleEventHandlerTokens::KEYWORD_SCALE_TYPE = "scale_type";
	const Ogre::String DoScaleEventHandlerTokens::KEYWORD_SCALE_TIME_TO_LIVE = "st_time_to_live";
	const Ogre::String DoScaleEventHandlerTokens::KEYWORD_SCALE_VELOCITY = "st_velocity";

	//-----------------------------------------------------------------------
	void DoScaleEventHandlerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_SCALE_FRACTION, &mScaleFractionToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE_TYPE, &mScaleTypeToken);
	}
	//-----------------------------------------------------------------------
	void DoScaleEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a DoScaleEventHandler
		const DoScaleEventHandler* eventHandler = static_cast<const DoScaleEventHandler*>(element);

		// Write the header of the DoScaleEventHandler
		serializer->writeLine(KEYWORD_HANDLER, eventHandler->getEventHandlerType(), eventHandler->getName(), 12);
		serializer->writeLine("{", 12);

		// Write base attributes
		ParticleEventHandlerTokens::write(serializer, element);

		// Write own attributes
		if (eventHandler->getScaleFraction() != DoScaleEventHandler::DEFAULT_SCALE_FRACTION) serializer->writeLine(
			KEYWORD_SCALE_FRACTION, Ogre::StringConverter::toString(eventHandler->getScaleFraction()), 16);
		Ogre::String scaleType = KEYWORD_SCALE_TIME_TO_LIVE;
		if (eventHandler->getScaleType() == DoScaleEventHandler::ST_VELOCITY) scaleType = KEYWORD_SCALE_VELOCITY;
		serializer->writeLine(KEYWORD_SCALE_TYPE, scaleType, 16);

		// Write the close bracket
		serializer->writeLine("}", 12);
	}
	//-----------------------------------------------------------------------
	void DoScaleEventHandlerTokens::ScaleFractionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoScaleEventHandlerTokens::ScaleFractionToken::parse");
		DoScaleEventHandler* handler = static_cast<DoScaleEventHandler*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real scaleFraction;
		deserializer->parseReal(scaleFraction);
		handler->setScaleFraction(scaleFraction);
	}
	//-----------------------------------------------------------------------
	void DoScaleEventHandlerTokens::ScaleTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoScaleEventHandlerTokens::ScaleTypeToken::parse");
		DoScaleEventHandler* handler = static_cast<DoScaleEventHandler*>(deserializer->context.getCurrentSectionElement());

		// Parse the string. A better solution would be to use Id's instead.
		Ogre::String type = deserializer->getNextTokenLabel();
		if (type == KEYWORD_SCALE_TIME_TO_LIVE)
		{
			handler->setScaleType(DoScaleEventHandler::ST_TIME_TO_LIVE);
		}
		else if (type == KEYWORD_SCALE_VELOCITY)
		{
			handler->setScaleType(DoScaleEventHandler::ST_VELOCITY);
		}
	}

}
