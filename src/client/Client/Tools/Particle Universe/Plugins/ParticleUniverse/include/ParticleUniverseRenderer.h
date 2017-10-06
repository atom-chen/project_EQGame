/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RENDERER_H__
#define __PU_RENDERER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseParticlePool.h"
#include "ParticleUniverseIAlias.h"
#include "ParticleUniverseIElement.h"

namespace ParticleUniverse
{
	/** ParticleRenderer is a virtual class and must be subclassed. A subclass of ParticleRenderer is
		responsible for rendering the visual particles. 
	@remarks
		Several types of renderers are possible. A billboard- and a mesh- renderer are examples.
    */
	class _ParticleUniverseExport ParticleRenderer : public IAlias, public IElement
	{
		protected:
			/** In case soft particles are used, a new material must be created.
	        */
			void _createSoftMaterial(void);

			/** Used to determine the original material name
			*/
			void _stripNameFromSoftPrefix(Ogre::String& name);

			// The technique to which this renderer belongs
			ParticleTechnique* mParentTechnique;

			// The billboard set that's doing the rendering
			Ogre::BillboardSet* mBillboardSet;

			// Type of the renderer
			Ogre::String mRendererType;

			/* 
			*/
			bool mCullIndividual;

			/* 
			*/
			Ogre::Real mHeight;

			/* 
			*/
			Ogre::Real mWidth;

			/* 
			*/
			Ogre::Real mDepth;

			/** Determines whether the visual particles must be sorted.
	        */
			bool mSorted;

			/** Defines the renderqueue.
	        */
			Ogre::uint8 mQueueId;

			/** Determines whether renderer is initialised.
	        */
			bool mRendererInitialised;

			/** Although the scale is on a Particle System level, the renderer can also be scaled.
			*/
			Ogre::Vector3 _mRendererScale;

			/** List of texture coordinates.
	        */
			std::vector<Ogre::FloatRect*> mUVList;

			/** If an atlas texture is used, it is possible to define texture coordinates. This is the number of 
				image rows (stacks) in the atlas texture.
	        */
			Ogre::uchar mTextureCoordsRows;

			/** If an atlas texture is used, it is possible to define texture coordinates. This is the number of 
				image columns (slices) in the atlas texture.
	        */
			Ogre::uchar mTextureCoordsColumns;

			/** If mTextureCoordsRows and mTextureCoordsColumns are set, this boolean is set to true, indicating that Texture 
				Coordinates are used.
	        */
			bool mTextureCoordsRowsAndColumnsSet;

			/** If textureCoords are set individually, this boolean is set to true, indicating that Texture 
				Coordinates are used.
	        */
			bool mTextureCoordsSet;

			/** Determines whether soft particles are used.
	        */
			bool mUseSoftParticles;

			/** Soft particles properties.
	        */
			Ogre::Real mSoftParticlesContrastPower;
			Ogre::Real mSoftParticlesScale;
			Ogre::Real mSoftParticlesDelta;

			/** In case soft particles are used, the renderer notifies the ParticleSystemManager until notification succeeded. The notification
				may be done multiple times, because a camera is needed from the Parent System, and it is not guaranteed that it already exists.
	        */
			bool mNotifiedDepthMap;

			/** Determines visibility of the renderer. This means that particles aren´t renderer anymore.
	        */
			bool mVisible;

			/** Original material (pass) settings.
	        */
			bool mDepthCheckEnabled;
			bool mDepthWriteEnabled;
			//Ogre::SceneBlendType mSceneBlending; // Not used, because Pass::getSceneBlending() doesn't exist.

		public:
			// Default values
			static const Ogre::uint8 DEFAULT_RENDER_QUEUE_GROUP;
			static const bool DEFAULT_SORTED;
			static const Ogre::uchar DEFAULT_TEXTURECOORDS_ROWS;
			static const Ogre::uchar DEFAULT_TEXTURECOORDS_COLUMNS;
			static const bool DEFAULT_USE_SOFT_PARTICLES;
			static const Ogre::Real DEFAULT_SOFT_PARTICLES_CONTRAST_POWER;
			static const Ogre::Real DEFAULT_SOFT_PARTICLES_SCALE;
			static const Ogre::Real DEFAULT_SOFT_PARTICLES_DELTA;

			ParticleRenderer(void);
			virtual ~ParticleRenderer(void);

			/** 
	        */
			const Ogre::String& getRendererType(void) const;
			void setRendererType(Ogre::String rendererType);

			/** Get / set the parent.
	        */
			ParticleTechnique* getParentTechnique(void) const;
			void setParentTechnique(ParticleTechnique* parentTechnique);

