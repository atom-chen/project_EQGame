/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_POINT_EMITTER_FACTORY_H__
#define __PU_POINT_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitterFactory.h"
#include "ParticleUniversePointEmitter.h"
#include "ParticleUniversePointEmitterTokens.h"

namespace ParticleUniverse
{
	/** This is the factory class that is responsible for creating a PointEmitter.
    */
	class _ParticleUniverseExport PointEmitterFactory : public ParticleEmitterFactory
	{
		protected:
			PointEmitterTokens mPointEmitterTokens;

		public:
			PointEmitterFactory(void) {};
	        virtual ~PointEmitterFactory(void) {};

			/** See ParticleEmitterFactory */
			Ogre::String getEmitterType() const
			{
				return "Point";
			}

			/** See ParticleEmitterFactory */
			ParticleEmitter* createEmitter(void)
			{
				return _createEmitter<PointEmitter>();
			}

			/** See ParticleEmitterFactory
			@remarks
				The Point emitter doesn't have other attributes than the base attributes */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister){};

			/** See ParticleEmitterFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				mPointEmitterTokens.write(serializer, element);
			}
	};

}
#endif
