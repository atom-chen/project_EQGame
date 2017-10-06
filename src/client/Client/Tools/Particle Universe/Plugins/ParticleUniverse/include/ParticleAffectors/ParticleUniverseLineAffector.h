/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LINE_AFFECTOR_H__
#define __PU_LINE_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	/** Affects a particle depending on a line shape. Particles are getting a new position along the line.
    */
	class _ParticleUniverseExport LineAffector : public ParticleAffector
	{
		protected:
			Ogre::Real mMaxDeviation;
			Ogre::Real _mScaledMaxDeviation;
			Ogre::Vector3 mEnd;
			Ogre::Real mTimeSinceLastUpdate;
			Ogre::Real mTimeStep;
			Ogre::Real mDrift;
			Ogre::Real mOneMinusDrift;
			bool mUpdate;
			bool mFirst;

		public:
			// Constants
			static const Ogre::Real DEFAULT_MAX_DEVIATION;
			static const Ogre::Real DEFAULT_TIME_STEP;
			static const Ogre::Vector3 DEFAULT_END;
			static const Ogre::Real DEFAULT_DRIFT;
			
			LineAffector(void);
	        virtual ~LineAffector(void) {};

			/** 
	        */
			Ogre::Real getMaxDeviation(void) const;
			void setMaxDeviation(Ogre::Real maxDeviation);

			/** 
	        */
			const Ogre::Vector3& getEnd(void) const;
			void setEnd(const Ogre::Vector3& end);

			/** 
	        */
			Ogre::Real getTimeStep(void) const;
			void setTimeStep(Ogre::Real timeStep);

			/** 
	        */
			Ogre::Real getDrift(void) const;
			void setDrift(Ogre::Real drift);

			/**
	        */
			virtual void _notifyRescaled(const Ogre::Vector3& scale);

			/** 
	        */
			virtual void _firstParticle(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);
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
