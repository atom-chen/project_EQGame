/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PATH_FOLLOWER_FACTORY_H__
#define __PU_PATH_FOLLOWER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniversePathFollowerTokens.h"
#include "ParticleUniversePathFollower.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the PathFollower.
    */
	class _ParticleUniverseExport PathFollowerFactory : public ParticleAffectorFactory
	{
		public:
			PathFollowerFactory(void) {};
	        virtual ~PathFollowerFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "PathFollower";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<PathFollower>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mPathFollowerTokens
				mPathFollowerTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mPathFollowerTokens
				mPathFollowerTokens.write(serializer, element);
			}

		protected:
			PathFollowerTokens mPathFollowerTokens;
	};

}
#endif
