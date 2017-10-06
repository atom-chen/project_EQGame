/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SINE_FORCE_AFFECTOR_FACTORY_H__
#define __PU_SINE_FORCE_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSineForceAffectorTokens.h"
#include "ParticleUniverseSineForceAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the SineForceAffector.
    */
	class _ParticleUniverseExport SineForceAffectorFactory : public ParticleAffectorFactory
	{
		public:
			SineForceAffectorFactory(void) {};
	        virtual ~SineForceAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "SineForce";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<SineForceAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{	
				// Delegate to mSineForceAffectorTokens
				mSineForceAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mSineForceAffectorTokens
				mSineForceAffectorTokens.write(serializer, element);
			}

		protected:
			SineForceAffectorTokens mSineForceAffectorTokens;
	};

}
#endif
