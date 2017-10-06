/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ParticleAffectors/ParticleUniverseForceAffector.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Vector3 ForceAffector::DEFAULT_FORCE_VECTOR = Ogre::Vector3::ZERO;
	const ForceAffector::ForceApplication ForceAffector::DEFAULT_FORCE_APPL = ForceAffector::FA_ADD;

	//-----------------------------------------------------------------------
	ForceAffector::ForceAffector(void) : 
		ParticleAffector(),
		mForceVector(DEFAULT_FORCE_VECTOR),
		mScaledVector(Ogre::Vector3::ZERO),
		mForceApplication(DEFAULT_FORCE_APPL)
	{
	}
	//-----------------------------------------------------------------------
	void ForceAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);

		ForceAffector* forceAffector = static_cast<ForceAffector*>(affector);
		forceAffector->mForceVector = mForceVector;
		forceAffector->mForceApplication = mForceApplication;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ForceAffector::getForceVector(void) const
	{
		return mForceVector;
	}
	//-----------------------------------------------------------------------
	void ForceAffector::setForceVector(const Ogre::Vector3& forceVector)
	{
		mForceVector = forceVector;
	}
	//-----------------------------------------------------------------------
	ForceAffector::ForceApplication ForceAffector::getForceApplication(void) const
	{
		return mForceApplication;
	}
	//-----------------------------------------------------------------------
	void ForceAffector::setForceApplication(ForceAffector::ForceApplication forceApplication)
	{
		mForceApplication = forceApplication;
	}
}
