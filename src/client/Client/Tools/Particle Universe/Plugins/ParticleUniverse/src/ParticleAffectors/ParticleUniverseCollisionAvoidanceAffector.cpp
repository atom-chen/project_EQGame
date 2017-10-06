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

#include "ParticleAffectors/ParticleUniverseCollisionAvoidanceAffector.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real CollisionAvoidanceAffector::DEFAULT_RADIUS = 100.0f;

	//-----------------------------------------------------------------------
	CollisionAvoidanceAffector::CollisionAvoidanceAffector(void) : 
		ParticleAffector(),
		mRadius(DEFAULT_RADIUS)
	{
	}
	//-----------------------------------------------------------------------
	Ogre::Real CollisionAvoidanceAffector::getRadius(void) const
	{
		return mRadius;
	}
	//-----------------------------------------------------------------------
	void CollisionAvoidanceAffector::setRadius(Ogre::Real radius)
	{
		mRadius = radius;
	}
	//-----------------------------------------------------------------------
	void CollisionAvoidanceAffector::_prepare(ParticleTechnique* particleTechnique)
	{
		// Activate spatial hashing
		particleTechnique->setSpatialHashing(true);
	}
	//-----------------------------------------------------------------------
	void CollisionAvoidanceAffector::_unprepare(ParticleTechnique* particleTechnique)
	{
		// Disactivate spatial hashing
		particleTechnique->setSpatialHashing(true);
	}
	//-----------------------------------------------------------------------
	void CollisionAvoidanceAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		// Determine neighbouring particles.
		SpatialHashTable<Particle*>* hashtable = particleTechnique->getSpatialHashTable();
		if (hashtable)
		{
			SpatialHashTable<Particle*>::HashTableCell cell = hashtable->getCell(particle->position);
			if (cell.empty())
				return;

			unsigned int size = static_cast<unsigned int>(cell.size());
			Ogre::Vector3 displacement = Ogre::Vector3::ZERO;
			Ogre::Vector3 diff = Ogre::Vector3::ZERO;
			for (unsigned int i = 0; i < size; ++i)
			{
				Particle* p = cell[i];

				// Don't check if it is the same particle
				if (particle != p)
				{
					// Validate whether the neighbouring particle is within range
					diff = p->position - particle->position;
					if (diff.length() < mRadius)
					{
						displacement -= diff;
					}
				}
			}
			particle->direction += displacement * timeElapsed;
		}
	}
	//-----------------------------------------------------------------------
	void CollisionAvoidanceAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);
		CollisionAvoidanceAffector* collisionAvoidanceAffector = static_cast<CollisionAvoidanceAffector*>(affector);
		collisionAvoidanceAffector->mRadius = mRadius;
	}
}
