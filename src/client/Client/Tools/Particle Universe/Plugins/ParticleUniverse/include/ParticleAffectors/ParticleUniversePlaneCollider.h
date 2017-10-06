/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PLANE_COLLIDER_H__
#define __PU_PLANE_COLLIDER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseCollider.h"

namespace ParticleUniverse
{
	/** The PlaneCollider is a plane shape that collides with the particles. The PlaneCollider can only collide 
		with particles that are created within the same ParticleTechnique as where the PlaneCollider is registered.
    */
	class _ParticleUniverseExport PlaneCollider : public Collider
	{
		public:
			// Constants
			static const Ogre::Vector3 DEFAULT_NORMAL;

			PlaneCollider(void);
	        virtual ~PlaneCollider(void){};

			/** Returns the normal of the plane
			*/
			const Ogre::Vector3 getNormal(void) const;

			/** Sets the normal of the plane
			*/
			void setNormal(const Ogre::Vector3& normal);

			/** 
			*/
			void calculateDirectionAfterCollision(Particle* particle, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::_affect */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

		protected:
			Ogre::Vector3 mNormal;
			Ogre::Vector3 mPredictedPosition;
			Ogre::Plane mPlane;
	};

}
#endif
