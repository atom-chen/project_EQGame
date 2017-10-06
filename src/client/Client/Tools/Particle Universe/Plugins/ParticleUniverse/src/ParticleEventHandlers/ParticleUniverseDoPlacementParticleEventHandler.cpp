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

#include "ParticleEventHandlers/ParticleUniverseDoPlacementParticleEventHandler.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"
#include "ParticleUniverseParticle.h"
#include "ParticleUniversePhysicsActor.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Real DoPlacementParticleEventHandler::DEFAULT_NUMBER_OF_PARTICLES = 1;

	//-----------------------------------------------------------------------
	DoPlacementParticleEventHandler::DoPlacementParticleEventHandler(void) : 
		ParticleEventHandler(),
		TechniqueListener(),
		mForceEmitterName(Ogre::StringUtil::BLANK),
		mNumberOfParticles(DEFAULT_NUMBER_OF_PARTICLES),
		mPosition(Ogre::Vector3::ZERO),
		mFound(false),
		mAlwaysUsePosition(true),
		mEmitter(0),
		mTechnique(0)
	{
	}
	//-----------------------------------------------------------------------
	DoPlacementParticleEventHandler::~DoPlacementParticleEventHandler(void)
	{
		// We cannot remove this listener from mTechnique, because it is undetermined whether the ParticleTechnique 
		// still exist.
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandler::_handle (ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed)
	{
		if (!particle)
			return;

#ifdef PU_PHYSICS
		// Do not assume that the contact point is always used if a physics engine is used.
		if (!mAlwaysUsePosition && particle->physicsActor)
		{
			mPosition = particle->physicsActor->contactPoint; // Store the contact point to spawn new particles on that position.
		}
		else
		{
			mPosition = particle->position; // Store the particles' position to spawn new particles on that position.
		}
#else
		mPosition = particle->position; // Store the particles' position to spawn new particles on that position.
#endif // PU_PHYSICS

		if (!mFound)
		{
			ParticleTechnique* technique = particleTechnique;
			ParticleEmitter* emitter = particleTechnique->getEmitter(mForceEmitterName);
			if (!emitter)
			{
				// Search all techniques in this ParticleSystem for an emitter with the correct name
				ParticleSystem* system = particleTechnique->getParentSystem();
				size_t size = system->getNumTechniques();
				for(size_t i = 0; i < size; ++i)
				{
					technique = system->getTechnique(i);
					emitter = technique->getEmitter(mForceEmitterName);
					if (emitter)
						break;
				}
			}

			if (emitter)
			{
				mTechnique = technique;
				mEmitter = emitter;
				if (mTechnique)
				{
					mTechnique->addTechniqueListener(this);
				}
				mFound = true;
			}
			else
			{
				return;
			}
		}

		// Emit 1 or more particles
		if (mTechnique)
		{
			mTechnique->forceEmission(mEmitter, mNumberOfParticles);
		}
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandler::particleEmitted(ParticleTechnique* particleTechnique, Particle* particle)
	{
		if (particle && mEmitter == particle->parentEmitter)
		{
			particle->position = mPosition;
		}
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandler::setForceEmitterName(const Ogre::String& forceEmitterName)
	{
		mForceEmitterName = forceEmitterName;
	}
	//-----------------------------------------------------------------------
	ParticleEmitter* DoPlacementParticleEventHandler::getForceEmitter(void) const
	{
		return mEmitter;
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandler::removeAsListener(void)
	{
		// Reset some values and remove this as a listener from the old technique.
		if (mTechnique)
		{
			mTechnique->removeTechniqueListener(this);
		}
		mFound = false;
		mEmitter = 0;
		mTechnique = 0;
	}
	//-----------------------------------------------------------------------
	void DoPlacementParticleEventHandler::copyAttributesTo (ParticleEventHandler* eventHandler)
	{
		ParticleEventHandler::copyAttributesTo(eventHandler);
		DoPlacementParticleEventHandler* doPlacementParticleEventHandler = static_cast<DoPlacementParticleEventHandler*>(eventHandler);
		doPlacementParticleEventHandler->setForceEmitterName(mForceEmitterName);
		doPlacementParticleEventHandler->setNumberOfParticles(mNumberOfParticles);
		doPlacementParticleEventHandler->mAlwaysUsePosition = mAlwaysUsePosition;
	}

}
