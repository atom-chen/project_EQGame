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

#include "ParticleAffectors/ParticleUniversePlaneCollider.h"
#include "ParticleUniverseVisualParticle.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 PlaneCollider::DEFAULT_NORMAL = Ogre::Vector3::ZERO;

	//-----------------------------------------------------------------------
	PlaneCollider::PlaneCollider(void) : 
		Collider(),
		mPredictedPosition(Ogre::Vector3::ZERO),
		mNormal(DEFAULT_NORMAL)
	{
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3 PlaneCollider::getNormal(void) const
	{
		return mNormal;
	}
	//-----------------------------------------------------------------------
	void PlaneCollider::setNormal(const Ogre::Vector3& normal)
	{
		mNormal = normal;
		mPlane.redefine(mNormal, position);
	}
	//-----------------------------------------------------------------------
	void PlaneCollider::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		mPredictedPosition = particle->position + timeElapsed * particle->direction;
		bool collision = false;

		switch(mIntersectionType)
		{
			case Collider::IT_POINT:
			{
				// Validate for a point-plane intersection (on the plane or the back side)
				// First determine whether it is now colliding (some affector made the particle move), else
				// determine whether it WILL be colliding
				if (mPlane.getDistance(particle->position) <= 0.0f)
				{
					// Collision detected (re-position the particle)
					particle->position -= timeElapsed * particle->direction;
					collision = true;
				}
				else if (mPlane.getDistance(mPredictedPosition) <= 0.0f)
				{
					// Collision detected
					collision = true;
				}
			}
			break;

			case Collider::IT_BOX:
			{
				// Validate for a box-plane intersection
				if (!particle->particleType == Particle::PT_VISUAL)
					break;

				VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
				Ogre::AxisAlignedBox box;
				populateAlignedBox(box,
					visualParticle->position, 
					visualParticle->width, 
					visualParticle->height,
					visualParticle->depth);
				if (box.intersects(mPlane))
				{
					// Collision detected (re-position the particle)
					particle->position -= timeElapsed * particle->direction;
					collision = true;
				}
				else 
				{
					populateAlignedBox(box,
						mPredictedPosition, 
						visualParticle->width, 
						visualParticle->height,
						visualParticle->depth);
					if (box.intersects(mPlane))
					{
						// Collision detected
						collision = true;
					}
				}
			}
			break;
		}

		if (collision)
		{
			calculateDirectionAfterCollision(particle, timeElapsed);
			calculateRotationSpeedAfterCollision(particle);
			particle->addEventFlags(Particle::PEF_COLLIDED);
		}
	}
	//-----------------------------------------------------------------------
	void PlaneCollider::calculateDirectionAfterCollision(Particle* particle, Ogre::Real timeElapsed)
	{
		Ogre::Real directionLength = particle->direction.length();
		switch (mCollisionType)
		{
			case Collider::CT_BOUNCE:
			{
				/** If the particle is on the plane or at the back of the plane, bounce it.
					Make use of the same formula as the sphere collider.
				*/
				particle->direction.normalise();
				particle->direction = 2 * (-particle->direction.dotProduct(-mNormal)) * -mNormal + particle->direction;

				// Adjust to original speed
				particle->direction *= directionLength;

				// Accelerate/slow down, using the bounce value
				particle->direction *= mBouncyness;
			}
			break;
			case Collider::CT_FLOW:
			{
				/** Reset the position (just in front of the plane), but keep the direction.
				@remarks
					This is not really the correct way, because the particle 'jumps'. Maybe it is better to change 
					the direction parallel to the plane.
				*/
				particle->position += timeElapsed * directionLength * mNormal;
			}
			break;
		}
	}
	//-----------------------------------------------------------------------
	void PlaneCollider::copyAttributesTo (ParticleAffector* affector)
	{
		Collider::copyAttributesTo(affector);

		PlaneCollider* planeCollider = static_cast<PlaneCollider*>(affector);
		planeCollider->setNormal(mNormal);
	}
}
