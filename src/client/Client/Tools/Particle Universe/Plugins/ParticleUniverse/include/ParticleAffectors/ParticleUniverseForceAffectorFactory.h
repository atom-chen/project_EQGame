/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_FORCE_AFFECTOR_FACTORY_H__
#define __PU_FORCE_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffectorTokens.h"
#include "ParticleUniverseForceAffector.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** This factory is just a dummy factory and only used to register generic ForceAffector tokens.
    */
	class _ParticleUniverseExport ForceAffectorFactory : public ParticleAffectorFactory
	{
		protected:
			ForceAffectorTokens mForceAffectorTokens;

		public:
			ForceAffectorFactory(void) {};
	        virtual ~ForceAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType(void) const
			{
				return "Dummy01"; // Dummy Factory, only for setting up token definitions.
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, "PU: ForceAffectorFactory is a dummy factory.", 
					"ForceAffectorFactory::createAffector");
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mForceAffectorTokens
				mForceAffectorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mForceAffectorTokens
				mForceAffectorTokens.write(serializer, element);
			}
	};

}
#endif
