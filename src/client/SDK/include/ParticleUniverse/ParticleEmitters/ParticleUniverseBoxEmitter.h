/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BOX_EMITTER_H__
#define __PU_BOX_EMITTER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseEmitter.h"

namespace ParticleUniverse
{
	/** The BoxEmitter is a ParticleEmitter that emits particles within a box shape.
    */
	class _ParticleUniverseExport BoxEmitter : public ParticleEmitter
	{
		protected:
			Ogre::Real mHeight;
			Ogre::Real mWidth;
			Ogre::Real mDepth;

			Ogre::Real mXRange;
			Ogre::Real mYRange;
			Ogre::Real mZRange;

		public:
			// Constants
			static const Ogre::Real DEFAULT_WIDTH;
			static const Ogre::Real DEFAULT_HEIGHT;
			static const Ogre::Real DEFAULT_DEPTH;

			BoxEmitter(void);
	        virtual ~BoxEmitter(void) {};

			/** 
	        */
			const Ogre::Real getHeight(void) const;
			void setHeight(const Ogre::Real height);

			/** 
	        */
			const Ogre::Real getWidth(void) const;
			void setWidth(const Ogre::Real width);

			/** 
	        */
			const Ogre::Real getDepth(void) const;
			void setDepth(const Ogre::Real depth);

			/** 
	        */
			virtual void _initParticlePosition(Particle* particle);

			/** 
	        */
			virtual void copyAttributesTo (ParticleEmitter* emitter);
	};

}
#endif
