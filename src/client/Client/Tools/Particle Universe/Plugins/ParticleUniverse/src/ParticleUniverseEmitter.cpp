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

#include "ParticleUniverseEmitter.h"
#include "ParticleUniverseTechnique.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const bool ParticleEmitter::DEFAULT_ENABLED = true;
	const Ogre::Vector3 ParticleEmitter::DEFAULT_POSITION = Ogre::Vector3::ZERO;
	const bool ParticleEmitter::DEFAULT_KEEP_LOCAL = false;
	const Ogre::Vector3 ParticleEmitter::DEFAULT_DIRECTION = Ogre::Vector3::UNIT_Y;
	const Ogre::Quaternion ParticleEmitter::DEFAULT_ORIENTATION = Ogre::Quaternion::IDENTITY;
	const Ogre::Quaternion ParticleEmitter::DEFAULT_ORIENTATION_RANGE_START = Ogre::Quaternion::IDENTITY;
	const Ogre::Quaternion ParticleEmitter::DEFAULT_ORIENTATION_RANGE_END = Ogre::Quaternion::IDENTITY;
	const Particle::ParticleType ParticleEmitter::DEFAULT_EMITS = VisualParticle::PT_VISUAL;
	const Ogre::uint16 ParticleEmitter::DEFAULT_START_TEXTURE_COORDS = 0;
	const Ogre::uint16 ParticleEmitter::DEFAULT_END_TEXTURE_COORDS = 0;
	const Ogre::uint16 ParticleEmitter::DEFAULT_TEXTURE_COORDS = 0;
	const Ogre::ColourValue ParticleEmitter::DEFAULT_START_COLOUR_RANGE = Ogre::ColourValue::Black;
	const Ogre::ColourValue ParticleEmitter::DEFAULT_END_COLOUR_RANGE = Ogre::ColourValue::White;
	const Ogre::ColourValue ParticleEmitter::DEFAULT_COLOUR = Ogre::ColourValue::White;
	const bool ParticleEmitter::DEFAULT_AUTO_DIRECTION = false;
	const bool ParticleEmitter::DEFAULT_FORCE_EMISSION = false;
	const Ogre::Real ParticleEmitter::DEFAULT_EMISSION_RATE = 10.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_TIME_TO_LIVE = 3.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_MASS = 1.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_VELOCITY = 100.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_DURATION = 0.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_REPEAT_DELAY = 0.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_ANGLE = 20.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_DIMENSIONS = 0.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_WIDTH = 0.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_HEIGHT = 0.0f;
	const Ogre::Real ParticleEmitter::DEFAULT_DEPTH = 0.0f;

	//-----------------------------------------------------------------------
	ParticleEmitter::ParticleEmitter(void) :
		Particle(),
		IAlias(),
		IElement(),
		mParentTechnique(0),
		mEmitsType(DEFAULT_EMITS),
		mEmitsName(Ogre::StringUtil::BLANK),
		mRemainder(0),
		mDurationRemain(0),
		mRepeatDelayRemain(0),
		mDynDurationSet(false),
		mDynRepeatDelaySet(false),
		mParticleDirection(DEFAULT_DIRECTION),
		mOriginalParticleDirection(DEFAULT_DIRECTION),
		mParticleOrientation(Ogre::Quaternion::IDENTITY),
		mParticleOrientationRangeStart(Ogre::Quaternion::IDENTITY),
		mParticleOrientationRangeEnd(Ogre::Quaternion::IDENTITY),
		mParticleOrientationRangeSet(false),
		mUpVector(Ogre::Vector3::ZERO),
		mDynParticleAllDimensionsSet(false),
		mDynParticleWidthSet(false),
		mDynParticleHeightSet(false),
		mDynParticleDepthSet(false),
		mEmissionRateCameraDependency(0),
		mAutoDirection(DEFAULT_AUTO_DIRECTION),
		mForceEmission(DEFAULT_FORCE_EMISSION),
		mOriginalForceEmission(false),
		mForceEmissionExecuted(false),
		mOriginalForceEmissionExecuted(false),
		mName(Ogre::StringUtil::BLANK),
		_mEmitterScale(Ogre::Vector3::UNIT_SCALE),
		mParticleColour(DEFAULT_COLOUR),
		mParticleColourRangeStart(DEFAULT_START_COLOUR_RANGE),
		mParticleColourRangeEnd(DEFAULT_END_COLOUR_RANGE),
		mParticleColourRangeSet(false),
		mKeepLocal(false),
		mParticleTextureCoords(DEFAULT_TEXTURE_COORDS),
		mParticleTextureCoordsRangeStart(DEFAULT_START_TEXTURE_COORDS),
		mParticleTextureCoordsRangeEnd(DEFAULT_END_TEXTURE_COORDS),
		mParticleTextureCoordsRangeSet(false)
	{
		particleType = PT_EMITTER;
		mAliasType = AT_EMITTER;
		mDynEmissionRate = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynEmissionRate))->setValue(DEFAULT_EMISSION_RATE);
		mDynTotalTimeToLive = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynTotalTimeToLive))->setValue(DEFAULT_TIME_TO_LIVE);
		mDynParticleMass = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynParticleMass))->setValue(DEFAULT_MASS);
		mDynVelocity = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynVelocity))->setValue(DEFAULT_VELOCITY);
		mDynDuration = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynDuration))->setValue(DEFAULT_DURATION);
		mDynRepeatDelay = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynRepeatDelay))->setValue(DEFAULT_REPEAT_DELAY);
		mDynAngle = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynAngle))->setValue(DEFAULT_ANGLE);

		// Set the dimensions attributes to 0; the default is to use the default dimensions of the ParticleTechnique
		mDynParticleAllDimensions = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynParticleAllDimensions))->setValue(DEFAULT_DIMENSIONS);
		mDynParticleWidth = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynParticleWidth))->setValue(DEFAULT_WIDTH);
		mDynParticleHeight = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynParticleHeight))->setValue(DEFAULT_HEIGHT);
		mDynParticleDepth = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		(static_cast<DynamicAttributeFixed*>(mDynParticleDepth))->setValue(DEFAULT_DEPTH);
	}
	//-----------------------------------------------------------------------
	ParticleEmitter::~ParticleEmitter(void)
	{
		if (mDynEmissionRate)
			OGRE_DELETE_T(mDynEmissionRate, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynTotalTimeToLive)
			OGRE_DELETE_T(mDynTotalTimeToLive, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynParticleMass)
			OGRE_DELETE_T(mDynParticleMass, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynVelocity)
			OGRE_DELETE_T(mDynVelocity, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynDuration)
			OGRE_DELETE_T(mDynDuration, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynRepeatDelay)
			OGRE_DELETE_T(mDynRepeatDelay, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynParticleAllDimensions)
			OGRE_DELETE_T(mDynParticleAllDimensions, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynParticleWidth)
			OGRE_DELETE_T(mDynParticleWidth, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynParticleHeight)
			OGRE_DELETE_T(mDynParticleHeight, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mDynParticleDepth)
			OGRE_DELETE_T(mDynParticleDepth, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		if (mEmissionRateCameraDependency)
			mCameraDependencyFactory.destroy(mEmissionRateCameraDependency);

		if (mDynAngle)
			OGRE_DELETE_T(mDynAngle, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);
	}
	//-----------------------------------------------------------------------
	bool ParticleEmitter::isKeepLocal(void) const
	{
		return mKeepLocal;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setKeepLocal(bool keepLocal)
	{
		mKeepLocal = keepLocal;
	}
	//-----------------------------------------------------------------------
	bool ParticleEmitter::makeParticleLocal(Particle* particle)
	{
		if (!particle)
			return true;

		if (!mKeepLocal || hasEventFlags(Particle::PEF_EXPIRED))
			return false;

		Ogre::Vector3 diff = position - latestPosition;
		particle->position += diff;
		return true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_notifyStart (void)
	{
		mForceEmission = mOriginalForceEmission;
		mForceEmissionExecuted = mOriginalForceEmissionExecuted;
		mRemainder = 0;
		mDurationRemain = 0;
		mRepeatDelayRemain = 0;
		setEnabled(mOriginalEnabled); // Also calls _initTimeBased
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_notifyRescaled(const Ogre::Vector3& scale)
	{
		_mEmitterScale = scale;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ParticleEmitter::getDerivedPosition(void)
	{
		if (mMarkedForEmission)
		{
			// Use the emitter position, because it is emitted
			// If a particle is emitted, position and derived position are the same
			mDerivedPosition = position;	
		}
		else
		{
			// Add the techniques' derived position. Use the emiters' own 'position' as offset.
			mDerivedPosition = mParentTechnique->getDerivedPosition() + _mEmitterScale * position;
		}
		return mDerivedPosition;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& ParticleEmitter::getParticleColour(void) const
	{
		return mParticleColour;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleColour(const Ogre::ColourValue& particleColour)
	{
		mParticleColour = particleColour;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& ParticleEmitter::getParticleColourRangeStart(void) const
	{
		return mParticleColourRangeStart;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleColourRangeStart(const Ogre::ColourValue& particleColourRangeStart)
	{
		mParticleColourRangeStart = particleColourRangeStart;
		mParticleColourRangeSet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& ParticleEmitter::getParticleColourRangeEnd(void) const
	{
		return mParticleColourRangeEnd;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleColourRangeEnd(const Ogre::ColourValue& particleColourRangeEnd)
	{
		mParticleColourRangeEnd = particleColourRangeEnd;
		mParticleColourRangeSet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::uint16& ParticleEmitter::getParticleTextureCoords(void) const
	{
		return mParticleTextureCoords;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleTextureCoords(const Ogre::uint16& particleTextureCoords)
	{
		mParticleTextureCoords = particleTextureCoords;
	}
	//-----------------------------------------------------------------------
	const Ogre::uint16& ParticleEmitter::getParticleTextureCoordsRangeStart(void) const
	{
		return mParticleTextureCoordsRangeStart;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleTextureCoordsRangeStart(const Ogre::uint16& particleTextureCoordsRangeStart)
	{
		mParticleTextureCoordsRangeStart = particleTextureCoordsRangeStart;
		mParticleTextureCoordsRangeSet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::uint16& ParticleEmitter::getParticleTextureCoordsRangeEnd(void) const
	{
		return mParticleTextureCoordsRangeEnd;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleTextureCoordsRangeEnd(const Ogre::uint16& particleTextureCoordsRangeEnd)
	{
		mParticleTextureCoordsRangeEnd = particleTextureCoordsRangeEnd;
		mParticleTextureCoordsRangeSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::copyAttributesTo(ParticleEmitter* emitter)
	{
		copyParentAttributesTo(emitter);
		emitter->setEmitterType(mEmitterType);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::copyParentAttributesTo(ParticleEmitter* emitter)
	{
		// Copy Parent attributes
		Particle::copyAttributesTo(emitter);

		emitter->setName(mName);
		emitter->setEmitsName(mEmitsName);
		emitter->setEmitsType(mEmitsType);
		emitter->mParticleDirection = mParticleDirection;
		emitter->mOriginalParticleDirection = mOriginalParticleDirection;
		emitter->mParticleOrientation = mParticleOrientation;
		emitter->mParticleOrientationRangeStart = mParticleOrientationRangeStart;
		emitter->mParticleOrientationRangeEnd = mParticleOrientationRangeEnd;
		emitter->mParticleOrientationRangeSet = mParticleOrientationRangeSet;
		emitter->mMarkedForEmission = mMarkedForEmission;
		emitter->mParentTechnique = mParentTechnique;
		emitter->setEmissionRateCameraDependency (mCameraDependencyFactory.clone(getEmissionRateCameraDependency()));
		emitter->mAutoDirection = mAutoDirection;
		emitter->mForceEmission = mForceEmission;
		emitter->mOriginalForceEmission = mOriginalForceEmission;
		emitter->mForceEmissionExecuted = mForceEmissionExecuted;
		emitter->mOriginalForceEmissionExecuted = mOriginalForceEmissionExecuted;
		emitter->mDynDurationSet = mDynDurationSet;
		emitter->mDynRepeatDelaySet = mDynRepeatDelaySet;
		emitter->mDynParticleAllDimensionsSet = mDynParticleAllDimensionsSet;
		emitter->mDynParticleWidthSet = mDynParticleWidthSet;
		emitter->mDynParticleHeightSet = mDynParticleHeightSet;
		emitter->mDynParticleDepthSet = mDynParticleDepthSet;
		emitter->_mEmitterScale = _mEmitterScale;
		emitter->mParticleColour = mParticleColour;
		emitter->mParticleColourRangeStart = mParticleColourRangeStart;
		emitter->mParticleColourRangeEnd = mParticleColourRangeEnd;
		emitter->mParticleColourRangeSet = mParticleColourRangeSet;
		emitter->mParticleTextureCoords = mParticleTextureCoords;
		emitter->mParticleTextureCoordsRangeStart = mParticleTextureCoordsRangeStart;
		emitter->mParticleTextureCoordsRangeEnd = mParticleTextureCoordsRangeEnd;
		emitter->mParticleTextureCoordsRangeSet = mParticleTextureCoordsRangeSet;
		emitter->mKeepLocal = mKeepLocal;

		// Copy Dyn. Emission rate if available
		emitter->setDynEmissionRate(mDynamicAttributeFactory.cloneDynamicAttribute(getDynEmissionRate()));

		// Copy Dyn. Total time to live if available
		emitter->setDynTotalTimeToLive(mDynamicAttributeFactory.cloneDynamicAttribute(getDynTotalTimeToLive()));

		// Copy Dyn. Velocity if available
		emitter->setDynVelocity(mDynamicAttributeFactory.cloneDynamicAttribute(getDynVelocity()));

		// Copy Dyn. Duration if available
		if (mDynDurationSet)
		{
			emitter->setDynDuration(mDynamicAttributeFactory.cloneDynamicAttribute(getDynDuration()));
		}

		// Copy Dyn. RepeatDelay if available
		if (mDynRepeatDelaySet)
		{
			emitter->setDynRepeatDelay(mDynamicAttributeFactory.cloneDynamicAttribute(getDynRepeatDelay()));
		}

		// Copy Dyn. Particle Mass if available
		emitter->setDynParticleMass(mDynamicAttributeFactory.cloneDynamicAttribute(getDynParticleMass()));

		// Copy Dyn. Angle if available
		emitter->setDynAngle(mDynamicAttributeFactory.cloneDynamicAttribute(getDynAngle()));

		// Copy Dyn. own width, height and depth if available
		if (mDynParticleAllDimensionsSet)
		{
			emitter->setDynParticleAllDimensions(mDynamicAttributeFactory.cloneDynamicAttribute(getDynParticleAllDimensions()));
		}
		if (mDynParticleWidthSet)
		{
			emitter->setDynParticleWidth(mDynamicAttributeFactory.cloneDynamicAttribute(getDynParticleWidth()));
		}
		if (mDynParticleHeightSet)
		{
			emitter->setDynParticleHeight(mDynamicAttributeFactory.cloneDynamicAttribute(getDynParticleHeight()));
		}
		if (mDynParticleDepthSet)
		{
			emitter->setDynParticleDepth(mDynamicAttributeFactory.cloneDynamicAttribute(getDynParticleDepth()));
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParentTechnique(ParticleTechnique* parentTechnique)
	{
		mParentTechnique = parentTechnique;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setEmitsName(const Ogre::String& emitsName)
	{
		mEmitsName = emitsName;
		if (mEmitsName != Ogre::StringUtil::BLANK)
		{
			mMarkedForEmission = true;
			if (mParentTechnique)
			{
				// Notify the Technique that something changed
				mParentTechnique->_notifyEmissionChange();
			}
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setEmissionRateCameraDependency(CameraDependency* cameraDependency)
	{
		if (mEmissionRateCameraDependency)
			mCameraDependencyFactory.destroy(mEmissionRateCameraDependency);

		mEmissionRateCameraDependency = cameraDependency;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setEmissionRateCameraDependency(Ogre::Real squareDistance, bool inc)
	{
		if (!mEmissionRateCameraDependency)
		{
			mEmissionRateCameraDependency = mCameraDependencyFactory.create(squareDistance, inc);
		}
		else
		{
			mEmissionRateCameraDependency->setThreshold(squareDistance);
			mEmissionRateCameraDependency->setIncrease(inc);
		}
	}
	//-----------------------------------------------------------------------
	CameraDependency* ParticleEmitter::getEmissionRateCameraDependency(void)
	{
		return mEmissionRateCameraDependency;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynEmissionRate(DynamicAttribute* dynEmissionRate)
	{
		if (mDynEmissionRate)
			OGRE_DELETE_T(mDynEmissionRate, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynEmissionRate = dynEmissionRate;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynTotalTimeToLive(DynamicAttribute* dynTotalTimeToLive)
	{
		if (mDynTotalTimeToLive)
			OGRE_DELETE_T(mDynTotalTimeToLive, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynTotalTimeToLive = dynTotalTimeToLive;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleMass(DynamicAttribute* dynParticleMass)
	{
		if (mDynParticleMass)
			OGRE_DELETE_T(mDynParticleMass, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynParticleMass = dynParticleMass;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynAngle(DynamicAttribute* dynAngle)
	{
		if (mDynAngle)
			OGRE_DELETE_T(mDynAngle, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynAngle = dynAngle;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynVelocity(DynamicAttribute* dynVelocity)
	{
		if (mDynVelocity)
			OGRE_DELETE_T(mDynVelocity, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynVelocity = dynVelocity;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynDuration(DynamicAttribute* dynDuration)
	{
		if (mDynDuration)
			OGRE_DELETE_T(mDynDuration, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynDuration = dynDuration;
		mDynDurationSet = true;
		_initTimeBased();
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynDurationSet(bool durationSet)
	{
		mDynDurationSet = durationSet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynRepeatDelay(DynamicAttribute* dynRepeatDelay)
	{
		if (mDynRepeatDelay)
			OGRE_DELETE_T(mDynRepeatDelay, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynRepeatDelay = dynRepeatDelay;
		mDynRepeatDelaySet = true;
		_initTimeBased();
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynRepeatDelaySet(bool repeatDelaySet)
	{
		mDynRepeatDelaySet = repeatDelaySet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleAllDimensions(DynamicAttribute* dynParticleAllDimensions)
	{
		if (mDynParticleAllDimensions)
			OGRE_DELETE_T(mDynParticleAllDimensions, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynParticleAllDimensions = dynParticleAllDimensions;
		mDynParticleAllDimensionsSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleAllDimensionsSet(bool particleAllDimensionsSet)
	{
		mDynParticleAllDimensionsSet = particleAllDimensionsSet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleWidth(DynamicAttribute* dynParticleWidth)
	{
		if (mDynParticleWidth)
			OGRE_DELETE_T(mDynParticleWidth, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynParticleWidth = dynParticleWidth;
		mDynParticleWidthSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleWidthSet(bool particleWidthSet)
	{
		mDynParticleWidthSet = particleWidthSet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleHeight(DynamicAttribute* dynParticleHeight)
	{
		if (mDynParticleHeight)
			OGRE_DELETE_T(mDynParticleHeight, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynParticleHeight = dynParticleHeight;
		mDynParticleHeightSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleHeightSet(bool particleHeightSet)
	{
		mDynParticleHeightSet = particleHeightSet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleDepth(DynamicAttribute* dynParticleDepth)
	{
		if (mDynParticleDepth)
			OGRE_DELETE_T(mDynParticleDepth, DynamicAttribute, Ogre::MEMCATEGORY_SCENE_OBJECTS);

		mDynParticleDepth = dynParticleDepth;
		mDynParticleDepthSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setDynParticleDepthSet(bool particleDepthSet)
	{
		mDynParticleDepthSet = particleDepthSet;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setEnabled(bool enabled)
	{
		Particle::setEnabled(enabled);
		_initTimeBased();
    }
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ParticleEmitter::getParticleDirection(void) const
	{
		return mParticleDirection;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleDirection(const Ogre::Vector3& direction)
	{
		mParticleDirection = direction;
		mOriginalParticleDirection = direction;
		mParticleDirection.normalise();
		mUpVector = mParticleDirection.perpendicular();
		mUpVector.normalise();
    }
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ParticleEmitter::getOriginalParticleDirection(void) const
	{
		return mOriginalParticleDirection;
	}
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& ParticleEmitter::getParticleOrientation(void) const
	{
		return mParticleOrientation;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleOrientation(const Ogre::Quaternion& orientation)
	{
		mParticleOrientation = orientation;
    }
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& ParticleEmitter::getParticleOrientationRangeStart(void) const
	{
		return mParticleOrientationRangeStart;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleOrientationRangeStart(const Ogre::Quaternion& orientationRangeStart)
	{
		mParticleOrientationRangeStart = orientationRangeStart;
		mParticleOrientationRangeSet = true;
    }
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& ParticleEmitter::getParticleOrientationRangeEnd(void) const
	{
		return mParticleOrientationRangeEnd;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setParticleOrientationRangeEnd(const Ogre::Quaternion& orientationRangeEnd)
	{
		mParticleOrientationRangeEnd = orientationRangeEnd;
		mParticleOrientationRangeSet = true;
    }
	//-----------------------------------------------------------------------
	bool ParticleEmitter::isAutoDirection(void) const
	{
		return mAutoDirection;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setAutoDirection(bool autoDirection)
	{
		mAutoDirection = autoDirection;
    }
	//-----------------------------------------------------------------------
	bool ParticleEmitter::isForceEmission(void) const
	{
		return mForceEmission;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::setForceEmission(bool forceEmission)
	{
		mForceEmission = forceEmission;
		mOriginalForceEmission = forceEmission;
		mForceEmissionExecuted = false;
		mOriginalForceEmissionExecuted = false;
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed)
	{
		// Calculate the emitters' position so it is safe to use mDerivedPosition).
		getDerivedPosition();
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initTimeBased(void)
    {
		if (mEnabled)
		{
			if (mDynDurationSet && mParentTechnique)
			{
				mDurationRemain = mDynamicAttributeHelper.calculate(mDynDuration, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
			}

			/** Determine whether duration must be used. If it is used, the emitter should at least be enabled.
			@remarks
				Note, that if the duration has been exceeded, the emitter is disabled. It makes perfect sense to enable 
				the emitter again when the duration is initialised with a valid value (> 0).
			*/
			if (mDurationRemain > 0)
			{
				mEnabled = true;
				mRepeatDelayRemain = 0;
			}
		}
		else
		{
			if (mDynRepeatDelaySet && mParentTechnique)
			{
				mRepeatDelayRemain = mDynamicAttributeHelper.calculate(mDynRepeatDelay, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
			}
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initForEmission(void)
	{
		// The emitter itself is emitted.
		Particle::_initForEmission();

		// Emitting an emitter is similar as starting one.
		_notifyStart();
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initForExpiration(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		// The emitter itself is expired.
		Particle::_initForExpiration(technique, timeElapsed);

		// Expiring an emitter is similar as stopping one.
		_notifyStop();
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticlePosition(Particle* particle)
	{
		particle->position = getDerivedPosition();
		particle->originalPosition = particle->position;
		particle->latestPosition = particle->position;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleForEmission(Particle* particle)
	{
		// Initialise the particle position (localspace)
		particle->parentEmitter = this;
		_initParticlePosition(particle);
		_initParticleDirection(particle);
		_initParticleVelocity(particle);
		_initParticleOrientation(particle);
		_initParticleMass(particle);
		_initParticleColour(particle);
		_initParticleTextureCoords(particle);
		particle->totalTimeToLive = _initParticleTimeToLive();
		particle->timeToLive = particle->totalTimeToLive;
		
		// Generate particles' own dimensions if defined.
		_initParticleDimensions(particle);
	}
	//-----------------------------------------------------------------------
    void ParticleEmitter::_initParticleDirection(Particle* particle)
    {
		// Use the default way of initialising the particle direction
		Ogre::Radian angle;
		_generateAngle(angle);
		if (angle != Ogre::Radian(0))
		{
			particle->direction = mParticleDirection.randomDeviant(angle, mUpVector);
		}
		else
		{
			particle->direction = mParticleDirection;
		}
		particle->originalDirection = particle->direction;
		particle->originalDirectionLength = particle->direction.length();
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_generateAngle(Ogre::Radian& angle)
	{
		Ogre::Radian a = Ogre::Angle(mDynamicAttributeHelper.calculate(mDynAngle, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart()));
		angle = a;
		if (mDynAngle->getType() == DynamicAttribute::DAT_FIXED)
		{
			// Make an exception here and don´t use the fixed angle.
			angle = Ogre::Math::UnitRandom() * angle;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleVelocity(Particle* particle)
    {
		Ogre::Real scalar = mDynamicAttributeHelper.calculate(mDynVelocity, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart(), 1.0f);
		particle->direction *= scalar;
		particle->originalVelocity = scalar;
		particle->originalScaledDirectionLength = particle->direction.length();

		// Don´t change the originalDirection, because the real original value has already been set.
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleOrientation(Particle* particle)
	{
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		if (mParticleOrientationRangeSet)
		{
			// Generate random orientation 'between' start en end.
			visualParticle->orientation = Ogre::Quaternion::nlerp(Ogre::Math::UnitRandom(), 
				mParticleOrientationRangeStart, 
				mParticleOrientationRangeEnd,
				true);
		}
		else
		{
			visualParticle->orientation = mParticleOrientation;
		}

		// Set original orientation
		visualParticle->originalOrientation = visualParticle->orientation;
    }
	//-----------------------------------------------------------------------
    void ParticleEmitter::_initParticleMass(Particle* particle)
    {
		Ogre::Real mass = mDynamicAttributeHelper.calculate(mDynParticleMass, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart(), Particle::DEFAULT_MASS);
		particle->mass = mass;
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleColour(Particle* particle)
    {
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		if (mParticleColourRangeSet)
		{
			visualParticle->colour.r = Ogre::Math::RangeRandom(mParticleColourRangeStart.r, mParticleColourRangeEnd.r);
			visualParticle->colour.g = Ogre::Math::RangeRandom(mParticleColourRangeStart.g, mParticleColourRangeEnd.g);
			visualParticle->colour.b = Ogre::Math::RangeRandom(mParticleColourRangeStart.b, mParticleColourRangeEnd.b);
			visualParticle->colour.a = Ogre::Math::RangeRandom(mParticleColourRangeStart.a, mParticleColourRangeEnd.a);
		}
		else
		{
			visualParticle->colour = mParticleColour;
		}

		// Set original colour
		visualParticle->originalColour = visualParticle->colour;
    }
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleTextureCoords(Particle* particle)
    {
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		if (mParticleTextureCoordsRangeSet)
		{
			visualParticle->textureCoordsCurrent = Ogre::Math::RangeRandom(mParticleTextureCoordsRangeStart, mParticleTextureCoordsRangeEnd);
		}
		else
		{
			visualParticle->textureCoordsCurrent = mParticleTextureCoords;
		}
    }
	//-----------------------------------------------------------------------
    Ogre::Real ParticleEmitter::_initParticleTimeToLive(void)
    {
		/*  Generate a value for totalTimeToLive. Use time since start from the particle system as argument 
			in getValue(). If mDynTotalTimeToLive doesn´t exist, use the default value.
		*/
		return mDynamicAttributeHelper.calculate(mDynTotalTimeToLive, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart(), Particle::DEFAULT_TTL);
    }
    //-----------------------------------------------------------------------
	unsigned short ParticleEmitter::_calculateRequestedParticles(Ogre::Real timeElapsed)
	{
		unsigned short requestedParticles = 0;
		Ogre::Real f = 0;
        
		if (mEnabled)
		{
			if (mDynEmissionRate && mParentTechnique && mParentTechnique->getParentSystem())
			{
				Ogre::Real rate = mDynEmissionRate->getValue(mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
				if (mEmissionRateCameraDependency)
				{
					// Affect the emission rate based on the camera distance
					mEmissionRateCameraDependency->affect(rate, mParentTechnique->getCameraSquareDistance());
				}
				if (mForceEmission)
				{
					if (mForceEmissionExecuted)
					{
						// It is a single-shot system, so there is nothing left anymore.
						requestedParticles = 0;
					}
					else
					{
						// Ignore the time. Just emit everything at once (if you absolutely need it).
						// The emitter cannot be disabled yet, because it needs to emit its particles first.
						requestedParticles = rate;
						mForceEmissionExecuted = true;
					}
				}
				else
				{
					// Particle emission is time driven
					mRemainder += rate * timeElapsed;
					requestedParticles = (unsigned short)mRemainder;
				}
				
				mRemainder -= requestedParticles;
			}

			// Determine whether the duration period has been exceeded.
			if (mDynDurationSet)
			{
				mDurationRemain -= timeElapsed;
				if (mDurationRemain <= 0)
				{
					setEnabled(false);
				}
			}
		}
		else if (mDynRepeatDelaySet)
		{
			mRepeatDelayRemain -= timeElapsed;
			if (mRepeatDelayRemain <= 0)
			{
				// Initialise again
				setEnabled(true);
			}
		}

		return requestedParticles;
	}
	//-----------------------------------------------------------------------
	void ParticleEmitter::_initParticleDimensions(Particle* particle)
    {
		if (!particle->particleType == Particle::PT_VISUAL)
			return;

		// Only continue if one of them is set
		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
		if (mDynParticleAllDimensionsSet || mDynParticleWidthSet || mDynParticleHeightSet || mDynParticleDepthSet)
		{
			// Set all dimensions equal ...
			Ogre::Real extend = 0;
			if (mDynParticleAllDimensionsSet && mDynParticleAllDimensions)
			{
				extend = mDynamicAttributeHelper.calculate(mDynParticleAllDimensions, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
				visualParticle->setOwnDimensions(_mEmitterScale.x * extend, _mEmitterScale.y * extend, _mEmitterScale.z * extend);
				return;
			}

			// ... or set the dimensions independent from each other
			Ogre::Real width = 0;
			Ogre::Real height = 0;
			Ogre::Real depth = 0;
			if (mDynParticleWidthSet && mDynParticleWidth)
			{
				width = mDynamicAttributeHelper.calculate(mDynParticleWidth, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
			}
			if (mDynParticleHeightSet && mDynParticleHeight)
			{
				height = mDynamicAttributeHelper.calculate(mDynParticleHeight, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
			}
			if (mDynParticleDepthSet && mDynParticleDepth)
			{
				depth = mDynamicAttributeHelper.calculate(mDynParticleDepth, mParentTechnique->getParentSystem()->getTimeElapsedSinceStart());
			}
	
			/** Set the width, height and depth if at least one of them is set.
			@remarks
				If one of the dimensions is 0, it will be overridden by the default value later on.
			*/
			if (mDynParticleWidthSet || mDynParticleHeightSet || mDynParticleDepthSet)
			{
				visualParticle->setOwnDimensions(_mEmitterScale.x * width, _mEmitterScale.y * height, _mEmitterScale.z * depth);
			}
		}
		else
		{
			// Just set the width, height and depth, but these are just the default settings; the particle doesn't
			// have own dimensions. Recalculate the bounding sphere radius.
			visualParticle->width = _mEmitterScale.x * mParentTechnique->getDefaultWidth();
			visualParticle->height = _mEmitterScale.y * mParentTechnique->getDefaultHeight();
			visualParticle->depth = _mEmitterScale.z * mParentTechnique->getDefaultDepth();
			visualParticle->_calculateBoundingSphereRadius();
		}
	}
}
