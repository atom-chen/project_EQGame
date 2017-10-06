/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONVELOCITY_OBSERVER_FACTORY_H__
#define __PU_ONVELOCITY_OBSERVER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverFactory.h"
#include "ParticleUniverseOnVelocityObserverTokens.h"
#include "ParticleUniverseOnVelocityObserver.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the OnVelocityObserver.
    */
	class _ParticleUniverseExport OnVelocityObserverFactory : public ParticleObserverFactory
	{
		public:
			OnVelocityObserverFactory(void) {};
	        virtual ~OnVelocityObserverFactory(void) {};

			/** See ParticleObserverFactory */
			virtual Ogre::String getObserverType(void) const
			{
				return "OnVelocity";
			}

			/** See ParticleObserverFactory */
			virtual ParticleObserver* createObserver(void)
			{
				return _createObserver<OnVelocityObserver>();
			}

			/** See ParticleObserverFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mOnVelocityObserverTokens
				mOnVelocityObserverTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleObserverFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mOnVelocityObserverTokens
				mOnVelocityObserverTokens.write(serializer, element);
			}


		protected:
			OnVelocityObserverTokens mOnVelocityObserverTokens;
	};

}
#endif
