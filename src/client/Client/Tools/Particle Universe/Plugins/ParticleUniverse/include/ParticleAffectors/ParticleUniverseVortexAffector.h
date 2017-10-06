/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VORTEX_AFFECTOR_H__
#define __PU_VORTEX_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"
#include "ParticleUniverseDynamicAttribute.h"
#include "ParticleUniverseDynamicAttributeFactory.h"

namespace ParticleUniverse
{
	/** The VortexAffector rotates particles around a given rotation axis.
    */
	class _ParticleUniverseExport VortexAffector : public ParticleAffector
	{
		public:
			// Constants
			static const Ogre::Vector3 DEFAULT_ROTATION_VECTOR;
			static const Ogre::Real DEFAULT_ROTATION_SPEED;
		
			VortexAffector(void);
	        virtual ~VortexAffector(void);

			/** 
	        */
			const Ogre::Vector3& getRotationVector(void) const;

			/** 
	        */
			void setRotationVector(const Ogre::Vector3& rotationVector);

			/** 
	        */
			DynamicAttribute* getRotationSpeed(void) const;

			/** 
	        */
			void setRotationSpeed(DynamicAttribute* dynRotationSpeed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** 
	        */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed);

			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

		protected:
			Ogre::Vector3 mRotationVector;
			Ogre::Quaternion mRotation;
			DynamicAttribute* mDynRotationSpeed;
			DynamicAttributeHelper mDynamicAttributeHelper;
			DynamicAttributeFactory mDynamicAttributeFactory;

			/** 
	        */
			Ogre::Radian VortexAffector::_calculateRotationSpeed(void);
	};

}
#endif
