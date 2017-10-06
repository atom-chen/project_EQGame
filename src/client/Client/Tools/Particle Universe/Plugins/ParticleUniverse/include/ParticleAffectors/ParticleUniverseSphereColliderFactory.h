/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SPHERE_COLLIDER_FACTORY_H__
#define __PU_SPHERE_COLLIDER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSphereColliderTokens.h"
#include "ParticleUniverseSphereCollider.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the SphereCollider.
    */
	class _ParticleUniverseExport SphereColliderFactory : public ParticleAffectorFactory
	{
		public:
			SphereColliderFactory(void){};
	        virtual ~SphereColliderFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "SphereCollider";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<SphereCollider>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mSphereColliderTokens
				mSphereColliderTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mSphereColliderTokens
				mSphereColliderTokens.write(serializer, element);
			}

		protected:
			SphereColliderTokens mSphereColliderTokens;
	};

}
#endif
