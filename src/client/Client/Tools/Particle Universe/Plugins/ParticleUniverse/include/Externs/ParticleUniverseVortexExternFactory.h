/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VORTEX_EXTERN_FACTORY_H__
#define __PU_VORTEX_EXTERN_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniverseVortexExtern.h"
#include "ParticleUniverseVortexExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the VortexExtern.
    */
	class _ParticleUniverseExport VortexExternFactory : public ExternFactory
	{
		public:
			VortexExternFactory(void) : 
				ExternFactory(){};
	        virtual ~VortexExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "Vortex";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<VortexExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister){}

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mVortexExternTokens
				mVortexExternTokens.write(serializer, element);
			}

		protected:
			VortexExternTokens mVortexExternTokens;
	};

}
#endif
