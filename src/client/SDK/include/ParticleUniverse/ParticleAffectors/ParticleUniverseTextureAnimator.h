/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TEXTURE_ANIMATION_H__
#define __PU_TEXTURE_ANIMATION_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"
#include "ParticleUniverseDynamicAttribute.h"
#include "ParticleUniverseDynamicAttributeFactory.h"

namespace ParticleUniverse
{
	/** The TextureAnimator makes it possible to have an animated texture for each individual particle. It relies on the uv coordinate 
	    settings in the ParticleRenderer.
    */
	class _ParticleUniverseExport TextureAnimator : public ParticleAffector
	{
		public:
			enum TextureAnimationType
			{
				TAT_LOOP,
				TAT_UP_DOWN,
				TAT_RANDOM
			};

			// Constants
			static const Ogre::Real DEFAULT_TIME_STEP;
			static const Ogre::uint16 DEFAULT_TEXCOORDS_START;
			static const Ogre::uint16 DEFAULT_TEXCOORDS_END;
			static const TextureAnimationType DEFAULT_ANIMATION_TYPE;
			static const bool DEFAULT_START_RANDOM;

			TextureAnimator(void);
	        virtual ~TextureAnimator(void);

			/** Returns the AnimationTimeStep. The AnimationTimeStep defines the time between each animation frame. */
			Ogre::Real getAnimationTimeStep(void) const;

			/** Set the  AnimationTimeStep */
			void setAnimationTimeStep(Ogre::Real animationTimeStep);

			/** Returns the type of texture animation. */
			TextureAnimationType getTextureAnimationType(void) const;

			/** Set the type of texture animation. */
			void setTextureAnimationType(TextureAnimationType textureAnimationType);

			/** Todo */
			Ogre::uint16 getTextureCoordsStart(void) const;

			/** Todo */
			void setTextureCoordsStart(Ogre::uint16 textureCoordsStart);

			/** Todo */
			Ogre::uint16 getTextureCoordsEnd(void) const;

			/** Todo */
			void setTextureCoordsEnd(Ogre::uint16 textureCoordsEnd);

			/** Todo */
			bool isStartRandom(void) const;

			/** Todo */
			void setStartRandom(bool startRandom);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** @copydoc ParticleAffector::_preProcessParticles */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::_initParticleForEmission */
			virtual void _initParticleForEmission(Particle* particle);

			/** @copydoc ParticleAffector::_affect */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

		protected:
			Ogre::Real mAnimationTimeStep;
			Ogre::Real mAnimationTimeStepCount;
			bool mStartRandom;
			bool mAnimationTimeStepSet;
			bool mNextIndex;
			TextureAnimationType mTextureAnimationType;
			Ogre::uint16 mTextureCoordsStart;
			Ogre::uint16 mTextureCoordsEnd;

			/** Todo.
			*/
			void _determineNextTextureCoords(VisualParticle* visualParticle);
	};

}
#endif
