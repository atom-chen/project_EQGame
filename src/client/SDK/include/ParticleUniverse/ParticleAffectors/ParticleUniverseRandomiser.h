/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RANDOMISER_H__
#define __PU_RANDOMISER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	/** Randomises the position or the direction of a particle.
    */
	class _ParticleUniverseExport Randomiser : public ParticleAffector
	{
		protected:
			Ogre::Real mMaxDeviationX;
			Ogre::Real mMaxDeviationY;
			Ogre::Real mMaxDeviationZ;
			Ogre::Real mTimeSinceLastUpdate;
			Ogre::Real mTimeStep;
			bool mRandomDirection;
			bool mUpdate;

		public:
			// Constants
			static const Ogre::Vector3 DEFAULT_MAX_DEVIATION;
			static const Ogre::Real DEFAULT_TIME_STEP;
			static const bool DEFAULT_RANDOM_DIRECTION;

			Randomiser(void);
	        virtual ~Randomiser(void) {};

			/** 
	        */
			Ogre::Real getMaxDeviationX(void) const;
			void setMaxDeviationX(const Ogre::Real maxDeviationX);

			/** 
	        */
			Ogre::Real getMaxDeviationY(void) const;
			void setMaxDeviationY(const Ogre::Real maxDeviationZ);

			/** 
	        */
			Ogre::Real getMaxDeviationZ(void) const;
			void setMaxDeviationZ(const Ogre::Real maxDeviationZ);

			/** 
	        */
			Ogre::Real getTimeStep(void) const;
			void setTimeStep(const Ogre::Real timeStep);

			/** 
	        */
			bool isRandomDirection(void) const;
			void setRandomDirection(bool randomDirection);

			/** 
	        */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed);
			
			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** 
	        */
			virtual void _postProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);
	};

}
#endif
