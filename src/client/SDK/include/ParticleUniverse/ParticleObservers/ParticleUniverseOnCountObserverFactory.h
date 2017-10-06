/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONCOUNT_OBSERVER_FACTORY_H__
#define __PU_ONCOUNT_OBSERVER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverFactory.h"
#include "ParticleUniverseOnCountObserverTokens.h"
#include "ParticleUniverseOnCountObserver.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the OnCountObserver.
    */
	class _ParticleUniverseExport OnCountObserverFactory : public ParticleObserverFactory
	{
		public:
			OnCountObserverFactory(void) {};
	        virtual ~OnCountObserverFactory(void) {};

			/** See ParticleObserverFactory */
			virtual Ogre::String getObserverType(void) const
			{
				return "OnCount";
			}

			/** See ParticleObserverFactory */
			virtual ParticleObserver* createObserver(void)
			{
				return _createObserver<OnCountObserver>();
			}

			/** See ParticleObserverFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mOnCountObserverTokens
				mOnCountObserverTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleObserverFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mOnCountObserverTokens
				mOnCountObserverTokens.write(serializer, element);
			}

		protected:
			OnCountObserverTokens mOnCountObserverTokens;
	};

}
#endif
