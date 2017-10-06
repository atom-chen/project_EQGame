/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONPOSITION_OBSERVER_FACTORY_H__
#define __PU_ONPOSITION_OBSERVER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverFactory.h"
#include "ParticleUniverseOnPositionObserver.h"
#include "ParticleUniverseOnPositionObserverTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the OnPositionObserver.
    */
	class _ParticleUniverseExport OnPositionObserverFactory : public ParticleObserverFactory
	{
		public:
			OnPositionObserverFactory(void) {};
	        virtual ~OnPositionObserverFactory(void) {};

			/** See ParticleObserverFactory */
			virtual Ogre::String getObserverType(void) const
			{
				return "OnPosition";
			}

			/** See ParticleObserverFactory */
			virtual ParticleObserver* createObserver(void)
			{
				return _createObserver<OnPositionObserver>();
			}

			/** See ParticleObserverFactory
			@remarks
				This observer doesn't have attributes */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mOnPositionObserverTokens
				mOnPositionObserverTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleObserverFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mOnPositionObserverTokens
				mOnPositionObserverTokens.write(serializer, element);
			}

		protected:
			OnPositionObserverTokens mOnPositionObserverTokens;
	};

}
#endif
