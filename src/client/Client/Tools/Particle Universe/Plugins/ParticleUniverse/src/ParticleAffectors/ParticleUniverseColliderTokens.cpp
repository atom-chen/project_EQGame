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
#include "ParticleAffectors/ParticleUniverseCollider.h"
#include "ParticleAffectors/ParticleUniverseColliderTokens.h"
#include "Externs/ParticleUniverseSphereColliderExtern.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ColliderTokens::KEYWORD_FRICTION = "collision_friction";
	const Ogre::String ColliderTokens::KEYWORD_BOUNCYNESS = "collision_bouncyness";
	const Ogre::String ColliderTokens::KEYWORD_INTERSECTION = "collision_intersection";
	const Ogre::String ColliderTokens::KEYWORD_COLLISION_TYPE = "collision_type";
	const Ogre::String ColliderTokens::KEYWORD_BOUNCE = "bounce";
	const Ogre::String ColliderTokens::KEYWORD_FLOW = "flow";
	const Ogre::String ColliderTokens::KEYWORD_NONE = "none";
	const Ogre::String ColliderTokens::KEYWORD_INTERSECTION_POINT = "point";
	const Ogre::String ColliderTokens::KEYWORD_INTERSECTION_BOX = "box";

	//-----------------------------------------------------------------------
	void ColliderTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_FRICTION, &mFrictionToken);
		tokenRegister->addLexemeAction(KEYWORD_BOUNCYNESS, &mBouncynessToken);
		tokenRegister->addLexemeAction(KEYWORD_INTERSECTION, &mIntersectionTypeToken);
		tokenRegister->addLexemeAction(KEYWORD_COLLISION_TYPE, &mCollisionTypeToken);
		
		// Register id´s
		tokenRegister->addTokenId(KEYWORD_BOUNCE, ITokenRegister::ID_COT_BOUNCE);
		tokenRegister->addTokenId(KEYWORD_FLOW, ITokenRegister::ID_COT_FLOW);
		tokenRegister->addTokenId(KEYWORD_NONE, ITokenRegister::ID_COT_NONE);
	}
	//-----------------------------------------------------------------------
	void ColliderTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a Collider
		const Collider* affector = static_cast<const Collider*>(element);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getFriction() != Collider::DEFAULT_FRICTION) serializer->writeLine(
			KEYWORD_FRICTION, Ogre::StringConverter::toString(affector->getFriction()), 12);
		if (affector->getBouncyness() != Collider::DEFAULT_BOUNCYNESS) serializer->writeLine(
			KEYWORD_BOUNCYNESS, Ogre::StringConverter::toString(affector->getBouncyness()), 12);
		if (affector->getIntersectionType() != Collider::DEFAULT_INTERSECTION_TYPE) 
		{
			Ogre::String intersectionType = KEYWORD_INTERSECTION_POINT;
			if (affector->getIntersectionType() == Collider::IT_BOX) intersectionType = KEYWORD_INTERSECTION_BOX;
			serializer->writeLine(KEYWORD_INTERSECTION, intersectionType, 12);
		}
		if (affector->getCollisionType() != Collider::DEFAULT_COLLISION_TYPE) 
		{
			Ogre::String collisionType = KEYWORD_BOUNCE;
			if (affector->getCollisionType() == Collider::CT_FLOW) collisionType = KEYWORD_FLOW;
			else if (affector->getCollisionType() == Collider::CT_NONE) collisionType = KEYWORD_NONE;
			serializer->writeLine(KEYWORD_COLLISION_TYPE, collisionType, 12);
		}
	}
	//-----------------------------------------------------------------------
	void ColliderTokens::FrictionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// For all ColliderTokens applies that they is are used both for the SphereCollider and for the SphereColliderExtern.
		Ogre::Real friction;
		deserializer->parseReal(friction);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			SphereColliderExtern* sphereColliderExtern = static_cast<SphereColliderExtern*>(externObject);
			sphereColliderExtern->setFriction(friction);
		}
		else
		{
			Collider* affector = static_cast<Collider*>(deserializer->context.getCurrentSectionElement());
			affector->setFriction(friction);
		}
	}
	//-----------------------------------------------------------------------
	void ColliderTokens::BouncynessToken::parse(ParticleScriptDeserializer* deserializer)
	{
		Ogre::Real bouncyness;
		deserializer->parseReal(bouncyness);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			SphereColliderExtern* sphereColliderExtern = static_cast<SphereColliderExtern*>(externObject);
			sphereColliderExtern->setBouncyness(bouncyness);
		}
		else
		{
			Collider* affector = static_cast<Collider*>(deserializer->context.getCurrentSectionElement());
			affector->setBouncyness(bouncyness);
		}
	}
	//-----------------------------------------------------------------------
	void ColliderTokens::IntersectionTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			SphereColliderExtern* sphereColliderExtern = static_cast<SphereColliderExtern*>(externObject);
			if (deserializer->testNextTokenID(ITokenRegister::ID_SHAPE_POINT))
			{
				sphereColliderExtern->setIntersectionType(Collider::IT_POINT);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_SHAPE_BOX))
			{
				sphereColliderExtern->setIntersectionType(Collider::IT_BOX);
			}
		}
		else
		{
			Collider* affector = static_cast<Collider*>(deserializer->context.getCurrentSectionElement());
			if (deserializer->testNextTokenID(ITokenRegister::ID_SHAPE_POINT))
			{
				affector->setIntersectionType(Collider::IT_POINT);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_SHAPE_BOX))
			{
				affector->setIntersectionType(Collider::IT_BOX);
			}
		}
	}
	//-----------------------------------------------------------------------
	void ColliderTokens::CollisionTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			SphereColliderExtern* sphereColliderExtern = static_cast<SphereColliderExtern*>(externObject);
			if (deserializer->testNextTokenID(ITokenRegister::ID_COT_BOUNCE))
			{
				sphereColliderExtern->setCollisionType(Collider::CT_BOUNCE);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_COT_FLOW))
			{
				sphereColliderExtern->setCollisionType(Collider::CT_FLOW);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_COT_NONE))
			{
				sphereColliderExtern->setCollisionType(Collider::CT_NONE);
			}
		}
		else
		{
			Collider* affector = static_cast<Collider*>(deserializer->context.getCurrentSectionElement());
			if (deserializer->testNextTokenID(ITokenRegister::ID_COT_BOUNCE))
			{
				affector->setCollisionType(Collider::CT_BOUNCE);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_COT_FLOW))
			{
				affector->setCollisionType(Collider::CT_FLOW);
			}
			else if (deserializer->testNextTokenID(ITokenRegister::ID_COT_NONE))
			{
				affector->setCollisionType(Collider::CT_NONE);
			}
		}
	}

}
