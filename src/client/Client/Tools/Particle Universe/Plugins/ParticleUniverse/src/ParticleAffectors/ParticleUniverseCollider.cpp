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

#include "ParticleAffectors/ParticleUniverseCollider.h"
#include "ParticleUniverseVisualParticle.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real Collider::DEFAULT_BOUNCYNESS = 1.0f;
	const Ogre::Real Collider::DEFAULT_FRICTION = 0.0f;
	const Collider::IntersectionType Collider::DEFAULT_INTERSECTION_TYPE = Collider::IT_POINT;
	const Collider::CollisionType Collider::DEFAULT_COLLISION_TYPE = Collider::CT_BOUNCE;

	//-----------------------------------------------------------------------
	Collider::Collider(void) :
		ParticleAffector(),
		mBouncyness(DEFAULT_BOUNCYNESS),
		mFriction(DEFAULT_FRICTION),
		mIntersectionType(DEFAULT_INTERSECTION_TYPE),
		mCollisionType(DEFAULT_COLLISION_TYPE)
	{
	}
	//-----------------------------------------------------------------------
	const Collider::IntersectionType Collider::getIntersectionType(void) const
	{
		return mIntersectionType;
	}
	//-----------------------------------------------------------------------
	void Collider::setIntersectionType(const Collider::IntersectionType& intersectionType)
	{
		mIntersectionType = intersectionType;
	}
	//-----------------------------------------------------------------------
	const Collider::CollisionType Collider::getCollisionType(void) const
	{
		return mCollisionType;
	}
	//-----------------------------------------------------------------------
	void Collider::setCollisionType(const Collider::CollisionType& collisionType)
	{
		mCollisionType = collisionType;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real Collider::getFriction(void) const
	{
		return mFriction;
	}
	//-----------------------------------------------------------------------
	void Collider::setFriction(const Ogre::Real friction)
	{
		mFriction = friction;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real Collider::getBouncyness(void) const
	{
		return mBouncyness;
	}
	//-----------------------------------------------------------------------
	void Collider::setBouncyness(const Ogre::Real bouncyness)
	{
		mBouncyness = bouncyness;
	}
	//-----------------------------------------------------------------------
	void Collider::populateAlignedBox(Ogre::AxisAlignedBox& box,
		const Ogre::Vector3& position, 
		const Ogre::Real width,
		const Ogre::Real height,
		const Ogre::Real depth)
	{
		Ogre::Real halfWidth = 0.5 * width;
		Ogre::Real halfHeight = 0.5 * height;
		Ogre::Real halfDepth = 0.5 * depth;
		box.setMinimum(position.x - halfWidth, 
			position.y - halfHeight, 
			position.z - halfDepth);
		box.setMaximum(position.x + halfWidth, 
			position.y + halfHeight, 
			position.z + halfDepth);
	}
	//-----------------------------------------------------------------------
	void Collider::calculateRotationSpeedAfterCollision(Particle* particle)
	{
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		Ogre::Real signedFriction = Ogre::Math::UnitRandom() > 0.5 ? -(mFriction - 1) : (mFriction - 1);
		visualParticle->rotationSpeed *= signedFriction;
		visualParticle->zRotationSpeed *= signedFriction;
	}
	//-----------------------------------------------------------------------
	void Collider::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		Collider* collider = static_cast<Collider*>(affector);
		collider->mBouncyness = mBouncyness;
		collider->mFriction = mFriction;
		collider->mIntersectionType = mIntersectionType;
		collider->mCollisionType = mCollisionType;
	}

}
