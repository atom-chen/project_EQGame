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

#include "ParticleAffectors/ParticleUniverseBoxCollider.h"
#include "ParticleUniverseVisualParticle.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real BoxCollider::DEFAULT_WIDTH = 0.0f;
	const Ogre::Real BoxCollider::DEFAULT_HEIGHT = 0.0f;
	const Ogre::Real BoxCollider::DEFAULT_DEPTH = 0.0f;

	//-----------------------------------------------------------------------
	BoxCollider::BoxCollider(void) : 
		Collider(),
		mWidth(DEFAULT_WIDTH),
		mHeight(DEFAULT_HEIGHT),
		mDepth(DEFAULT_DEPTH),
		mXmin(0.0f),
		mXmax(0.0f),
		mYmin(0.0f),
		mYmax(0.0f),
		mZmin(0.0f),
		mZmax(0.0f),
		mPredictedPosition(Ogre::Vector3::ZERO)
	{
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxCollider::getWidth(void) const
	{
		return mWidth;
	}
	//-----------------------------------------------------------------------
	void BoxCollider::setWidth(const Ogre::Real width)
	{
		mWidth = width;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxCollider::getHeight(void) const
	{
		return mHeight;
	}
	//-----------------------------------------------------------------------
	void BoxCollider::setHeight(const Ogre::Real height)
	{
		mHeight = height;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real BoxCollider::getDepth(void) const
	{
		return mDepth;
	}
	//-----------------------------------------------------------------------
	void BoxCollider::setDepth(const Ogre::Real depth)
	{
		mDepth = depth;
	}
	//-----------------------------------------------------------------------
	void BoxCollider::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		// Calculate the affectors' center position in worldspace, set the box and calculate the bounds
		populateAlignedBox(mBox, getDerivedPosition(), mWidth, mHeight, mDepth);
		_calculateBounds();
	}
	//-----------------------------------------------------------------------
	void BoxCollider::_affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		mPredictedPosition = particle->position + timeElapsed * particle->direction;
		bool collision = false;

		/** Collision detection is a two-step. First, we determine whether the particle is now colliding.
			If it is, the particle is re-positioned. However, a timeElapsed value is used, which is not the same
			as the one that was used at the moment before the particle was colliding. Therefore, we rather 
			want to predict particle collision in front. This probably isn't the fastest solution.
			The same approach was used for the other colliders.
		*/
		switch(mIntersectionType)
		{
			case Collider::IT_POINT:
			{
				// Validate for a point-box intersection
				if (mBox.intersects(particle->position))
				{
					// Collision detected (re-position the particle)
					particle->position -= timeElapsed * particle->direction;
					collision = true;
				}
				else if (mBox.intersects(mPredictedPosition))
				{
					// Collision detected
					collision = true;
				}
			}
			break;

			case Collider::IT_BOX:
			{
				// Validate for a box-box intersection
				if (!particle->particleType == Particle::PT_VISUAL)
					break;

				VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
				Ogre::AxisAlignedBox box;
				populateAlignedBox(box,
					visualParticle->position, 
					visualParticle->width, 
					visualParticle->height,
					visualParticle->depth);
				if (box.intersects(mBox))
				{
					// Collision detected (re-position the particle)
					particle->position -= timeElapsed * particle->direction;
					collision = true;
				}
				else
				{
					Ogre::AxisAlignedBox box;
					populateAlignedBox(box,
						mPredictedPosition, 
						visualParticle->width, 
						visualParticle->height,
						visualParticle->depth);
					if (box.intersects(mBox))
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
			calculateDirectionAfterCollision(particle);
			calculateRotationSpeedAfterCollision(particle);
			particle->addEventFlags(Particle::PEF_COLLIDED);
		}
	}
	//-----------------------------------------------------------------------
	void BoxCollider::calculateDirectionAfterCollision(Particle* particle)
	{
		switch (mCollisionType)
		{
			case Collider::CT_BOUNCE:
			{
				// Determine the nearest side and reverse the direction
				if (_isSmallestValue (particle->position.x - mXmin, particle->position))
				{		
					particle->direction.x *= -1;
				}
				else if (_isSmallestValue (mXmax - particle->position.x, particle->position))
				{
					particle->direction.x *= -1;
				}
				else if (_isSmallestValue (particle->position.y - mYmin, particle->position))
				{
					particle->direction.y *= -1;
				}
				else if (_isSmallestValue (mYmax - particle->position.y, particle->position))
				{
					particle->direction.y *= -1;
				}
				else if (_isSmallestValue (particle->position.z - mZmin, particle->position))
				{
					particle->direction.z *= -1;
				}
				else if (_isSmallestValue (mZmax - particle->position.z, particle->position))
				{
					particle->direction.z *= -1;
				}
				particle->direction *= mBouncyness;
			}
			break;
			case Collider::CT_FLOW:
			{
				if (_isSmallestValue (particle->position.x - mXmin, particle->position))
				{		
					particle->direction.x = 0;
				}
				else if (_isSmallestValue (mXmax - particle->position.x, particle->position))
				{
					particle->direction.x = 0;
				}
				else if (_isSmallestValue (particle->position.y - mYmin, particle->position))
				{
					particle->direction.y = 0;
				}
				else if (_isSmallestValue (mYmax - particle->position.y, particle->position))
				{
					particle->direction.y = 0;
				}
				else if (_isSmallestValue (particle->position.z - mZmin, particle->position))
				{
					particle->direction.z = 0;
				}
				else if (_isSmallestValue (mZmax - particle->position.z, particle->position))
				{
					particle->direction.z = 0;
				}
				particle->direction *= -mFriction;
			}
			break;
		}
	}
	//-----------------------------------------------------------------------
	void BoxCollider::_calculateBounds (void)
	{
		mXmin = mDerivedPosition.x - 0.5 * mWidth;
		mXmax = mDerivedPosition.x + 0.5 * mWidth;
		mYmin = mDerivedPosition.y - 0.5 * mHeight;
		mYmax = mDerivedPosition.y + 0.5 * mHeight;
		mZmin = mDerivedPosition.z - 0.5 * mDepth;
		mZmax = mDerivedPosition.z + 0.5 * mDepth;
	}
    //-----------------------------------------------------------------------
	bool BoxCollider::_isSmallestValue(Ogre::Real value, const Ogre::Vector3& particlePosition)
    {
		Ogre::Real value1 = particlePosition.x - mXmin;
		Ogre::Real value2 = mXmax - particlePosition.x;
		Ogre::Real value3 = particlePosition.y - mYmin;
		Ogre::Real value4 = mYmax - particlePosition.y;
		Ogre::Real value5 = particlePosition.z - mZmin;
		Ogre::Real value6 = mZmax - particlePosition.z;

		return (
			value <= value1 && 
			value <= value2 &&
			value <= value3 && 
			value <= value4 &&
			value <= value5 &&
			value <= value6);
    }
	//-----------------------------------------------------------------------
	void BoxCollider::copyAttributesTo (ParticleAffector* affector)
	{
		Collider::copyAttributesTo(affector);

		BoxCollider* boxCollider = static_cast<BoxCollider*>(affector);
		boxCollider->mWidth = mWidth;
		boxCollider->mHeight = mHeight;
		boxCollider->mDepth = mDepth;
	}
}
