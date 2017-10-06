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

#include "ParticleUniverseCameraDependency.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real CameraDependency::DEFAULT_DISTANCE_THRESHOLD = 1000.0f;
	const bool CameraDependency::DEFAULT_INCREASE = false;

	//-----------------------------------------------------------------------
	CameraDependency::CameraDependency(void) :
		IDependency(),
		mThreshold(DEFAULT_DISTANCE_THRESHOLD),
		mIncrease(DEFAULT_INCREASE)
	{
	}
	//-----------------------------------------------------------------------
	CameraDependency::CameraDependency(Ogre::Real threshold, bool inc) :
		IDependency(),
		mThreshold(threshold),
		mIncrease(inc)
	{
	}
	//-----------------------------------------------------------------------
	bool CameraDependency::affect(Ogre::Real& baseValue, Ogre::Real dependencyValue)
	{
		Ogre::Real f = 0;
		bool changed = false;
		if (mIncrease)
		{
			if (mThreshold != 0)
				f = dependencyValue / mThreshold;
		}
		else
		{
			if (dependencyValue != 0)
				f = mThreshold / dependencyValue;
		}
		if (f > 0 && f <= 1)
		{
			baseValue *= f;
			changed = true;
		}
		
		return changed;
	}
	//-----------------------------------------------------------------------
	void CameraDependency::copyAttributesTo (CameraDependency* cameraDependency)
	{
		cameraDependency->setThreshold(mThreshold);
		cameraDependency->setIncrease(mIncrease);
	}

}
