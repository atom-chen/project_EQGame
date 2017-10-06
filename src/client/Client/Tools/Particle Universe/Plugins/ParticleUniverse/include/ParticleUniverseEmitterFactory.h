/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_EMITTER_FACTORY_H__
#define __PU_EMITTER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseEmitter.h"
#include "ParticleUniverseEmitterTokens.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This is the base factory of all ParticleEmitter implementations.
    */
	class _ParticleUniverseExport ParticleEmitterFactory : public ITokenBind, public Ogre::FactoryAlloc
	{
	    protected:

			/** 
	        */
			template <class T>
			ParticleEmitter* _createEmitter(void)
			{
				ParticleEmitter* particleEmitter = OGRE_NEW_T(T, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
				particleEmitter->setEmitterType(getEmitterType());
				return particleEmitter;
			};

	public:
			ParticleEmitterFactory(void) {};
	        virtual ~ParticleEmitterFactory(void) {};

			/** Returns the type of the factory, which identifies the particle emitter type this factory creates. */
			virtual Ogre::String getEmitterType() const = 0;

	        /** Creates a new emitter instance.
		    @remarks
	        */
		    virtual ParticleEmitter* createEmitter(void) = 0;

			/** Delete an emitter 
	        */
			void destroyEmitter (ParticleEmitter* emitter)
			{
				OGRE_DELETE_T(emitter, ParticleEmitter, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			};

	};

}
#endif
