/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONPOSITION_OBSERVER_H__
#define __PU_ONPOSITION_OBSERVER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseCommon.h"
#include "ParticleUniverseObserver.h"

namespace ParticleUniverse
{
	/** This class is used to observe whether a Particle reaches a certain position.
    */
	class _ParticleUniverseExport OnPositionObserver : public ParticleObserver
	{
		protected:
			Ogre::Real mPositionXThreshold;
			Ogre::Real mPositionYThreshold;
			Ogre::Real mPositionZThreshold;
			bool mPositionXThresholdSet;
			bool mPositionYThresholdSet;
			bool mPositionZThresholdSet;
			ComparisionOperator mComparePositionX;
			ComparisionOperator mComparePositionY;
			ComparisionOperator mComparePositionZ;

		public:
			// Constants
			static const Ogre::Vector3 DEFAULT_POSITION_THRESHOLD;

			OnPositionObserver(void);
	        virtual ~OnPositionObserver(void) {};

			/** 
	        */
			virtual bool _observe (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** 
			 */
			void setPositionXThreshold(Ogre::Real threshold){mPositionXThreshold = threshold; mPositionXThresholdSet = true;};
			void setPositionYThreshold(Ogre::Real threshold){mPositionYThreshold = threshold; mPositionYThresholdSet = true;};
			void setPositionZThreshold(Ogre::Real threshold){mPositionZThreshold = threshold; mPositionZThresholdSet = true;};

			/** 
			 */
			Ogre::Real getPositionXThreshold(void) const {return mPositionXThreshold;};
			Ogre::Real getPositionYThreshold(void) const {return mPositionYThreshold;};
			Ogre::Real getPositionZThreshold(void) const {return mPositionZThreshold;};

			/** 
			 */
			bool isPositionXThresholdSet(void) const {return mPositionXThresholdSet;};
			bool isPositionYThresholdSet(void) const {return mPositionYThresholdSet;};
			bool isPositionZThresholdSet(void) const {return mPositionZThresholdSet;};

			/** 
			 */
			void resetPositionXThreshold(void) {mPositionXThresholdSet = false;};
			void resetPositionYThreshold(void) {mPositionYThresholdSet = false;};
			void resetPositionZThreshold(void) {mPositionZThresholdSet = false;};

			/** 
			 */
			void setComparePositionX(ComparisionOperator op){mComparePositionX = op;};
			void setComparePositionY(ComparisionOperator op){mComparePositionY = op;};
			void setComparePositionZ(ComparisionOperator op){mComparePositionZ = op;};

			/** 
			 */
			const ComparisionOperator getComparePositionX(void) const {return mComparePositionX;};
			const ComparisionOperator getComparePositionY(void) const {return mComparePositionY;};
			const ComparisionOperator getComparePositionZ(void) const {return mComparePositionZ;};

			/** Copy attributes to another observer.
	        */
			virtual void copyAttributesTo (ParticleObserver* observer);
	};

}
#endif
