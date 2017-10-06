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

#include "ParticleRenderers/ParticleUniverseSphere.h"
#include "ParticleRenderers/ParticleUniverseSphereSet.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	Sphere::Sphere():
        mOwnRadius(false),
		mPosition(Ogre::Vector3::ZERO),
		mColour(Ogre::ColourValue::White),
        mParentSet(0),
		mRadius(100.0f),
		mRadiusChanged(false)
	{
		mOrientation = Ogre::Quaternion::IDENTITY;
	}
    //-----------------------------------------------------------------------
    Sphere::Sphere(const Ogre::Vector3& position, SphereSet* owner) :
        mOwnRadius(false),
		mPosition(Ogre::Vector3::ZERO),
		mColour(Ogre::ColourValue::White),
        mParentSet(0),
		mRadius(10.0f),
		mRadiusChanged(false)
	{
		mOrientation = Ogre::Quaternion::IDENTITY;
	}
	//-----------------------------------------------------------------------
	Sphere::~Sphere()
	{
	}
	//-----------------------------------------------------------------------
	void Sphere::setPosition(const Ogre::Vector3& position)
	{
		mPosition = position;
	}
	//-----------------------------------------------------------------------
	void Sphere::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& Sphere::getPosition(void) const
	{
		return mPosition;
	}
	//-----------------------------------------------------------------------
	void Sphere::setRadius(Ogre::Real radius)
	{
		mOwnRadius = true;
		mRadius = radius;
		if (mParentSet)
			mParentSet->_notifyResized();
		mRadiusChanged = true;
    }
	//-----------------------------------------------------------------------
	bool Sphere::hasOwnRadius(void) const
	{
		return mOwnRadius;
	}
	//-----------------------------------------------------------------------
	void Sphere::_notifyOwner(SphereSet* owner)
	{
		mParentSet = owner;
	}
	//-----------------------------------------------------------------------
	void Sphere::setColour(const Ogre::ColourValue& colour)
	{
		mColour = colour;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& Sphere::getColour(void) const
	{
		return mColour;
	}
	//-----------------------------------------------------------------------
	Ogre::Real Sphere::getOwnRadius(void) const
	{
		return mRadius;
	}

}
