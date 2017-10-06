/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SINE_FORCE_AFFECTOR_H__
#define __PU_SINE_FORCE_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffector.h"

namespace ParticleUniverse
{
	/** Applies a sine force to a particle.
    */
	class _ParticleUniverseExport SineForceAffector : public ForceAffector
	{
		public:
			// Constants
			static const Ogre::Real DEFAULT_FREQ_MIN;
			static const Ogre::Real DEFAULT_FREQ_MAX;

			SineForceAffector(void);
	        virtual ~SineForceAffector(void) {};

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** 
	        */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed);

			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** 
	        */
			const Ogre::Real getFrequencyMin(void) const;
			void setFrequencyMin(const Ogre::Real frequencyMin);

			/** 
	        */
			const Ogre::Real getFrequencyMax(void) const;
			void setFrequencyMax(const Ogre::Real frequencyMax);

		protected:
			Ogre::Real mAngle;
			Ogre::Real mFrequencyMin;
			Ogre::Real mFrequencyMax;
			Ogre::Real mFrequency;
	};

}
#endif
