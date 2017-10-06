/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_GRAVITY_AFFECTOR_FACTORY_H__
#define __PU_GRAVITY_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseGravityAffectorTokens.h"
#include "ParticleUniverseGravityAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the GravityAffector.
    */
	class _ParticleUniverseExport GravityAffectorFactory : public ParticleAffectorFactory
	{
		public:
			GravityAffectorFactory(void){};
	        virtual ~GravityAffectorFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Gravity";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<GravityAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mGravityAffectorTokens
				mGravityAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mGravityAffectorTokens
				mGravityAffectorTokens.write(serializer, element);
			}

		protected:
			GravityAffectorTokens mGravityAffectorTokens;
	};

}
#endif
