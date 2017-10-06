/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_COLLISION_AVOIDANCE_AFFECTOR_FACTORY_H__
#define __PU_COLLISION_AVOIDANCE_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorFactory.h"
#include "ParticleUniverseCollisionAvoidanceAffector.h"
#include "ParticleUniverseCollisionAvoidanceAffectorTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the CollisionAvoidanceAffector.
    */
	class _ParticleUniverseExport CollisionAvoidanceAffectorFactory : public ParticleAffectorFactory
	{
		public:
			CollisionAvoidanceAffectorFactory(void) {};
	        virtual ~CollisionAvoidanceAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "CollisionAvoidance";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<CollisionAvoidanceAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mCollisionAvoidanceAffectorTokens
				mCollisionAvoidanceAffectorTokens.setupTokenDefinitions(tokenRegister);
			};

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mCollisionAvoidanceAffectorTokens
				mCollisionAvoidanceAffectorTokens.write(serializer, element);
			}

		protected:
			CollisionAvoidanceAffectorTokens mCollisionAvoidanceAffectorTokens;
	};

}
#endif
