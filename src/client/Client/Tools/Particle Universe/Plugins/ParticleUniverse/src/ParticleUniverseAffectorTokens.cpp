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
#include "ParticleUniverseAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleAffectorTokens::KEYWORD_AFFECTOR = "affector";
	const Ogre::String ParticleAffectorTokens::KEYWORD_ENABLED = "enabled";
	const Ogre::String ParticleAffectorTokens::KEYWORD_POSITION = "position";
	const Ogre::String ParticleAffectorTokens::KEYWORD_MASS = "mass_affector";
	const Ogre::String ParticleAffectorTokens::KEYWORD_EXCLUDE_EMITTER = "exclude_emitter";
	const Ogre::String ParticleAffectorTokens::KEYWORD_SPECIALISATION = "affect_specialisation";
	const Ogre::String ParticleAffectorTokens::KEYWORD_SPEC_DEFAULT = "special_default";
	const Ogre::String ParticleAffectorTokens::KEYWORD_SPEC_TTL_INCREASE = "special_ttl_increase";
	const Ogre::String ParticleAffectorTokens::KEYWORD_SPEC_TTL_DECREASE = "special_ttl_decrease";

	//-----------------------------------------------------------------------
	void ParticleAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MASS, &mAffectorMassToken);
		tokenRegister->addLexemeAction(KEYWORD_EXCLUDE_EMITTER, &mExcludeEmitterToken);
		tokenRegister->addLexemeAction(KEYWORD_SPECIALISATION, &mAffectSpecialisationToken);

		tokenRegister->addTokenId(KEYWORD_SPEC_DEFAULT, ITokenRegister::ID_AFSP_DEFAULT);
		tokenRegister->addTokenId(KEYWORD_SPEC_TTL_INCREASE, ITokenRegister::ID_AFSP_TTL_INCREASE);
		tokenRegister->addTokenId(KEYWORD_SPEC_TTL_DECREASE, ITokenRegister::ID_AFSP_TTL_DECREASE);
	}
	//-----------------------------------------------------------------------
	void ParticleAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleAffector
		const ParticleAffector* affector = static_cast<const ParticleAffector*>(element);

		// Write base attributes
		if (affector->_getOriginalEnabled() != ParticleAffector::DEFAULT_ENABLED) serializer->writeLine(
			KEYWORD_ENABLED, Ogre::StringConverter::toString(affector->_getOriginalEnabled()), 12);
		if (affector->position != ParticleAffector::DEFAULT_POSITION) serializer->writeLine(
			KEYWORD_POSITION, Ogre::StringConverter::toString(affector->position), 12);
		if (affector->mass != ParticleAffector::DEFAULT_MASS) serializer->writeLine(
			KEYWORD_MASS, Ogre::StringConverter::toString(affector->mass), 12);
		Ogre::String affectSpecialisation = KEYWORD_SPEC_DEFAULT;
		if (affector->getAffectSpecialisation() == ParticleAffector::AFSP_TTL_INCREASE)
		{
			affectSpecialisation = KEYWORD_SPEC_TTL_INCREASE;
		}
		else if (affector->getAffectSpecialisation() == ParticleAffector::AFSP_TTL_DECREASE)
		{
			affectSpecialisation = KEYWORD_SPEC_TTL_DECREASE;
		}
		if (affector->getAffectSpecialisation() != ParticleAffector::DEFAULT_SPECIALISATION) serializer->writeLine(
			KEYWORD_SPECIALISATION, affectSpecialisation, 12);

		const std::list<Ogre::String> excludedEmitters = affector->getEmittersToExclude();
		Ogre::String emitterList = Ogre::StringUtil::BLANK;
		if (!excludedEmitters.empty())
		{
			std::list<Ogre::String>::const_iterator it;
			std::list<Ogre::String>::const_iterator itEnd = excludedEmitters.end();
			for (it = excludedEmitters.begin(); it != itEnd; ++it)
			{
				emitterList.append("\"" + (*it) + "\" ");
			}
			serializer->writeLine(KEYWORD_EXCLUDE_EMITTER, emitterList, 12);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleAffectorTokens::AffectorMassToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ParticleAffectorTokens::AffectorMassToken::parse");
		ParticleAffector* affector = static_cast<ParticleAffector*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real mass;
		deserializer->parseReal(mass);
		affector->mass = mass;
	}
	//-----------------------------------------------------------------------
	void ParticleAffectorTokens::ExcludeEmitterToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ParticleAffectorTokens::ExcludeEmitterToken::parse");
		ParticleAffector* affector = static_cast<ParticleAffector*>(deserializer->context.getCurrentSectionElement());
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		for (size_t i = 0; i < paramCount; ++i)
		{
			affector->addEmitterToExclude(deserializer->getNextTokenLabel());
		}
	}
	//-----------------------------------------------------------------------
	void ParticleAffectorTokens::AffectSpecialisationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ParticleAffectorTokens::AffectSpecialisationToken::parse");
		ParticleAffector* affector = static_cast<ParticleAffector*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_AFSP_DEFAULT))
		{
			affector->setAffectSpecialisation(ParticleAffector::AFSP_DEFAULT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_AFSP_TTL_INCREASE))
		{
			affector->setAffectSpecialisation(ParticleAffector::AFSP_TTL_INCREASE);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_AFSP_TTL_DECREASE))
		{
			affector->setAffectSpecialisation(ParticleAffector::AFSP_TTL_DECREASE);
		}
	}

}
