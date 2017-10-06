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

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX
	#include "Externs/ParticleUniversePhysXExtern.h"
	#include "Externs/ParticleUniversePhysXBridge.h"
	#include "Externs/ParticleUniversePhysXMath.h"
	#include "ParticleUniverseTechnique.h"

	namespace ParticleUniverse
	{
		//-----------------------------------------------------------------------
		PhysXExtern::PhysXExtern(void) : 
			PhysicsExtern(), 
			mScene(0),
			mSynchronize(false),
			mTimeElapsedSynchronize(0.0f),
			mPhysicsShapeDesc(0),
			mActorCollisionGroup(0),
			mFluid(0),
			mFluidSet(false),
			mPhysXParticles(0),
			mMaxParticles(0),
			mPhysXParticlesPointer(0),
			mStart(0)
		{
			// Set default physics shape
			mPhysicsShapeDesc = OGRE_NEW_T(PhysicsBoxDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS)();

			// Set default fluid settings
			mFluidDesc.kernelRadiusMultiplier = 2.3f;
			mFluidDesc.restParticlesPerMeter = 0.3f;
			mFluidDesc.motionLimitMultiplier = 2.0f;
			mFluidDesc.packetSizeMultiplier = 8;
			mFluidDesc.stiffness = 20.0f;
			mFluidDesc.viscosity = 10.0f;
			mFluidDesc.restDensity = 1000.0f;
			mFluidDesc.damping = 0.0f;
			mFluidDesc.surfaceTension = 10.0f;
			//mFluidDesc.simulationMethod = NX_F_SPH;
			mFluidDesc.simulationMethod = NX_F_NO_PARTICLE_INTERACTION;
			mFluidDesc.externalAcceleration = NxVec3(0.0f, 0.0f, 0.0f);
			mFluidDesc.flags &= ~NX_FF_HARDWARE;
		}
		//-----------------------------------------------------------------------
        PhysXExtern::~PhysXExtern(void)
		{
			// Force both the PhysX engine and the parent technique to expire all particles; this destroys all remaining actors.
			mParentTechnique->_initAllParticlesForExpiration();

			// Ignores unregistering if the PhysXExtern wasn´t registered in the first place.
			PhysXBridge::getSingletonPtr()->unRegisterPhysXExtern(this);

			if (mPhysicsShapeDesc)
			{
				OGRE_DELETE_T(mPhysicsShapeDesc, PhysicsShapeDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			}

			// Destroy fluid
			destroyFluid();
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_notifyStop (void)
		{
			if (mFluidSet)
			{
				mFluid->removeAllParticles();
				mPhysXParticlesPointer = 0;
			}
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::setPhysicsShape(PhysicsShapeDesc* physicsShapeDesc)
		{
			if (mPhysicsShapeDesc)
			{
				OGRE_DELETE_T(mPhysicsShapeDesc, PhysicsShapeDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			}

			switch (physicsShapeDesc->mPhysicsShapeType)
			{
				case ST_BOX:
				{
					PhysicsBoxDesc* physicsBoxDesc = static_cast<PhysicsBoxDesc*>(physicsShapeDesc);
					mPhysicsShapeDesc = OGRE_NEW_T(PhysicsBoxDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS)(*physicsBoxDesc);
				}
				break;

				case ST_SPHERE:
				{
					PhysicsSphereDesc* physicsSphereDesc = static_cast<PhysicsSphereDesc*>(physicsShapeDesc);
					mPhysicsShapeDesc = OGRE_NEW_T(PhysicsSphereDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS)(*physicsSphereDesc);
				}
				break;

				case ST_CAPSULE:
				{
					PhysicsCapsuleDesc* physicsCapsuleDesc = static_cast<PhysicsCapsuleDesc*>(physicsShapeDesc);
					mPhysicsShapeDesc = OGRE_NEW_T(PhysicsCapsuleDesc, Ogre::MEMCATEGORY_SCENE_OBJECTS)(*physicsCapsuleDesc);
				}
				break;
			}
		}
		//-----------------------------------------------------------------------
		Ogre::uint16 PhysXExtern::getActorGroup(void) const
		{
			return mActorCollisionGroup;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::setActorGroup(Ogre::uint16 actorCollisionGroup)
		{
			mActorCollisionGroup = actorCollisionGroup;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_initParticleForEmission(Particle* particle)
		{
			if (mFluidSet)
			{
				// In case of fluid settings, add the particle to the buffer
				createPhysicsActorForFluid(particle);
				return;
			}

			// No fluid defined, so it is a regular actor.
			if (particle->particleType != Particle::PT_VISUAL)
				return;
			
			VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
			PhysicsActorDesc physicsActorDesc;
			physicsActorDesc.position = particle->position;
			physicsActorDesc.direction = particle->direction;
			physicsActorDesc.mass = particle->mass;
			physicsActorDesc.orientation = visualParticle->orientation;
			physicsActorDesc.collisionGroup = mActorCollisionGroup;
			NxActorDesc actorDesc;
			
			// Use the particle dimensions. A particle is (for now) either a box or a sphere. Never a plane.
			switch (mPhysicsShapeDesc->mPhysicsShapeType)
			{
				case ST_BOX:
				{
					PhysicsBoxDesc* physicsBoxDesc = static_cast<PhysicsBoxDesc*>(mPhysicsShapeDesc);
					physicsBoxDesc->mDimensions = 0.5 * Ogre::Vector3(visualParticle->width, visualParticle->height, visualParticle->depth);
				}
				break;

				case ST_SPHERE:
				{
					PhysicsSphereDesc* physicsSphereDesc = static_cast<PhysicsSphereDesc*>(mPhysicsShapeDesc);
					physicsSphereDesc->mRadius = 0.5 * visualParticle->width;
				}
				break;

				case ST_CAPSULE:
				{
					PhysicsCapsuleDesc* physicsCapsuleDesc = static_cast<PhysicsCapsuleDesc*>(mPhysicsShapeDesc);
					physicsCapsuleDesc->mRadius = 0.5 * visualParticle->width;
					physicsCapsuleDesc->mHeight = 0.5 * visualParticle->height;
				}
				break;
			}

			PhysicsActor* actor = createPhysicsActor(&physicsActorDesc, mPhysicsShapeDesc);
			particle->physicsActor = actor;
			(static_cast<PhysXActor*>(actor))->nxActor->userData = particle; // So particles and actors refer to each other
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_initParticleForExpiration(Particle* particle)
		{
			if (mFluidSet)
			{
				// Delete both the physics actor and its nxParticleData
				destroyPhysicsActorForFluid(particle->physicsActor);
			}
			else
			{
				// No fluid defined, so the particles are regular actors.
				destroyPhysicsActor(particle->physicsActor);
			}
			particle->physicsActor = 0;
		}
		//-----------------------------------------------------------------------
		PhysicsActor* PhysXExtern::createPhysicsActor(PhysicsActorDesc* physicsActorDesc, PhysicsShapeDesc* physicsShapeDesc)
		{
			if (!PhysXBridge::getSingletonPtr()->getScene() || !physicsActorDesc || !physicsShapeDesc)
				return 0;

			NxBodyDesc bodyDesc;
			bodyDesc.angularDamping = physicsShapeDesc->mAngularDamping;
			bodyDesc.angularVelocity = PhysXMath::convert(physicsShapeDesc->mAngularVelocity);
			bodyDesc.linearVelocity = PhysXMath::convert(physicsActorDesc->direction);

			NxActorDesc actorDesc;
			switch (physicsShapeDesc->mPhysicsShapeType)
			{
				case ST_BOX:
				{
					PhysicsBoxDesc* physicsBoxDesc = static_cast<PhysicsBoxDesc*>(physicsShapeDesc);
					NxBoxShapeDesc boxDesc;
					boxDesc.dimensions = PhysXMath::convert(physicsBoxDesc->mDimensions);
					boxDesc.group = physicsBoxDesc->mCollisionGroup;
					boxDesc.groupsMask = PhysXMath::convert(physicsBoxDesc->mGroupMask);
					boxDesc.materialIndex = physicsBoxDesc->mMaterialIndex;
					actorDesc.density = NxComputeBoxDensity(2 * boxDesc.dimensions, physicsActorDesc->mass);
					actorDesc.shapes.pushBack(&boxDesc);
				}
				break;

				case ST_SPHERE:
				{
					PhysicsSphereDesc* physicsSphereDesc = static_cast<PhysicsSphereDesc*>(physicsShapeDesc);
					NxSphereShapeDesc sphereDec;
					sphereDec.radius = physicsSphereDesc->mRadius;
					sphereDec.group = physicsSphereDesc->mCollisionGroup;
					sphereDec.groupsMask = PhysXMath::convert(physicsSphereDesc->mGroupMask);
					sphereDec.materialIndex = physicsSphereDesc->mMaterialIndex;
					actorDesc.density = NxComputeSphereDensity(sphereDec.radius, physicsActorDesc->mass);
					actorDesc.shapes.pushBack(&sphereDec);
				}
				break;

				case ST_CAPSULE:
				{
					PhysicsCapsuleDesc* physicsCapsuleDesc = static_cast<PhysicsCapsuleDesc*>(physicsShapeDesc);
					NxCapsuleShapeDesc capsuleDec;
					capsuleDec.radius = physicsCapsuleDesc->mRadius;
					capsuleDec.height = physicsCapsuleDesc->mHeight;
					capsuleDec.group = physicsCapsuleDesc->mCollisionGroup;
					capsuleDec.groupsMask = PhysXMath::convert(physicsCapsuleDesc->mGroupMask);
					capsuleDec.materialIndex = physicsCapsuleDesc->mMaterialIndex;
					actorDesc.density = NxComputeCylinderDensity(capsuleDec.radius, capsuleDec.height, physicsActorDesc->mass);
					actorDesc.shapes.pushBack(&capsuleDec);
				}
				break;
			}
			actorDesc.globalPose.t = PhysXMath::convert(physicsActorDesc->position);
			actorDesc.body = &bodyDesc;
			actorDesc.group = physicsActorDesc->collisionGroup;
			PhysXActor* physXActor = 0;
			NxActor* nxActor = PhysXBridge::getSingletonPtr()->getScene()->createActor(actorDesc);

			if (nxActor)
			{
				physXActor = OGRE_NEW_T(PhysXActor, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
				physXActor->position = PhysXMath::convert(nxActor->getGlobalPosition());
				physXActor->direction = PhysXMath::convert(nxActor->getLinearVelocity());
				nxActor->setGlobalOrientationQuat(PhysXMath::convert(physicsActorDesc->orientation));
				physXActor->orientation = physicsActorDesc->orientation;
				physXActor->mass = nxActor->getMass();
				physXActor->collisionGroup = nxActor->getGroup();
				physXActor->nxActor = nxActor;
			}
			return physXActor;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::destroyPhysicsActor(PhysicsActor* physicsActor)
		{
			if (!physicsActor)
				return;

			if (!PhysXBridge::getSingletonPtr()->getScene())
				return;

			NxActor* nxActor = (static_cast<PhysXActor*>(physicsActor))->nxActor;
			PhysXBridge::getSingletonPtr()->getScene()->releaseActor(*nxActor);
			OGRE_DELETE_T(physicsActor, PhysicsActor, Ogre::MEMCATEGORY_SCENE_OBJECTS); // Watch the PhysicsActor type instead of PhysXActor!
		}
		//-----------------------------------------------------------------------
		PhysicsActor* PhysXExtern::createPhysicsActorForFluid(Particle* particle)
		{
			if (mPhysXParticlesPointer >= mMaxParticles)
			{
				// Disable emitters, because quota has been reached.
				size_t numEmitters = mParentTechnique->getNumEmitters();
				for (size_t i = 0; i < numEmitters; ++i)
				{
					mParentTechnique->getEmitter(i)->setEnabled(false); 
				}
				return 0;
			}

			PhysXActor* physXActor = OGRE_NEW_T(PhysXActor, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			physXActor->nxParticleData = OGRE_NEW_T(NxParticleData, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			NxU32 start = 1;
			physXActor->nxParticleData->numParticlesPtr = &start;
			physXActor->nxParticleData->bufferPos = &(particle->position).x;
			physXActor->nxParticleData->bufferPosByteStride = sizeof(ParticleStructure);
			physXActor->nxParticleData->bufferVel = &(particle->direction).x;
			physXActor->nxParticleData->bufferVelByteStride = sizeof(ParticleStructure);
			physXActor->nxParticleData->bufferLife = &particle->timeToLive;
			physXActor->nxParticleData->bufferLifeByteStride = sizeof(ParticleStructure);
			physXActor->nxParticleData->bufferFlag = 0;
			physXActor->nxParticleData->bufferFlagByteStride = sizeof(ParticleStructure);
			NxF32 density = NxComputeSphereDensity(mFluid->getKernelRadiusMultiplier(), particle->mass);
			physXActor->nxParticleData->bufferDensity = &density;
			physXActor->nxParticleData->bufferDensityByteStride = sizeof(ParticleStructure);
			particle->physicsActor = physXActor;
			mPhysXParticles[mPhysXParticlesPointer].particle = particle;
			if (mFluid->addParticles(*physXActor->nxParticleData) > 0)
			{	
				mPhysXParticlesPointer++;
			}
			particle->direction = Ogre::Vector3::ZERO; // Reset, because the direction must be recalculated by the fluid
			return particle->physicsActor;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::destroyPhysicsActorForFluid(PhysicsActor* physicsActor)
		{
			if (!physicsActor)
				return;

			// Delete both the physics actor and its nxParticleData
			OGRE_DELETE_T((static_cast<PhysXActor*>(physicsActor))->nxParticleData, NxParticleData, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			OGRE_DELETE_T(physicsActor, PhysicsActor, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_prepare(ParticleTechnique* technique)
		{
			// Register the PhysXExtern to the PhysXBridge
			PhysXBridge::getSingletonPtr()->registerPhysXExtern(this);
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_unprepare(ParticleTechnique* technique)
		{
			// Todo
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::createFluid(void)
		{
			destroyFluid();
			mMaxParticles = static_cast<NxU32>(mParentTechnique->getVisualParticleQuota());
			mPhysXParticles = OGRE_NEW_ARRAY_T(ParticleStructure, mMaxParticles, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			mParticles.numParticlesPtr = &mStart;
			mParticles.bufferPos = &mPhysXParticles[0].position.x;
			mParticles.bufferPosByteStride = sizeof(ParticleStructure);
			mParticles.bufferVel = &mPhysXParticles[0].velocity.x;
			mParticles.bufferVelByteStride = sizeof(ParticleStructure);
			mParticles.bufferLife = &mPhysXParticles[0].lifetime;
			mParticles.bufferLifeByteStride = sizeof(ParticleStructure);
			mParticles.bufferFlag = &mPhysXParticles[0].bufferFlag;
			mParticles.bufferFlagByteStride = sizeof(ParticleStructure);
			mParticles.bufferDensity = &mPhysXParticles[0].density;
			mParticles.bufferDensityByteStride = sizeof(ParticleStructure);
			//mFluidDesc.initialParticleData = mParticles;
			mFluidDesc.particlesWriteData = mParticles;
			mFluidDesc.maxParticles = mMaxParticles;
			if (mFluidDesc.isValid())
			{
				mFluid = PhysXBridge::getSingletonPtr()->getScene()->createFluid(mFluidDesc);
				if (mFluid)
				{
					mFluidSet = true;
				}
			}
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::createFluid(const NxFluidDesc& nxFluidDesc)
		{
			mFluidDesc = nxFluidDesc;
			createFluid();
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::destroyFluid(void)
		{
			if (mFluid)
			{
				// Delete mFluid: The PhysX scene must still exist
				if (PhysXBridge::getSingletonPtr()->getScene())
				{
					PhysXBridge::getSingletonPtr()->getScene()->releaseFluid(*mFluid);
				}
				mFluid = 0;
			}
			
			if (mPhysXParticles)
			{
				// Delete mPhysXParticles
				OGRE_DELETE_ARRAY_T(mPhysXParticles, ParticleStructure, mMaxParticles, Ogre::MEMCATEGORY_SCENE_OBJECTS);
				mPhysXParticles = 0;
			}
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::synchronize(Ogre::Real timeElapsed)
		{
			// Is set by the PhysXBridge as soon as a new simulation has been done.
			mSynchronize = true;
			mTimeElapsedSynchronize = timeElapsed;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_processParticle(
			ParticleTechnique* particleTechnique, 
			Particle* particle, 
			Ogre::Real timeElapsed, 
			bool firstParticle)
		{
			// Only update after a PhysX simulation step and if it isn´t a fluid (fluids are updated in the _postProcessParticles() function)
			if (mSynchronize && !mFluidSet)
			{
				if (particle->particleType != Particle::PT_VISUAL)
					return;

				if (!particle->physicsActor)
					return;

				VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
				PhysXActor* physXActor = static_cast<PhysXActor*>(particle->physicsActor);
				NxActor* nxActor = physXActor->nxActor;
				if (nxActor)
				{
					// Synchronize both the particle and the pysicsActor with the nxActor
					particle->position = PhysXMath::convert(nxActor->getGlobalPosition());
					particle->direction = PhysXMath::convert(nxActor->getLinearVelocity());
					visualParticle->orientation = PhysXMath::convert(nxActor->getGlobalOrientationQuat());
					physXActor->position = particle->position;
					physXActor->direction = particle->direction;
					physXActor->orientation = visualParticle->orientation;

					if (nxActor->getNbShapes())
					{
						NxShape *shape = nxActor->getShapes()[0]; // Max one.
						switch(shape->getType())
						{
							case NX_SHAPE_BOX:
								(static_cast<NxBoxShape*>(shape))->setDimensions(
									PhysXMath::convert(0.5 * Ogre::Vector3(
									visualParticle->width, visualParticle->height, visualParticle->depth)));
							break;
						    case NX_SHAPE_SPHERE:
								(static_cast<NxSphereShape*>(shape))->setRadius(0.5 * visualParticle->width);
							break;
							case NX_SHAPE_CAPSULE:
							{
								(static_cast<NxCapsuleShape*>(shape))->setRadius(0.5 * visualParticle->width);
								(static_cast<NxCapsuleShape*>(shape))->setHeight(0.5 * visualParticle->height);
							}
							break;
						}
					}
				}
			}
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::_postProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed)
		{
			// Only update after a PhysX simulation step and if it is a fluid
			if (mSynchronize && mFluidSet)
			{
				// Run through particle fluid buffer
				NxU32 numberOfParticles = *mParticles.numParticlesPtr;
				if (numberOfParticles > 0)
				{
					// Run through the particles
					for (NxU32 i = 0; i < numberOfParticles; ++i)
					{
						// Run through all non-expired particles
						if (mPhysXParticles[i].particle)
						{
							/** Todo: Particle in mPhysXParticles doesn´t correspond with particles in the fluid. This is a problem when
								particles are different in size and also causes flickering. Don't have a solution (yet), so fluids are 
								for the time being a nice-to-have feature, but limited in use.
							*/
							mPhysXParticles[i].particle->position = PhysXMath::convert(mPhysXParticles[i].position);
							mPhysXParticles[i].particle->direction = PhysXMath::convert(mPhysXParticles[i].velocity);
						}
					}
				}
			}

			// Reset synchronization flag.
			mSynchronize = false;
		}
		//-----------------------------------------------------------------------
		void PhysXExtern::copyAttributesTo (Extern* externObject)
		{
			Extern::copyAttributesTo(externObject);
			PhysXExtern* physXExtern = static_cast<PhysXExtern*>(externObject);
	
			// Copy attributes
			physXExtern->setPhysicsShape(mPhysicsShapeDesc);
			physXExtern->setActorGroup(mActorCollisionGroup);
		}
	}
#endif //PU_PHYSICS_PHYSX