			/** Get / set whether the renderer is initialised.
	        */
			bool isRendererInitialised(void) const;
			void setRendererInitialised(bool rendererInitialised);

			/** Perform activities when a Renderer is started.
			*/
			virtual void _notifyStart(void);

			/** Perform activities when a Renderer is stopped.
			*/
			virtual void _notifyStop(void);

			/** Notify that the Particle System is rescaled.
	        */
			virtual void _notifyRescaled(const Ogre::Vector3& scale);

			/** To make currently displayed objects visible or not.
			*/
			virtual void setVisible(bool visible = true) {mVisible = visible;};

			/** Prepare the renderer before it can be used.
			*/
			virtual void _prepare(ParticleTechnique* technique) = 0;

			/** Reverse the actions from the _prepare.
	        */
			virtual void _unprepare(ParticleTechnique* particleTechnique){/* No implementation */};

			/** Returns value that indicates whether usage of soft particles is on or off.
			*/
			bool getUseSoftParticles(void) const;

			/** Set usage of soft particles on or off.
			*/
			void setUseSoftParticles(bool useSoftParticles);

			/** Contrast Power determines the strength of the alpha that is applied to the particles.
			*/
			Ogre::Real getSoftParticlesContrastPower(void) const;

			/** Scale determines the 'velocity' of alpha fading.
			*/
			Ogre::Real getSoftParticlesScale(void) const;

			/** The delta is a threshold value that determines at what 'depth distance' alpha fading is applied.
			*/
			Ogre::Real getSoftParticlesDelta(void) const;

			/** Todo
			*/
			void setSoftParticlesContrastPower(Ogre::Real softParticlesContrastPower);

			/** Todo
			*/
			void setSoftParticlesScale(Ogre::Real softParticlesScale);

			/** Todo
			*/
			void setSoftParticlesDelta(Ogre::Real softParticlesDelta);

			/** Updates the renderqueue
			@remarks
				The subclass must update the render queue using whichever Renderable instance(s) it wishes.
			*/
			inline virtual void _updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool);

			/** Sets the material this renderer must use; called by ParticleTechnique. */
			virtual void _setMaterialName(const Ogre::String& materialName) = 0;

			/** Delegated to by ParticleTechnique::_notifyCurrentCamera */
			virtual void _notifyCurrentCamera(Ogre::Camera* cam) = 0;
	
			/** Delegated to by ParticleTechnique::_notifyAttached */
			virtual void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false) = 0;

			/** The particle quota has changed */
			virtual void _notifyParticleQuota(size_t quota) = 0;

			/** The particle default size has changed */
			virtual void _notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth) = 0;

			/** Callback when particles are resized */
			virtual void _notifyParticleResized(void) = 0;

			/** Callback when particles are rotated */
			virtual void _notifyParticleZRotated(void) {}

			/** Sets renderqueue group */
			virtual void setRenderQueueGroup(Ogre::uint8 queueId);

			/** Get renderqueue group */
			virtual Ogre::uint8 getRenderQueueGroup(void) const;
			
			/** Gets the particles sort mode */
			virtual Ogre::SortMode _getSortMode(void) const = 0;

			/** Returns whether the particles are sorted */
			const bool isSorted(void) const;

			/** Determines whether the particles are sorted */
			void setSorted(bool sorted);

			/** Get the number of textureCoords rows (stacks) of an atlas texture. */
			const Ogre::uchar getTextureCoordsRows(void) const;

			/** Set the number of textureCoords rows (stacks) of an atlas texture. */
			void setTextureCoordsRows(Ogre::uchar const textureCoordsRows);

			/** Get the number of textureCoords colums (slices) of an atlas texture. */
			const Ogre::uchar getTextureCoordsColumns(void) const;

			/** Set the number of textureCoords colums (slices) of an atlas texture. */
			void setTextureCoordsColumns(Ogre::uchar const textureCoordsColumns);

			/** Return the number of texture coordinates.
	        */
			size_t getNumTextureCoords(void);

			/** Copy attributes to another renderer.
	        */
			virtual void copyAttributesTo(ParticleRenderer* renderer);

			/** Copy parent attributes to another renderer.
	        */
			virtual void copyParentAttributesTo(ParticleRenderer* renderer);

			/** Add a texture coordinate set, which consist of a the texel position (u, v), width and height
	        */
			void addTextureCoords(const Ogre::Real u, const Ogre::Real v, const Ogre::Real width, const Ogre::Real height);

			/** Returns the list with texture coordinates.
	        */
			const std::vector<Ogre::FloatRect*>& getTextureCoords(void) const;
	};

}
#endif
