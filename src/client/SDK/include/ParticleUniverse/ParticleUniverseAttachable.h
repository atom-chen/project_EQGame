/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ATTACHABLE_H__
#define __PU_ATTACHABLE_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExtern.h"

namespace ParticleUniverse
{
	/** The Attachable is a MovableObject that can be registered at a ParticleSystem as an Extern. By means
		of this construction it is possible to make a dependency between the 2 MovableObjects.
		This can be convenient if the Attachable is a Particle Affector wrapper, that affects a particle
		based on their mutual distance.
	@remarks
		This MovableObject will not be registered at the SceneManager; its purpose is to take advantage of the
		fact that it can be attached to a SceneNode. If an inherited class of Attachable uses visual (rendering)
		elements, it should use the registerAttachable() and unregisterAttachable() functions of the 
		ParticleSystemManager. This implies some additional coding, because this is not supported by the
		scripting capabilities.
    */
	class _ParticleUniverseExport Attachable : public Extern, public Ogre::MovableObject
	{
		public:
			// Constants
			static Ogre::String PU_ATTACHABLE;

			Attachable(void);
	        virtual ~Attachable(void){};

			/** Get the Distance Threshold
			*/
			const Ogre::Real getDistanceThreshold(void) const;

			/** Set the Distance Threshold. This threshold defines at which distance the Attachable doesn't influence 
				the particle anymore.
			*/
			void setDistanceThreshold(const Ogre::Real distanceThreshold);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			virtual void _notifyCurrentCamera(Ogre::Camera* cam);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			const Ogre::String& getMovableType(void) const;

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			const Ogre::AxisAlignedBox& getBoundingBox(void) const;

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			virtual Ogre::Real getBoundingRadius(void) const;
			
			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

			/** @see MovableObject
		    */
			virtual void visitRenderables(Ogre::Renderable::Visitor* visitor,
				bool debugRenderables = false) {/* No implementation */};

			/** Copy attributes to another Extern object.
	        */
			virtual void copyAttributesTo (Extern* externObject);

			/** Perform initialisation actions.
	        */
			virtual void _prepare(ParticleTechnique* technique);

			/** Reverse the actions from the _prepare.
	        */
			virtual void _unprepare(ParticleTechnique* particleTechnique);

			/** Actually processes a particle.
	        */
			virtual void _interface(ParticleTechnique* technique, 
				Particle* particle, 
				Ogre::Real timeElapsed);

		protected:
			Ogre::AxisAlignedBox mAABB;
			Ogre::Real mBoundingRadius;
			Ogre::Real mDistanceThreshold;
			bool mDistanceThresholdSet;
	};

}
#endif
