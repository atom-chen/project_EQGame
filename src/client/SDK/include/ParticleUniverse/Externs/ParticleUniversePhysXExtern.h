/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PHYSX_EXTERN_H__
#define __PU_PHYSX_EXTERN_H__

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX

#include "ParticleUniversePhysicsExtern.h"
#include "NxPhysics.h"
#include "NxActor.h"
#include "NxScene.h"
#include "NxFluid.h"

namespace ParticleUniverse
{
	class _ParticleUniverseExport PhysXActor : public PhysicsActor
	{
		public:
			// Constructor
			PhysXActor(void) : 
				PhysicsActor(),
				nxActor(0),
				nxParticleData(0){};

			// Destructor
			virtual ~PhysXActor(void) {};

			NxActor* nxActor;
			NxParticleData* nxParticleData;
	};
	/////////////////////////////////////////////////////////////////////////////
	class ParticleStructure
	{
		public:
			// Constructor and Destructor
			ParticleStructure(void) : 
				lifetime(0),
				id(0),
				density(0),
				bufferFlag(0),
				particle(0){};
			virtual ~ParticleStructure(void) {};

			// Public attributes
			NxVec3	position;
			NxVec3  velocity;
			NxReal  lifetime;
			NxVec3	collisionNormal;
			NxU32	id;
			NxU32   bufferFlag;
			NxReal	density;
			Particle* particle;
	};
	/////////////////////////////////////////////////////////////////////////////
	/** The PhysXExtern class is responsible for connecting a physical object to a particle that was emitted by Particle Universe.
		This can be either by means of regular actors (one particle relates to one actor with one shape) or a fluid is used, 
		in which case the Particle Universe particles are 'connected' to a fluid particle. As soon as createFluid() is called, fluid
		particles are used.
	*/
	class _ParticleUniverseExport PhysXExtern : public PhysicsExtern
	{
		public:
			PhysXExtern(void);
	        virtual ~PhysXExtern(void);

			/** See PhysicsExtern
			*/
			virtual void _initParticleForEmission(Particle* particle);

			/** See PhysicsExtern
			*/
			virtual void _initParticleForExpiration(Particle* particle);

			/** See PhysicsExtern
			*/
			virtual PhysicsActor* createPhysicsActor(PhysicsActorDesc* physicsActorDesc, PhysicsShapeDesc* physicsShapeDesc);

			/** This is a specialized create function for fluids
			*/
			PhysicsActor* createPhysicsActorForFluid(Particle* particle);

			/** See PhysicsExtern
			*/
			virtual void destroyPhysicsActor(PhysicsActor* physicsActor);

			/** This is a specialized destroy function for fluids
			*/
			void destroyPhysicsActorForFluid(PhysicsActor* physicsActor);

			/** See Extern
	        */
			virtual void _prepare(ParticleTechnique* technique);

			/** See Extern
	        */
			virtual void _unprepare(ParticleTechnique* technique);

			/** See Extern: Override the implementation of Extern
	        */
			virtual void _processParticle(
				ParticleTechnique* particleTechnique, 
				Particle* particle, 
				Ogre::Real timeElapsed, 
				bool firstParticle);

			/** See Extern: Ignore
	        */
			virtual void _interface(ParticleTechnique* technique, 
				Particle* particle, 
				Ogre::Real timeElapsed){};

			/** See Extern
	        */
			virtual void _postProcessParticles(ParticleTechnique* technique, Ogre::Real timeElapsed);

			/** See Extern.
	        */
			virtual void _notifyStop (void);

			/** Create a fluid using the current fluid descriptor.
	        */
			void createFluid(void);

			/** Create a fluid using the fluid descriptor in the argument.
	        */
			void createFluid(const NxFluidDesc& nxFluidDesc);

			/** Delete a fluid.
	        */
			void destroyFluid(void);

			/** This function is called by the PhysXBridge. Every time it is called, the physics are updated, which means that the 
				particles and physics actors must also be synchronized.
	        */
			void synchronize(Ogre::Real timeElapsed);

			/** 
	        */
			virtual void copyAttributesTo (Extern* externObject);

			/** Set the physics shape.
	        */
			void setPhysicsShape(PhysicsShapeDesc* physicsShapeDesc);

			/** Get the actorgroup.
	        */
			Ogre::uint16 getActorGroup(void) const;

			/** Set the actorgroup.
	        */
			void setActorGroup(Ogre::uint16 actorCollisionGroup);

			/** Public attributes.
	        */
			PhysicsShapeDesc* mPhysicsShapeDesc; // Defines the physical shape of the particles; only one shape is allowed.

		protected:

			NxScene* mScene;
			bool mSynchronize; // Flag that is set by the PhysXBridge if the scene is simulated.
			Ogre::Real mTimeElapsedSynchronize;
			Ogre::uint16 mActorCollisionGroup;
			NxFluid* mFluid;
			bool mFluidSet;
			NxFluidDesc mFluidDesc;
			NxU32 mMaxParticles;
			ParticleStructure* mPhysXParticles;
			NxU32 mPhysXParticlesPointer;
			NxU32 mStart;
			NxParticleData mParticles;
	};

}
#endif //PU_PHYSICS_PHYSX
#endif
