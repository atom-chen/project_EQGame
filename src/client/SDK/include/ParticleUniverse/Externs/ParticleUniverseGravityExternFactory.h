/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_GRAVITY_EXTERN_FACTORY_H__
#define __PU_GRAVITY_EXTERN_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternFactory.h"
#include "ParticleUniverseGravityExtern.h"
#include "ParticleUniverseGravityExternTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the GravityExtern.
    */
	class _ParticleUniverseExport GravityExternFactory : public ExternFactory
	{
		public:
			GravityExternFactory(void) : 
				ExternFactory(){};
	        virtual ~GravityExternFactory(void){};

			/** 
			*/
			Ogre::String getExternType() const
			{
				return "Gravity";
			};

			/** 
			*/
			Extern* createExtern(void)
			{
				return _createExtern<GravityExtern>();
			};

			/** See ParticleExternFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {}

			/** See ParticleExternFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mGravityExternTokens
				mGravityExternTokens.write(serializer, element);
			}


		protected:
			GravityExternTokens mGravityExternTokens;
	};

}
#endif
