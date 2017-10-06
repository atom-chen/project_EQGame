/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_VISUAL_PARTICLE_H__
#define __PU_VISUAL_PARTICLE_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseParticle.h"

namespace ParticleUniverse
{
	/** A VisualParticle is the most obvious implementation of a particle. It represents that particles that can be
		visualised on the screen.
    */
	class _ParticleUniverseExport VisualParticle : public Particle
	{
		public:
			VisualParticle(void);
			virtual ~VisualParticle(void) {};

			/** Current and original colour */
			Ogre::ColourValue colour;
			Ogre::ColourValue originalColour;

			/** zRotation is used to rotate the particle in 2D (around the Z-axis)
			@remarks
				There is no relation between zRotation and orientation.
				rotationSpeed in combination with orientation are used for 3D rotation of the particle, while
				zRotation means the rotation around the Z-axis. This type of rotation is typically used for 
				rotating textures. This also means that both types of rotation can be used together.
			*/
			Ogre::Radian zRotation;

			/** The zRotationSpeed is used in combination with zRotation and defines tha actual rotationspeed
				in 2D. */
			Ogre::Radian zRotationSpeed;

			/*  Orientation of the particle.
			@remarks
				The orientation of the particle is only visible if the Particle Renderer - such as the Box renderer - 
				supports orientation.
			*/
			Ogre::Quaternion orientation;
			Ogre::Quaternion originalOrientation;

			/** The rotation is used in combination with orientation. Because the rotation speed is part
				of the particle itself, it can be changed independently. */
			Ogre::Real rotationSpeed;

			/** Does this particle have it's own dimensions? */
			bool ownDimensions;

			/** Own width
			*/
			Ogre::Real width;
        
			/** Own height
			*/
			Ogre::Real height;

			/** Own depth
			*/
			Ogre::Real depth;

			/** Radius of the particle, to be used for inter-particle collision and such.
			*/
			Ogre::Real radius;

			/** Animation attributes
			*/
			Ogre::Real textureAnimationTimeStep;
			Ogre::Real textureAnimationTimeStepCount;
			Ogre::uint16 textureCoordsCurrent;
			bool textureAnimationDirectionUp;

			/** Set own dimensions
			*/
			inline void setOwnDimensions(Ogre::Real newWidth, Ogre::Real newHeight, Ogre::Real newDepth);

			/** @see Particle::_initForEmission()
			*/
			inline virtual void _initForEmission(void);

			/** @see Particle::_initForExpiration()
			*/
			inline virtual void _initForExpiration(ParticleTechnique* technique, Ogre::Real timeElapsed);

			/** Calculate the bounding sphere radius
			*/
			inline void _calculateBoundingSphereRadius(void);

	};

}
#endif
