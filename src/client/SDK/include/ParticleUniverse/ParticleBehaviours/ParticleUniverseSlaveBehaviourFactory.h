/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SLAVE_BEHAVIOUR_FACTORY_H__
#define __PU_SLAVE_BEHAVIOUR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseBehaviourFactory.h"
#include "ParticleUniverseSlaveBehaviour.h"
#include "ParticleUniverseSlaveBehaviourTokens.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a SlaveBehaviour.
    */
	class _ParticleUniverseExport SlaveBehaviourFactory : public ParticleBehaviourFactory
	{
		public:
			SlaveBehaviourFactory(void) {};
	        virtual ~SlaveBehaviourFactory(void) {};

			/** See ParticleBehaviourFactory */
			Ogre::String getBehaviourType() const
			{
				return "Slave";
			}

			/** See ParticleBehaviourFactory */
			ParticleBehaviour* createBehaviour(void)
			{
				return _createBehaviour<SlaveBehaviour>();
			}

			/** See ParticleBehaviourFactory
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
			}

			/** See ParticleBehaviourFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mSlaveBehaviourTokens
				mSlaveBehaviourTokens.write(serializer, element);
			}

		protected:
			SlaveBehaviourTokens mSlaveBehaviourTokens;
	};

}
#endif
