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
#include "ParticleAffectors/ParticleUniverseParticleFollower.h"
#include "ParticleAffectors/ParticleUniverseParticleFollowerTokens.h"
#include "Externs/ParticleUniverseGravityExtern.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleFollowerTokens::KEYWORD_MAX_DISTANCE = "follower_max_distance";
	const Ogre::String ParticleFollowerTokens::KEYWORD_MIN_DISTANCE = "follower_min_distance";

	//-----------------------------------------------------------------------
	void ParticleFollowerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MAX_DISTANCE, &mMaxDistanceToken);
		tokenRegister->addLexemeAction(KEYWORD_MIN_DISTANCE, &mMinDistanceToken);
	}
	//-----------------------------------------------------------------------
	void ParticleFollowerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleFollower
		const ParticleFollower* affector = static_cast<const ParticleFollower*>(element);

		// Write the header of the ParticleFollower
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getMinDistance() != ParticleFollower::DEFAULT_MIN_DISTANCE) serializer->writeLine(
			KEYWORD_MIN_DISTANCE, Ogre::StringConverter::toString(affector->getMinDistance()), 12);
		if (affector->getMaxDistance() != ParticleFollower::DEFAULT_MAX_DISTANCE) serializer->writeLine(
			KEYWORD_MAX_DISTANCE, Ogre::StringConverter::toString(affector->getMaxDistance()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void ParticleFollowerTokens::MaxDistanceToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ParticleFollowerTokens::MinDistanceToken::parse");
		ParticleFollower* affector = static_cast<ParticleFollower*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real maxDistance;
		deserializer->parseReal(maxDistance);
		affector->setMaxDistance(maxDistance);
	}
	//-----------------------------------------------------------------------
	void ParticleFollowerTokens::MinDistanceToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ParticleFollowerTokens::MinDistanceToken::parse");
		ParticleFollower* affector = static_cast<ParticleFollower*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real minDistance;
		deserializer->parseReal(minDistance);
		affector->setMinDistance(minDistance);
	}

}
