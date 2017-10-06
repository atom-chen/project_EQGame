/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_RIBBONTRAIL_RENDERER_H__
#define __PU_RIBBONTRAIL_RENDERER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRenderer.h"

namespace ParticleUniverse
{
	/** Visual data specific for this type of renderer.
    */
	class _ParticleUniverseExport RibbonTrailRendererVisualData : public IVisualData
	{
		public:
			RibbonTrailRendererVisualData (Ogre::SceneNode* sceneNode, Ogre::RibbonTrail* ribbonTrail) : 
				IVisualData(), 
				node(sceneNode),
				trail(ribbonTrail),
				addedToTrail(false),
				index(0){};

			Ogre::SceneNode* node;
			bool addedToTrail;
			Ogre::RibbonTrail* trail;
			size_t index;
			virtual void setVisible(bool visible)
			{
				if (visible)
				{
					if (!addedToTrail)
					{
						trail->addNode(node);
						addedToTrail = true;
					}
				}
				else
				{
					if (addedToTrail)
					{
						trail->removeNode(node);
						addedToTrail = false;
					}
				}
			};
	};

	/** The RibbonTrailRenderer class is responsible to render particles as a RibbonTrail.
    */
	class _ParticleUniverseExport RibbonTrailRenderer : public ParticleRenderer
	{
		protected:
			std::vector<RibbonTrailRendererVisualData*> mAllVisualData;
			std::vector<RibbonTrailRendererVisualData*> mVisualData;
			size_t mQuota;
			Ogre::RibbonTrail* mTrail;
			Ogre::String mRibbonTrailName;
			bool mUseVertexColours;
			size_t mMaxChainElements;
			Ogre::Real mTrailLength;
			Ogre::Real mTrailWidth;
			bool mRandomInitialColour;
			bool mSetLength;
			bool mSetWidth;
			Ogre::ColourValue mInitialColour;
			Ogre::ColourValue mColourChange;

		public:
			// Constants
			static const bool DEFAULT_USE_VERTEX_COLOURS;
			static const size_t DEFAULT_MAX_ELEMENTS;
			static const Ogre::Real DEFAULT_LENGTH;
			static const Ogre::Real DEFAULT_WIDTH;
			static const bool DEFAULT_RANDOM_INITIAL_COLOUR;
			static const Ogre::ColourValue DEFAULT_INITIAL_COLOUR;
			static const Ogre::ColourValue DEFAULT_COLOUR_CHANGE;

			RibbonTrailRenderer(void);
	        virtual ~RibbonTrailRenderer(void);


			/** Getters and Setters
			*/
			bool isUseVertexColours(void) const;
			void setUseVertexColours(bool useVertexColours);

			size_t getMaxChainElements(void) const;
			void setMaxChainElements(size_t maxChainElements);

			Ogre::Real getTrailLength(void) const;
			void setTrailLength(Ogre::Real trailLength);

			Ogre::Real getTrailWidth(void) const;
			void setTrailWidth(Ogre::Real trailWidth);

			bool isRandomInitialColour(void) const;
			void setRandomInitialColour(bool randomInitialColour);

			const Ogre::ColourValue& getInitialColour(void) const;
			void setInitialColour(const Ogre::ColourValue& initialColour);

			const Ogre::ColourValue& getColourChange(void) const;
			void setColourChange(const Ogre::ColourValue& colourChange);

			/** Deletes all ChildSceneNodes en Entities.
			*/
			void _destroyAll(void);
			
			/** Enable the RibbonTrail
			*/
//			virtual void _notifyStart(void);

			/** Disable the RibbonTrail
			*/
//			virtual void _notifyStop(void);
			
			/** Make all objects visible or invisible.
			*/
			virtual void setVisible(bool visible);

			/** @copydoc ParticleRenderer::_prepare */
			virtual void _prepare(ParticleTechnique* technique);

			/** @copydoc ParticleRenderer::_unprepare */
			virtual void _unprepare(ParticleTechnique* technique);

			/** 
			*/
			inline virtual void _updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool);

			/** 
			*/
			virtual void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

			/** @copydoc ParticleRenderer::_setMaterialName */
			virtual void _setMaterialName(const Ogre::String& materialName);

			/** 
			*/
			virtual void _notifyCurrentCamera(Ogre::Camera* cam);
	
			/** 
			*/
			virtual void _notifyParticleQuota(size_t quota);

			/** 
			*/
			virtual void _notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth);

			/** 
			*/
			virtual void _notifyParticleResized(void);

			/** 
			*/
			virtual void _notifyParticleZRotated(void);

			/** 
			*/
			virtual void setRenderQueueGroup(Ogre::uint8 queueId);

			/** 
			*/
			virtual Ogre::SortMode _getSortMode(void) const;

			/** 
			*/
			virtual void copyAttributesTo (ParticleRenderer* renderer);
	};

}
#endif

