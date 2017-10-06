/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PARTICLE_POOL_H__
#define __PU_PARTICLE_POOL_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniversePool.h"
#include "ParticleUniversePoolMap.h"
#include "ParticleUniverseVisualParticle.h"

namespace ParticleUniverse
{
	/** The ParticlePool is a container class that includes other pools. The ParticlePool acts as one pool
		with different types of particles.
    */
	class _ParticleUniverseExport ParticlePool
	{
		protected:
			ParticleTechnique* mParentTechnique;

			/** The Pools
		    */
			Pool<VisualParticle> mVisualParticlesPool;
			PoolMap<ParticleEmitter> mParticleEmitterPool;
			PoolMap<ParticleAffector> mParticleAffectorPool;
			PoolMap<ParticleSystem> mParticleSystemPool;
			PoolMap<ParticleTechnique> mParticleTechniquePool;

			/** Vectors used for deletion of created objects
		    */
			std::vector<VisualParticle*> mVisualParticles;
			std::vector<ParticleEmitter*> mEmitters;
			std::vector<ParticleAffector*> mAffectors;
			std::vector<ParticleTechnique*> mTechniques;
			std::vector<Ogre::String> mSystems; // Use names instead of pointers to avoid problems during cleanup.

			/** Protected methods to increase the pools
		    */
			void _increaseVisualParticlePool(size_t size, std::vector<ParticleBehaviour*>& behaviours);
			void _increaseParticleEmitterPool(size_t size, std::vector<ParticleBehaviour*>& behaviours, ParticleTechnique* technique);
			void _increaseParticleAffectorPool(size_t size, std::vector<ParticleBehaviour*>& behaviours, ParticleTechnique* technique);
			void _increaseParticleTechniquePool(size_t size, std::vector<ParticleBehaviour*>& behaviours, ParticleSystem* system);
			void _increaseParticleSystemPool(size_t size, std::vector<ParticleBehaviour*>& behaviours, ParticleTechnique* technique);

			Particle* mLatestParticle;

		public:

			ParticlePool(void) : mLatestParticle(0){};
			virtual ~ParticlePool (void);

			/** 
			*/
			void setParentTechnique(ParticleTechnique* parentTechnique);

			/** 
			*/
			inline bool isEmpty(void);

			/** 
			*/
			inline bool isEmpty(const Particle::ParticleType particleType);

			/** 
			*/
			inline size_t getSize(void);

			/** 
			*/
			inline size_t getSize(const Particle::ParticleType particleType);

			/** 
		    */
			void initialisePool(void);

			/** 
		    */
			void increasePool (const Particle::ParticleType particleType, 
				size_t size,
				std::vector<ParticleBehaviour*>& behaviours,
				ParticleTechnique* technique);

			/** Destroy particles of a certain type
		    */
			void destroyParticles(const Particle::ParticleType particleType);
			void destroyAllVisualParticles(void);
			void destroyAllEmitterParticles(void);
			void destroyAllTechniqueParticles(void);
			void destroyAllAffectorParticles(void);
			void destroyAllSystemParticles(void);

			/** Releases a particle from the pool
		    */
			inline Particle* releaseParticle (const Particle::ParticleType particleType, const Ogre::String& name);

			/** Releases all particles from the pool
		    */
			inline void releaseAllParticles (void);

			/** 
		    */
			inline void lockLatestParticle (void);

			/** Lock all particles in the pool
		    */
			inline void lockAllParticles (void);

			/** 
		    */
			inline void resetIterator(void);

			/** 
		    */
			inline Particle* getFirst(void);

			/** 
		    */
			inline Particle* getNext(void);

			/** 
		    */
			inline Particle* getFirst(const Particle::ParticleType particleType);

			/** 
		    */
			inline Particle* getNext(const Particle::ParticleType particleType);

			/** 
		    */
			inline bool end(void);

			/** 
		    */
			inline bool end(const Particle::ParticleType particleType);

			/** 
		    */
			std::list<VisualParticle*>& getVisualParticlesList(void)
			{
				return mVisualParticlesPool.getActiveElementsList();
			};
	};

}
#endif
