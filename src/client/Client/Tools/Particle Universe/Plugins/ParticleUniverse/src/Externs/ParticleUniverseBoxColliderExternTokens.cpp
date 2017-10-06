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

#include "ParticleUniverseScriptSerializer.h"
#include "Externs/ParticleUniverseBoxColliderExtern.h"
#include "Externs/ParticleUniverseBoxColliderExternTokens.h"
#include "ParticleAffectors/ParticleUniverseColliderTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String BoxColliderExternTokens::KEYWORD_RADIUS = "Box_collider_radius";
	const Ogre::String BoxColliderExternTokens::KEYWORD_WIDTH = "box_collider_width";
	const Ogre::String BoxColliderExternTokens::KEYWORD_HEIGHT = "box_collider_height";
	const Ogre::String BoxColliderExternTokens::KEYWORD_DEPTH = "box_collider_depth";

	//-----------------------------------------------------------------------
	void BoxColliderExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a BoxColliderExtern
		const Extern* externObject = static_cast<const Extern*>(element);
		const BoxColliderExtern* boxColliderExtern = static_cast<const BoxColliderExtern*>(externObject);
		serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ExternTokens::write(serializer, element);
		AttachableTokens::write(serializer, element);

		// Write own attributes
		if (boxColliderExtern->getWidth() != BoxCollider::DEFAULT_WIDTH) serializer->writeLine(
			KEYWORD_WIDTH, Ogre::StringConverter::toString(boxColliderExtern->getWidth()), 12);
		if (boxColliderExtern->getHeight() != BoxCollider::DEFAULT_HEIGHT) serializer->writeLine(
			KEYWORD_HEIGHT, Ogre::StringConverter::toString(boxColliderExtern->getHeight()), 12);
		if (boxColliderExtern->getDepth() != BoxCollider::DEFAULT_DEPTH) serializer->writeLine(
			KEYWORD_DEPTH, Ogre::StringConverter::toString(boxColliderExtern->getDepth()), 12);
		if (boxColliderExtern->getFriction() != Collider::DEFAULT_FRICTION) serializer->writeLine(
			ColliderTokens::KEYWORD_FRICTION, Ogre::StringConverter::toString(boxColliderExtern->getFriction()), 12);
		if (boxColliderExtern->getBouncyness() != Collider::DEFAULT_BOUNCYNESS) serializer->writeLine(
			ColliderTokens::KEYWORD_BOUNCYNESS, Ogre::StringConverter::toString(boxColliderExtern->getBouncyness()), 12);
		if (boxColliderExtern->getIntersectionType() != Collider::DEFAULT_INTERSECTION_TYPE) 
		{
			Ogre::String intersectionType = ColliderTokens::KEYWORD_INTERSECTION_POINT;
			if (boxColliderExtern->getIntersectionType() == Collider::IT_BOX) intersectionType = ColliderTokens::KEYWORD_INTERSECTION_BOX;
			serializer->writeLine(ColliderTokens::KEYWORD_INTERSECTION, intersectionType, 12);
		}
		if (boxColliderExtern->getCollisionType() != Collider::DEFAULT_COLLISION_TYPE) 
		{
			Ogre::String collisionType = ColliderTokens::KEYWORD_BOUNCE;
			if (boxColliderExtern->getCollisionType() == Collider::CT_FLOW) collisionType = ColliderTokens::KEYWORD_FLOW;
			else if (boxColliderExtern->getCollisionType() == Collider::CT_NONE) collisionType = ColliderTokens::KEYWORD_NONE;
			serializer->writeLine(ColliderTokens::KEYWORD_COLLISION_TYPE, collisionType, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
