/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PHYSX_EXTERN_FACTORY_H__
#define __PU_PHYSX_EXTERN_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniversePhysXExtern.h"
#include "ParticleUniversePhysXExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the PhysXExtern.
    */
	class _ParticleUniverseExport PhysXExternFactory : public ExternFactory
	{
		public:
			PhysXExternFactory(void) : 
				ExternFactory(){};
	        virtual ~PhysXExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "PhysX";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<PhysXExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mPhysXExternTokens
				mPhysXExternTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mPhysXExternTokens
				mPhysXExternTokens.write(serializer, element);
			}


		protected:
			PhysXExternTokens mPhysXExternTokens;
	};

}
#endif //PU_PHYSICS_PHYSX
#endif
