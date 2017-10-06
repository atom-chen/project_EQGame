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
#include "Externs/ParticleUniverseSphereColliderExtern.h"
#include "Externs/ParticleUniverseSphereColliderExternTokens.h"
#include "ParticleAffectors/ParticleUniverseColliderTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SphereColliderExternTokens::KEYWORD_RADIUS = "sphere_collider_radius";

	//-----------------------------------------------------------------------
	void SphereColliderExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SphereColliderExtern
		const Extern* externObject = static_cast<const Extern*>(element);
		const SphereColliderExtern* sphereColliderExtern = static_cast<const SphereColliderExtern*>(externObject);
		serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ExternTokens::write(serializer, element);
		AttachableTokens::write(serializer, element);

		// Write own attributes
		if (sphereColliderExtern->getRadius() != SphereCollider::DEFAULT_RADIUS) serializer->writeLine(
			KEYWORD_RADIUS, Ogre::StringConverter::toString(sphereColliderExtern->getRadius()), 12);
		if (sphereColliderExtern->getFriction() != Collider::DEFAULT_FRICTION) serializer->writeLine(
			ColliderTokens::KEYWORD_FRICTION, Ogre::StringConverter::toString(sphereColliderExtern->getFriction()), 12);
		if (sphereColliderExtern->getBouncyness() != Collider::DEFAULT_BOUNCYNESS) serializer->writeLine(
			ColliderTokens::KEYWORD_BOUNCYNESS, Ogre::StringConverter::toString(sphereColliderExtern->getBouncyness()), 12);
		if (sphereColliderExtern->getIntersectionType() != Collider::DEFAULT_INTERSECTION_TYPE) 
		{
			Ogre::String intersectionType = ColliderTokens::KEYWORD_INTERSECTION_POINT;
			if (sphereColliderExtern->getIntersectionType() == Collider::IT_BOX) intersectionType = ColliderTokens::KEYWORD_INTERSECTION_BOX;
			serializer->writeLine(ColliderTokens::KEYWORD_INTERSECTION, intersectionType, 12);
		}
		if (sphereColliderExtern->getCollisionType() != Collider::DEFAULT_COLLISION_TYPE) 
		{
			Ogre::String collisionType = ColliderTokens::KEYWORD_BOUNCE;
			if (sphereColliderExtern->getCollisionType() == Collider::CT_FLOW) collisionType = ColliderTokens::KEYWORD_FLOW;
			else if (sphereColliderExtern->getCollisionType() == Collider::CT_NONE) collisionType = ColliderTokens::KEYWORD_NONE;
			serializer->writeLine(ColliderTokens::KEYWORD_COLLISION_TYPE, collisionType, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
