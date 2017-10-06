/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RANDOMISER_FACTORY_H__
#define __PU_RANDOMISER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRandomiserTokens.h"
#include "ParticleUniverseRandomiser.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the Randomiser.
    */
	class _ParticleUniverseExport RandomiserFactory : public ParticleAffectorFactory
	{
		public:
			RandomiserFactory(void) {};
	        virtual ~RandomiserFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Randomiser";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<Randomiser>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mRandomiserTokens
				mRandomiserTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mRandomiserTokens
				mRandomiserTokens.write(serializer, element);
			}

		protected:
			RandomiserTokens mRandomiserTokens;
	};

}
#endif
