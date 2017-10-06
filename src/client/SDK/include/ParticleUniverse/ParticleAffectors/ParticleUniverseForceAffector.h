/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_FORCE_AFFECTOR_H__
#define __PU_FORCE_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	/** This is a baseclass for other Force Affector classes.
    */
	class _ParticleUniverseExport ForceAffector : public ParticleAffector
	{
		public:
			enum ForceApplication
			{
				FA_AVERAGE,
				FA_ADD
			};

			// Constants
			static const Ogre::Vector3 DEFAULT_FORCE_VECTOR;
			static const ForceApplication DEFAULT_FORCE_APPL;

			ForceAffector(void);
	        virtual ~ForceAffector(void) {};

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** 
	        */
			const Ogre::Vector3& getForceVector(void) const;
			void setForceVector(const Ogre::Vector3& forceVector);

			/** 
	        */
			ForceApplication getForceApplication(void) const;
			void setForceApplication(ForceApplication forceApplication);

		protected:
			Ogre::Vector3 mForceVector;
			Ogre::Vector3 mScaledVector;
			ForceApplication mForceApplication;
	};

}
#endif
