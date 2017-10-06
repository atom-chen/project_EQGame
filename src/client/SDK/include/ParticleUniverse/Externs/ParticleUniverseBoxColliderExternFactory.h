/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BOX_COLLIDER_EXTERN_FACTORY_H__
#define __PU_BOX_COLLIDER_EXTERN_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniverseBoxColliderExtern.h"
#include "ParticleUniverseBoxColliderExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the BoxColliderExtern.
    */
	class _ParticleUniverseExport BoxColliderExternFactory : public ExternFactory
	{
		public:
			BoxColliderExternFactory(void) : 
				ExternFactory(){};
	        virtual ~BoxColliderExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "BoxCollider";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<BoxColliderExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {}

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mBoxColliderExternTokens
				mBoxColliderExternTokens.write(serializer, element);
			}

		protected:
			BoxColliderExternTokens mBoxColliderExternTokens;
	};

}
#endif
