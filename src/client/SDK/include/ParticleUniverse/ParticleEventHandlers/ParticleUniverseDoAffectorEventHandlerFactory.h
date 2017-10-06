/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_AFFECTOR_EVENT_HANDLER_FACTORY_H__
#define __PU_DO_AFFECTOR_EVENT_HANDLER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEventHandlerFactory.h"
#include "ParticleUniverseDoAffectorEventHandlerTokens.h"
#include "ParticleUniverseDoAffectorEventHandler.h"

namespace ParticleUniverse
{
	/** This factory class is responsible for creation of a DoAffectorEventHandler.
    */
	class _ParticleUniverseExport DoAffectorEventHandlerFactory : public ParticleEventHandlerFactory
	{
		public:
			DoAffectorEventHandlerFactory(void) {};
	        virtual ~DoAffectorEventHandlerFactory(void) {};

			/** See ParticleEventHandlerFactory */
			virtual Ogre::String getEventHandlerType(void) const
			{
				return "DoAffector";
			}

			/** See ParticleEventHandlerFactory */
			virtual ParticleEventHandler* createEventHandler(void)
			{
				return _createEventHandler<DoAffectorEventHandler>();
			}

			/** See ParticleEventHandlerFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mDoAffectorEventHandlerTokens
				mDoAffectorEventHandlerTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleEventHandlerFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mDoAffectorEventHandlerTokens
				mDoAffectorEventHandlerTokens.write(serializer, element);
			}

		protected:
			DoAffectorEventHandlerTokens mDoAffectorEventHandlerTokens;
	};

}
#endif
