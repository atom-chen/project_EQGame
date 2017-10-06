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
#include "ParticleEventHandlers/ParticleUniverseDoEnableComponentEventHandler.h"
#include "ParticleEventHandlers/ParticleUniverseDoEnableComponentEventHandlerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String DoEnableComponentEventHandlerTokens::KEYWORD_ENABLE_COMPONENT = "enable_component";
	const Ogre::String DoEnableComponentEventHandlerTokens::KEYWORD_EMITTER_COMPONENT = "emitter_component";
	const Ogre::String DoEnableComponentEventHandlerTokens::KEYWORD_AFFECTOR_COMPONENT = "affector_component ";
	const Ogre::String DoEnableComponentEventHandlerTokens::KEYWORD_TECHNIQUE_COMPONENT = "technique_component";
	const Ogre::String DoEnableComponentEventHandlerTokens::KEYWORD_OBSERVER_COMPONENT = "observer_component";

	//-----------------------------------------------------------------------
	void DoEnableComponentEventHandlerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_ENABLE_COMPONENT, &mEnableComponentToken);
	}
	//-----------------------------------------------------------------------
	void DoEnableComponentEventHandlerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a DoEnableComponentEventHandler
		const DoEnableComponentEventHandler* eventHandler = static_cast<const DoEnableComponentEventHandler*>(element);

		// Write the header of the DoEnableComponentEventHandler
		serializer->writeLine(KEYWORD_HANDLER, eventHandler->getEventHandlerType(), eventHandler->getName(), 12);
		serializer->writeLine("{", 12);

		// Write base attributes
		ParticleEventHandlerTokens::write(serializer, element);

		// Write own attributes
		Ogre::String componentType = KEYWORD_EMITTER_COMPONENT;
		if (eventHandler->getComponentType() == ParticleSystem::CT_AFFECTOR) componentType = KEYWORD_AFFECTOR_COMPONENT;
		else if (eventHandler->getComponentType() == ParticleSystem::CT_TECHNIQUE) componentType = KEYWORD_TECHNIQUE_COMPONENT;
		else if (eventHandler->getComponentType() == ParticleSystem::CT_OBSERVER) componentType = KEYWORD_OBSERVER_COMPONENT;
		serializer->writeLine(
			KEYWORD_ENABLE_COMPONENT, 
			componentType, 
			eventHandler->getComponentName(), 
			Ogre::StringConverter::toString(eventHandler->isComponentEnabled()),
			16);

		// Write the close bracket
		serializer->writeLine("}", 12);
	}
	//-----------------------------------------------------------------------
	void DoEnableComponentEventHandlerTokens::EnableComponentToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(HANDLER, "DoEnableComponentEventHandlerTokens::ComponentNameToken::parse");
		DoEnableComponentEventHandler* handler = static_cast<DoEnableComponentEventHandler*>(deserializer->context.getCurrentSectionElement());
		switch(deserializer->getNextTokenID())
		{
			case ITokenRegister::ID_CT_EMITTER:
				{
					handler->setComponentType(ParticleSystem::CT_EMITTER);
				}
				break;

			case ITokenRegister::ID_CT_AFFECTOR:
				{
					handler->setComponentType(ParticleSystem::CT_AFFECTOR);
				}
				break;

			case ITokenRegister::ID_CT_OBSERVER:
				{
					handler->setComponentType(ParticleSystem::CT_OBSERVER);
				}
				break;

			case ITokenRegister::ID_CT_TECHNIQUE:
				{
					handler->setComponentType(ParticleSystem::CT_TECHNIQUE);
				}
				break;
		}

		handler->setComponentName(deserializer->getNextTokenLabel());

		bool enabled;
		deserializer->parseBool(enabled);
		handler->setComponentEnabled(enabled);
	}

}
