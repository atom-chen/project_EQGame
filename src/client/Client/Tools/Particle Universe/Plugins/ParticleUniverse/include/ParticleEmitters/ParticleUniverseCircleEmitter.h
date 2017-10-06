/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_CIRCLE_EMITTER_H__
#define __PU_CIRCLE_EMITTER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseEmitter.h"

namespace ParticleUniverse
{
	/** The CircleEmitter is a ParticleEmitter that emits particles on a circle shape. Particle can be emitted
		random on the circle, but it can also follow the circles' contours.
    */
	class _ParticleUniverseExport CircleEmitter : public ParticleEmitter
	{
		protected:
			Ogre::Real mRadius;
			Ogre::Real mCircleAngle;
			Ogre::Real mOriginalCircleAngle;
			Ogre::Real mStep;
			Ogre::Real mX;
			Ogre::Real mZ;
			bool mRandom;
			Ogre::Quaternion mOrientation;
			Ogre::Vector3 mNormal;

		public:
			// Constants
			static const Ogre::Real DEFAULT_RADIUS;
			static const Ogre::Real DEFAULT_STEP;
			static const Ogre::Real DEFAULT_ANGLE;
			static const bool DEFAULT_RANDOM;
			static const Ogre::Vector3 DEFAULT_NORMAL;

			CircleEmitter(void);
	        virtual ~CircleEmitter(void) {};

			/** 
	        */
			const Ogre::Real getRadius(void) const;
			void setRadius(const Ogre::Real radius);

			/** 
	        */
			const Ogre::Real getCircleAngle(void) const;
			void setCircleAngle(const Ogre::Real circleAngle);

			/** 
	        */
			const Ogre::Real getStep(void) const;
			void setStep(const Ogre::Real step);

			/** 
	        */
			const bool isRandom(void) const;
			void setRandom(const bool random);

			/* 
			*/ 
			const Ogre::Quaternion& getOrientation(void) const;
			const Ogre::Vector3& getNormal(void) const;
			void setNormal(const Ogre::Vector3 &normal); 

			/** See ParticleEmiter
	        */
			void _notifyStart(void);

			/** Determine a particle position on the circle.
	        */
			virtual void _initParticlePosition(Particle* particle);

			/** Determine the particle direction.
	        */
			virtual void _initParticleDirection(Particle* particle);

			/** 
	        */
			virtual void copyAttributesTo (ParticleEmitter* emitter);
	};

}
#endif
