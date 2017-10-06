/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONCOLLISON_OBSERVER_FACTORY_H__
#define __PU_ONCOLLISON_OBSERVER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverFactory.h"
#include "ParticleUniverseOnCollisionObserver.h"
#include "ParticleUniverseOnCollisionObserverTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the OnCollisionObserver.
    */
	class _ParticleUniverseExport OnCollisionObserverFactory : public ParticleObserverFactory
	{
		public:
			OnCollisionObserverFactory(void) {};
	        virtual ~OnCollisionObserverFactory(void) {};

			/** See ParticleObserverFactory */
			virtual Ogre::String getObserverType(void) const
			{
				return "OnCollision";
			}

			/** See ParticleObserverFactory */
			virtual ParticleObserver* createObserver(void)
			{
				return _createObserver<OnCollisionObserver>();
			}

			/** See ParticleObserverFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {}

			/** See ParticleObserverFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				mOnCollisionObserverTokens.write(serializer, element);
			}

			protected:
				OnCollisionObserverTokens mOnCollisionObserverTokens;
	};

}
#endif
