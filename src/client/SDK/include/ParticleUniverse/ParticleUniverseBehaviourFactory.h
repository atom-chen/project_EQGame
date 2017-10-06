/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BEHAVIOUR_FACTORY_H__
#define __PU_BEHAVIOUR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseBehaviour.h"
#include "ParticleUniverseBehaviourTokens.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This is the base factory of all ParticleBehaviour implementations.
    */
	class _ParticleUniverseExport ParticleBehaviourFactory : public ITokenBind, public Ogre::FactoryAlloc
	{
	    protected:
			/** 
	        */
			template <class T>
			ParticleBehaviour* _createBehaviour(void)
			{
				ParticleBehaviour* particleBehaviour = OGRE_NEW_T(T, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
				particleBehaviour->setBehaviourType(getBehaviourType());
				return particleBehaviour;
			};

	public:
			ParticleBehaviourFactory(void) {};
			virtual ~ParticleBehaviourFactory(void){};

		    /** Returns the type of the factory, which identifies the particle behaviour type this factory creates. */
			virtual Ogre::String getBehaviourType() const = 0;

			/** Creates a new behaviour instance.
		    @remarks
	        */
		    virtual ParticleBehaviour* createBehaviour(void) = 0;

			/** Delete a behaviour
	        */
			void destroyBehaviour (ParticleBehaviour* behaviour)
			{
				OGRE_DELETE_T(behaviour, ParticleBehaviour, Ogre::MEMCATEGORY_SCENE_OBJECTS);
			};
	};

}
#endif
