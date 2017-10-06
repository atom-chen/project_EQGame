/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LINEAR_FORCE_AFFECTOR_FACTORY_H__
#define __PU_LINEAR_FORCE_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseForceAffectorTokens.h"
#include "ParticleUniverseLinearForceAffector.h"
#include "ParticleUniverseLinearForceAffectorTokens.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the LinearForceAffector.
    */
	class _ParticleUniverseExport LinearForceAffectorFactory : public ParticleAffectorFactory
	{
		public:
			LinearForceAffectorFactory(void) {};
	        virtual ~LinearForceAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "LinearForce";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<LinearForceAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister){}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mLinearForceAffectorTokens
				mLinearForceAffectorTokens.write(serializer, element);
			}

		protected:
			LinearForceAffectorTokens mLinearForceAffectorTokens;
	};

}
#endif
