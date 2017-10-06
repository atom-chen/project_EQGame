/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_JET_AFFECTOR_FACTORY_H__
#define __PU_JET_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseJetAffectorTokens.h"
#include "ParticleUniverseJetAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the JetAffector.
    */
	class _ParticleUniverseExport JetAffectorFactory : public ParticleAffectorFactory
	{
		public:
			JetAffectorFactory(void) {};
	        virtual ~JetAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Jet";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<JetAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mJetAffectorTokens
				mJetAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mJetAffectorTokens
				mJetAffectorTokens.write(serializer, element);
			}

		protected:
			JetAffectorTokens mJetAffectorTokens;
	};

}
#endif
