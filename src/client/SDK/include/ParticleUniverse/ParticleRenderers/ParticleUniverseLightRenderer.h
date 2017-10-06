/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LIGHT_RENDERER_H__
#define __PU_LIGHT_RENDERER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRenderer.h"
#include "ParticleUniverseTechnique.h"

namespace ParticleUniverse
{
	/** Visual data specific for this type of renderer.
    */
	class _ParticleUniverseExport LightRendererVisualData : public IVisualData
	{
		public:
			LightRendererVisualData (Ogre::SceneNode* sceneNode) : IVisualData(), node(sceneNode){};
			Ogre::SceneNode* node;
			Ogre::Light* light;
			virtual void setVisible(bool visible)
			{
				if (node)
				{
					node->setVisible(visible);
				}
			};
	};

	/** The LightRenderer class is responsible to render particles as a light.
    */
	class _ParticleUniverseExport LightRenderer : public ParticleRenderer
	{

		protected:
			Ogre::String mLightName; // Used for random light name prefix
			std::vector<LightRendererVisualData*> mAllVisualData;
			std::vector<LightRendererVisualData*> mVisualData;
			std::vector<Ogre::Light*> mLights;
			size_t mQuota;
			Ogre::Light::LightTypes mLightType;
			Ogre::ColourValue mDiffuseColour;
			Ogre::ColourValue mSpecularColour;
			Ogre::Real mAttenuationRange;
			Ogre::Real mAttenuationConstant;
			Ogre::Real mAttenuationLinear;
			Ogre::Real mAttenuationQuadratic;
			Ogre::Radian mSpotlightInnerAngle;
			Ogre::Radian mSpotlightOuterAngle;
			Ogre::Real mSpotlightFalloff;
			Ogre::Real mPowerScale;

			/** Make all nodes to which the entities are attached visible or invisible.
			*/
			void _makeNodesVisible(bool visible);

		public:
			// Constants
			static const Ogre::Light::LightTypes DEFAULT_LIGHT_TYPE;
			static const Ogre::ColourValue DEFAULT_DIFFUSE;
			static const Ogre::ColourValue DEFAULT_SPECULAR;
			static const Ogre::Real DEFAULT_ATT_RANGE;
			static const Ogre::Real DEFAULT_ATT_CONSTANT;
			static const Ogre::Real DEFAULT_ATT_LINEAR;
			static const Ogre::Real DEFAULT_ATT_QUADRATIC;
			static const Ogre::Radian DEFAULT_SPOT_INNER_ANGLE;
			static const Ogre::Radian DEFAULT_SPOT_OUTER_ANGLE;
			static const Ogre::Real DEFAULT_FALLOFF;
			static const Ogre::Real DEFAULT_POWER_SCALE;

			LightRenderer(void);
	        virtual ~LightRenderer(void);

			/** Return the type of light that is emitted.
			*/
			Ogre::Light::LightTypes getLightType(void) const;

			/** Set the type of light that is emitted.
			*/
			void setLightType(Ogre::Light::LightTypes lightType);

			/** 
			*/
			const Ogre::ColourValue& getDiffuseColour(void) const;
			void setDiffuseColour(const Ogre::ColourValue& diffuseColour);

			/** 
			*/
			const Ogre::ColourValue& getSpecularColour(void) const;
			void setSpecularColour(const Ogre::ColourValue& specularColour);

			/** 
			*/
			Ogre::Real getAttenuationRange(void) const;
			void setAttenuationRange(Ogre::Real attenuationRange);

			/** 
			*/
			Ogre::Real getAttenuationConstant(void) const;
			void setAttenuationConstant(Ogre::Real attenuationConstant);

			/** 
			*/
			Ogre::Real getAttenuationLinear(void) const;
			void setAttenuationLinear(Ogre::Real attenuationLinear);

			/** 
			*/
			Ogre::Real getAttenuationQuadratic(void) const;
			void setAttenuationQuadratic(Ogre::Real attenuationQuadratic);

			/** 
			*/
			const Ogre::Radian& getSpotlightInnerAngle(void) const;
			void setSpotlightInnerAngle(const Ogre::Radian& spotlightInnerAngle);

			/** 
			*/
			const Ogre::Radian& getSpotlightOuterAngle(void) const;
			void setSpotlightOuterAngle(const Ogre::Radian& spotlightOuterAngle);

			/** 
			*/
			Ogre::Real getSpotlightFalloff(void) const;
			void setSpotlightFalloff(Ogre::Real spotlightFalloff);

			/** 
			*/
			Ogre::Real getPowerScale(void) const;
			void setPowerScale(Ogre::Real powerScale);

			/** Deletes all ChildSceneNodes en Lights.
			*/
			void _destroyAll(void);

			/** Disable all ChildSceneNodes.
			*/
//			virtual void _notifyStop(void);

			/** @copydoc ParticleRenderer::setVisible */
			virtual void setVisible(bool visible = true);

			/** @copydoc ParticleRenderer::_prepare */
			virtual void _prepare(ParticleTechnique* technique);

			/** @copydoc ParticleRenderer::_unprepare */
			virtual void _unprepare(ParticleTechnique* technique);

			/** @copydoc ParticleRenderer::_updateRenderQueue */
			inline virtual void _updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool);

			/** @copydoc ParticleRenderer::_setMaterialName */
			virtual void _setMaterialName(const Ogre::String& materialName);

			/** @copydoc ParticleRenderer::_notifyCurrentCamera */
			virtual void _notifyCurrentCamera(Ogre::Camera* cam);
	
			/** @copydoc ParticleRenderer::_notifyAttached */
			virtual void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

			/** @copydoc ParticleRenderer::_notifyParticleQuota */
			virtual void _notifyParticleQuota(size_t quota);

			/** @copydoc ParticleRenderer::_notifyDefaultDimensions */
			virtual void _notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth);

			/** @copydoc ParticleRenderer::_notifyParticleResized */
			virtual void _notifyParticleResized(void);

			/** @copydoc ParticleRenderer::_getSortMode */
			virtual Ogre::SortMode _getSortMode(void) const;

			/** @copydoc ParticleRenderer::copyAttributesTo */
			virtual void copyAttributesTo (ParticleRenderer* renderer);
	};

}
#endif
