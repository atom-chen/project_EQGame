/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PATH_FOLLOWER_H__
#define __PU_PATH_FOLLOWER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	/** This affector let particles move along a line. The line is derived from a number of points in 3D space.
		Using a spline interpolation, the line becomes smooth.
    */
	class _ParticleUniverseExport PathFollower : public ParticleAffector
	{
		protected:
			Ogre::SimpleSpline mSpline;

		public:
			PathFollower(void) : 
				ParticleAffector()
			{
			};
	        virtual ~PathFollower(void) {};

			/** 
	        */
			void addPoint(const Ogre::Vector3& point);

			/** Clear all points
	        */
			void clearPoints (void);

			/** 
	        */
			unsigned short getNumPoints(void) const {return mSpline.getNumPoints();};
				
			/** 
	        */
			const Ogre::Vector3& getPoint(unsigned short index) const {return mSpline.getPoint(index);};
			
			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);
	};

}
#endif
