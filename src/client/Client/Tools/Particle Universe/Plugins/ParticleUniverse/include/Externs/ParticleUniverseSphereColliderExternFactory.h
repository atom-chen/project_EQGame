/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SPHERE_COLLIDER_EXTERN_FACTORY_H__
#define __PU_SPHERE_COLLIDER_EXTERN_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniverseSphereColliderExtern.h"
#include "ParticleUniverseSphereColliderExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the SphereColliderExtern.
    */
	class _ParticleUniverseExport SphereColliderExternFactory : public ExternFactory
	{
		public:
			SphereColliderExternFactory(void) : 
				ExternFactory(){};
	        virtual ~SphereColliderExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "SphereCollider";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<SphereColliderExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {}

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mSphereColliderExternTokens
				mSphereColliderExternTokens.write(serializer, element);
			}

		protected:
			SphereColliderExternTokens mSphereColliderExternTokens;
	};

}
#endif
