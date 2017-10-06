/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VORTEX_AFFECTOR_FACTORY_H__
#define __PU_VORTEX_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseVortexAffectorTokens.h"
#include "ParticleUniverseVortexAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the VortexAffector.
    */
	class _ParticleUniverseExport VortexAffectorFactory : public ParticleAffectorFactory
	{
		public:
			VortexAffectorFactory(void){};
	        virtual ~VortexAffectorFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Vortex";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<VortexAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mVortexAffectorTokens
				mVortexAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mVelocityMatchingAffectorTokens
				mVortexAffectorTokens.write(serializer, element);
			}

		protected:
			VortexAffectorTokens mVortexAffectorTokens;
	};

}
#endif
