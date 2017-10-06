/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_INTERPARTICLE_COLLIDER_H__
#define __PU_INTERPARTICLE_COLLIDER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseCollider.h"

namespace ParticleUniverse
{
	/** The InterParticleCollider ....
    */
	class _ParticleUniverseExport InterParticleCollider : public Collider
	{
		public:
			enum InterParticleCollisionResponse
			{
				IPCR_AVERAGE_VELOCITY,
				IPCR_ANGLE_BASED_VELOCITY
			};

			// Constants
			static const Ogre::Real DEFAULT_ADJUSTMENT;
			static const InterParticleCollisionResponse DEFAULT_COLLISION_RESPONSE;

			InterParticleCollider(void);
	        virtual ~InterParticleCollider(void){};

			/** Todo
			*/
			Ogre::Real getAdjustment(void) const;

			/** Todo
			*/
			void setAdjustment(Ogre::Real adjustment);

			/** Todo
			*/
			InterParticleCollisionResponse getInterParticleCollisionResponse(void) const;

			/** Todo
			*/
			void setInterParticleCollisionResponse(InterParticleCollisionResponse interParticleCollisionResponse);

			/** @copydoc ParticleAffector::_prepare */
			virtual void _prepare(ParticleTechnique* particleTechnique);

			/** @copydoc ParticleAffector::_unprepare */
			virtual void _unprepare(ParticleTechnique* particleTechnique);

			/** @copydoc ParticleAffector::_affect */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);
		
		protected:
			Ogre::Real mAdjustment;
			InterParticleCollisionResponse mInterParticleCollisionResponse;

			/** Todo
			*/
			bool _validateAndExecuteSphereCollision (Particle* particle1, Particle* particle2, Ogre::Real timeElapsed);
	};

}
#endif
