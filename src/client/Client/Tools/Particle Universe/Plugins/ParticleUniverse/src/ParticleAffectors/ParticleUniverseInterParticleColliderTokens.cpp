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
#include "ParticleAffectors/ParticleUniverseInterParticleCollider.h"
#include "ParticleAffectors/ParticleUniverseInterParticleColliderTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String InterParticleColliderTokens::KEYWORD_ADJUSTMENT = "ip_adjustment";
	const Ogre::String InterParticleColliderTokens::KEYWORD_COLLISION_RESPONSE = "ip_collision_response";
	const Ogre::String InterParticleColliderTokens::KEYWORD_AVERAGE_VELOCITY = "average_velocity";
	const Ogre::String InterParticleColliderTokens::KEYWORD_ANGLE_BASED_VELOCITY = "angle_based_velocity";

	//-----------------------------------------------------------------------
	void InterParticleColliderTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_ADJUSTMENT, &mAdjustmentToken);
		tokenRegister->addLexemeAction(KEYWORD_COLLISION_RESPONSE, &mCollisionResponseToken);
	}
	//-----------------------------------------------------------------------
	void InterParticleColliderTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a InterParticleCollider
		const InterParticleCollider* affector = static_cast<const InterParticleCollider*>(element);

		// Write the header of the InterParticleCollider
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getAdjustment() != InterParticleCollider::DEFAULT_ADJUSTMENT) serializer->writeLine(
			KEYWORD_ADJUSTMENT, Ogre::StringConverter::toString(affector->getAdjustment()), 12);

		if (affector->getInterParticleCollisionResponse() != InterParticleCollider::DEFAULT_COLLISION_RESPONSE) 
		{
			Ogre::String collisionResponse = KEYWORD_ANGLE_BASED_VELOCITY;
			if (affector->getInterParticleCollisionResponse() == InterParticleCollider::IPCR_AVERAGE_VELOCITY) collisionResponse = KEYWORD_AVERAGE_VELOCITY;
			serializer->writeLine(KEYWORD_COLLISION_RESPONSE, collisionResponse, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void InterParticleColliderTokens::AdjustmentToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "InterParticleColliderTokens::AdjustmentToken::parse");
		InterParticleCollider* affector = static_cast<InterParticleCollider*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real adjustment;
		deserializer->parseReal(adjustment);
		affector->setAdjustment(adjustment);
	}
	//-----------------------------------------------------------------------
	void InterParticleColliderTokens::CollisionResponseToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "InterParticleColliderTokens::CollisionResponseToken::parse");
		InterParticleCollider* affector = static_cast<InterParticleCollider*>(deserializer->context.getCurrentSectionElement());
		Ogre::String collisionResponse = deserializer->getNextTokenLabel();
		if (collisionResponse == KEYWORD_AVERAGE_VELOCITY)
		{
			affector->setInterParticleCollisionResponse(InterParticleCollider::IPCR_AVERAGE_VELOCITY);
		}
		else if (collisionResponse == KEYWORD_ANGLE_BASED_VELOCITY)
		{
			affector->setInterParticleCollisionResponse(InterParticleCollider::IPCR_ANGLE_BASED_VELOCITY);
		}
	}

}
