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
#include "ParticleEventHandlers/ParticleUniverseDoAffectorEventHandler.h"
#include "ParticleEventHandlers/ParticleUniverseDoAffectorEventHandlerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String DoAffectorEventHandlerTokens::KEYWORD_FORCE_AFFECTOR = "force_affector";
	const Ogre::String DoAffectorEventHandlerTokens::KEYWORD_PRE_POST = "pre_post";

	//-----------------------------------------------------------------------
	void DoAffectorEventHandlerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_FORCE_AFFECTOR, &mAffectorToken);
		tokenRegister->addLexemeAction(KEYWORD_PRE_POST, &mPrePostToken);
	}
	//-----------------------------------------------------------------------
	void DoAffectorEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a DoAffectorEventHandler
		const DoAffectorEventHandler* eventHandler = static_cast<const DoAffectorEventHandler*>(element);

		// Write the header of the DoAffectorEventHandler
		serializer->writeLine(KEYWORD_HANDLER, eventHandler->getEventHandlerType(), eventHandler->getName(), 12);
		serializer->writeLine("{", 12);

		// Write base attributes
		ParticleEventHandlerTokens::write(serializer, element);

		// Write own attributes
		if (eventHandler->getAffectorName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_FORCE_AFFECTOR, eventHandler->getAffectorName(), 16);
		if (eventHandler->getPrePost() != DoAffectorEventHandler::DEFAULT_PRE_POST) serializer->writeLine(
			KEYWORD_PRE_POST, Ogre::StringConverter::toString(eventHandler->getPrePost()), 16);

		// Write the close bracket
		serializer->writeLine("}", 12);
	}
	//-----------------------------------------------------------------------
	void DoAffectorEventHandlerTokens::AffectorToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoAffectorEventHandlerTokens::ComponentNameToken::parse");
		DoAffectorEventHandler* handler = static_cast<DoAffectorEventHandler*>(deserializer->context.getCurrentSectionElement());
		handler->setAffectorName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void DoAffectorEventHandlerTokens::PrePostToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoAffectorEventHandlerTokens::PrePostToken::parse");
		DoAffectorEventHandler* handler = static_cast<DoAffectorEventHandler*>(deserializer->context.getCurrentSectionElement());
		bool prePost;
		deserializer->parseBool(prePost);
		handler->setPrePost(prePost);
	}

}
