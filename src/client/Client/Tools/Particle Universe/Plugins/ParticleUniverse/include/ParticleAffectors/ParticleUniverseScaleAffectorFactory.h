/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SCALE_AFFECTOR_FACTORY_H__
#define __PU_SCALE_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseScaleAffectorTokens.h"
#include "ParticleUniverseScaleAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the ScaleAffector.
    */
	class _ParticleUniverseExport ScaleAffectorFactory : public ParticleAffectorFactory
	{
		public:
			ScaleAffectorFactory(void) {};
	        virtual ~ScaleAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Scale";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<ScaleAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mScaleAffectorTokens
				mScaleAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mScaleAffectorTokens
				mScaleAffectorTokens.write(serializer, element);
			}

		protected:
			ScaleAffectorTokens mScaleAffectorTokens;
	};

}
#endif
