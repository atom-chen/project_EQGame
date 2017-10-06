/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_EXPIRE_EVENT_HANDLER_FACTORY_H__
#define __PU_DO_EXPIRE_EVENT_HANDLER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEventHandlerFactory.h"
#include "ParticleUniverseDoExpireEventHandler.h"
#include "ParticleUniverseDoExpireEventHandlerTokens.h"

namespace ParticleUniverse
{
	/** This factory class is responsible for creation of a DoExpireEventHandler.
    */
	class _ParticleUniverseExport DoExpireEventHandlerFactory : public ParticleEventHandlerFactory
	{
		public:
			DoExpireEventHandlerFactory(void) {};
	        virtual ~DoExpireEventHandlerFactory(void) {};

			/** See ParticleEventHandlerFactory*/
			virtual Ogre::String getEventHandlerType(void) const
			{
				return "DoExpire";
			}

			/** See ParticleEventHandlerFactory*/
			virtual ParticleEventHandler* createEventHandler(void)
			{
				return _createEventHandler<DoExpireEventHandler>();
			}

			/** See ParticleEventHandlerFactory
			@remarks
				This event handler doesn't have attributes */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {};

			/** See ParticleEventHandlerFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				mDoExpireEventHandlerTokens.write(serializer, element);
			}

		protected:
			DoExpireEventHandlerTokens mDoExpireEventHandlerTokens;
	};

}
#endif
