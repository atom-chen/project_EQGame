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
#include "ParticleEmitters/ParticleUniverseSlaveEmitter.h"
#include "ParticleEmitters/ParticleUniverseSlaveEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SlaveEmitterTokens::KEYWORD_MASTER_TECHNIQUE = "master_technique_name";
	const Ogre::String SlaveEmitterTokens::KEYWORD_MASTER_EMITTER = "master_emitter_name";

	//-----------------------------------------------------------------------
	void SlaveEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MASTER_TECHNIQUE, &mMasterTechniqueNameToken);
		tokenRegister->addLexemeAction(KEYWORD_MASTER_EMITTER, &mMasterEmitterNameToken);
	}
	//-----------------------------------------------------------------------
	void SlaveEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SlaveEmitter
		const SlaveEmitter* emitter = static_cast<const SlaveEmitter*>(element);

		// Write the header of the SlaveEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getMasterTechniqueName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_MASTER_TECHNIQUE, emitter->getMasterTechniqueName(), 12);
		if (emitter->getMasterEmitterName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_MASTER_EMITTER, emitter->getMasterEmitterName(), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void SlaveEmitterTokens::MasterTechniqueNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "SlaveEmitterTokens::MasterTechniqueNameToken::parse");
		SlaveEmitter* emitter = static_cast<SlaveEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setMasterTechniqueName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void SlaveEmitterTokens::MasterEmitterNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "SlaveEmitterTokens::MasterEmitterNameToken::parse");
		SlaveEmitter* emitter = static_cast<SlaveEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setMasterEmitterName(deserializer->getNextTokenLabel());
	}

}
