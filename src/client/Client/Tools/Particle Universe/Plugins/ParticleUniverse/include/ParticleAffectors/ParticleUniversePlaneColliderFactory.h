/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PLANE_COLLIDER_FACTORY_H__
#define __PU_PLANE_COLLIDER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniversePlaneColliderTokens.h"
#include "ParticleUniversePlaneCollider.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the PlaneCollider.
    */
	class _ParticleUniverseExport PlaneColliderFactory : public ParticleAffectorFactory
	{
		public:
			PlaneColliderFactory(void){};
	        virtual ~PlaneColliderFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "PlaneCollider";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<PlaneCollider>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mPlaneColliderTokens
				mPlaneColliderTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mPlaneColliderTokens
				mPlaneColliderTokens.write(serializer, element);
			}

		protected:
			PlaneColliderTokens mPlaneColliderTokens;
	};

}
#endif
