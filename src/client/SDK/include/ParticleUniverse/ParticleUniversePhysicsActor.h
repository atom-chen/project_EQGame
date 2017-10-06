/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PHYSICS_ACTOR_H__
#define __PU_PHYSICS_ACTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniversePhysicsShape.h"

namespace ParticleUniverse
{
	/** PhysicsActorDesc contains the properties of the actor and is used to create a actor.
	*/
	class _ParticleUniverseExport PhysicsActorDesc
	{
		public:
			// Constructor
			PhysicsActorDesc() : 
				position(Ogre::Vector3::ZERO),
				direction(Ogre::Vector3::ZERO),
				orientation(Ogre::Quaternion::IDENTITY),
				mass(1.0f),
				collisionGroup(0),
				contactPoint(Ogre::Vector3::ZERO){};

			// Destructor
			virtual ~PhysicsActorDesc() {}

			// Public attributes
			Ogre::Vector3 position;
			Ogre::Vector3 direction;
			Ogre::Quaternion orientation;
			Ogre::Real mass;
			Ogre::uint16 collisionGroup;
			Ogre::Vector3 contactPoint;
	};

	/** A PhysicsActor is an element of the physical world and has no visual representation by itself. It only has physical properties. 
		The PhysicsActor is an abstraction class of the real underlying physics engine, such as PhysX, Havok or Bullet.
	*/
	class _ParticleUniverseExport PhysicsActor
	{
		public:
			// Constructor
			PhysicsActor(void) : 
				position(Ogre::Vector3::ZERO),
				direction(Ogre::Vector3::ZERO),
				orientation(Ogre::Quaternion::IDENTITY),
				mass(1.0f),
				collisionGroup(0),
				contactPoint(Ogre::Vector3::ZERO){};

			// Destructor
			virtual ~PhysicsActor(void) {};

			// Public attributes
			Ogre::Vector3 position;
			Ogre::Vector3 direction;
			Ogre::Quaternion orientation;
			Ogre::Real mass;
			Ogre::uint16 collisionGroup;
			Ogre::Vector3 contactPoint;
	};

}
#endif
