/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONEMISSION_OBSERVER_FACTORY_H__
#define __PU_ONEMISSION_OBSERVER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverFactory.h"
#include "ParticleUniverseOnEmissionObserver.h"
#include "ParticleUniverseOnEmissionObserverTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the OnEmissionObserver.
    */
	class _ParticleUniverseExport OnEmissionObserverFactory : public ParticleObserverFactory
	{
		public:
			OnEmissionObserverFactory(void) {};
	        virtual ~OnEmissionObserverFactory(void) {};

			/** See ParticleObserverFactory */
			virtual Ogre::String getObserverType(void) const
			{
				return "OnEmission";
			}

			/** See ParticleObserverFactory */
			virtual ParticleObserver* createObserver(void)
			{
				return _createObserver<OnEmissionObserver>();
			}

			/** See ParticleObserverFactory
			@remarks
				This observer doesn't have attributes */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister){}

			/** See ParticleObserverFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				mOnEmissionObserverTokens.write(serializer, element);
			}

		protected:
			OnEmissionObserverTokens mOnEmissionObserverTokens;
	};

}
#endif
