/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ParticleRenderers/ParticleUniverseLightRenderer.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::Light::LightTypes LightRenderer::DEFAULT_LIGHT_TYPE = Ogre::Light::LT_POINT;
	const Ogre::ColourValue LightRenderer::DEFAULT_DIFFUSE = Ogre::ColourValue::White;
	const Ogre::ColourValue LightRenderer::DEFAULT_SPECULAR = Ogre::ColourValue::Black;
	const Ogre::Real LightRenderer::DEFAULT_ATT_RANGE = 100000;
	const Ogre::Real LightRenderer::DEFAULT_ATT_CONSTANT = 1.0f;
	const Ogre::Real LightRenderer::DEFAULT_ATT_LINEAR = 0.0f;
	const Ogre::Real LightRenderer::DEFAULT_ATT_QUADRATIC = 0.0f;
	const Ogre::Radian LightRenderer::DEFAULT_SPOT_INNER_ANGLE = Ogre::Degree(30.0f);
	const Ogre::Radian LightRenderer::DEFAULT_SPOT_OUTER_ANGLE = Ogre::Degree(40.0f);
	const Ogre::Real LightRenderer::DEFAULT_FALLOFF = 1.0f;
	const Ogre::Real LightRenderer::DEFAULT_POWER_SCALE = 1.0f;

	//-----------------------------------------------------------------------
	LightRenderer::LightRenderer(void) : 
		ParticleRenderer(),
		mLightType(DEFAULT_LIGHT_TYPE),
		mLightName(Ogre::StringUtil::BLANK),
		mQuota(0),
		mDiffuseColour(DEFAULT_DIFFUSE),
		mSpecularColour(DEFAULT_SPECULAR),
		mAttenuationRange(DEFAULT_ATT_RANGE),
		mAttenuationConstant(DEFAULT_ATT_CONSTANT),
		mAttenuationLinear(DEFAULT_ATT_LINEAR),
		mAttenuationQuadratic(DEFAULT_ATT_QUADRATIC),
		mSpotlightInnerAngle(DEFAULT_SPOT_INNER_ANGLE),
		mSpotlightOuterAngle(DEFAULT_SPOT_OUTER_ANGLE),
		mSpotlightFalloff(DEFAULT_FALLOFF),
		mPowerScale(DEFAULT_POWER_SCALE)
	{
	}
	//-----------------------------------------------------------------------
	LightRenderer::~LightRenderer(void)
	{
		if (!mParentTechnique)
			return;
		
		_destroyAll();
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_destroyAll(void)
	{
		if (!mParentTechnique)
			return;

		// Note: The created ChildSceneNodes are destroyed when the ParticleSystem is destroyed
		
		std::vector<LightRendererVisualData*>::const_iterator it;
		std::vector<LightRendererVisualData*>::const_iterator itEnd = mAllVisualData.end();
		for (it = mAllVisualData.begin(); it != itEnd; ++it)
		{
			OGRE_DELETE_T(*it, LightRendererVisualData, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		}

		mAllVisualData.clear();
		mVisualData.clear();

		// Destroy the Lights. Do it like this, because it must be assured that the light still exists
		// and has not already been destroyed.
		Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
		for (size_t i = 0; i < mQuota; i++)
		{
			if (sceneManager->hasLight(mLightName + Ogre::StringConverter::toString(i)))
			{
				sceneManager->destroyLight(mLightName + Ogre::StringConverter::toString(i));
			}
		}
		mLights.clear();

		// Reset the visual data in the pool
		mParentTechnique->initVisualDataInPool();
	}
	//-----------------------------------------------------------------------
	Ogre::Light::LightTypes LightRenderer::getLightType(void) const
	{
		return mLightType;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setLightType(Ogre::Light::LightTypes lightType)
	{
		mLightType = lightType;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& LightRenderer::getDiffuseColour(void) const
	{
		return mDiffuseColour;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setDiffuseColour(const Ogre::ColourValue& diffuseColour)
	{
		mDiffuseColour = diffuseColour;
	}
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& LightRenderer::getSpecularColour(void) const
	{
		return mSpecularColour;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setSpecularColour(const Ogre::ColourValue& specularColour)
	{
		mSpecularColour = specularColour;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getAttenuationRange(void) const
	{
		return mAttenuationRange;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setAttenuationRange(Ogre::Real attenuationRange)
	{
		mAttenuationRange = attenuationRange;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getAttenuationConstant(void) const
	{
		return mAttenuationConstant;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setAttenuationConstant(Ogre::Real attenuationConstant)
	{
		mAttenuationConstant = attenuationConstant;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getAttenuationLinear(void) const
	{
		return mAttenuationLinear;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setAttenuationLinear(Ogre::Real attenuationLinear)
	{
		mAttenuationLinear = attenuationLinear;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getAttenuationQuadratic(void) const
	{
		return mAttenuationQuadratic;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setAttenuationQuadratic(Ogre::Real attenuationQuadratic)
	{
		mAttenuationQuadratic = attenuationQuadratic;
	}
	//-----------------------------------------------------------------------
	const Ogre::Radian& LightRenderer::getSpotlightInnerAngle(void) const
	{
		return mSpotlightInnerAngle;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setSpotlightInnerAngle(const Ogre::Radian& spotlightInnerAngle)
	{
		mSpotlightInnerAngle = spotlightInnerAngle;
	}
	//-----------------------------------------------------------------------
	const Ogre::Radian& LightRenderer::getSpotlightOuterAngle(void) const
	{
		return mSpotlightOuterAngle;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setSpotlightOuterAngle(const Ogre::Radian& spotlightOuterAngle)
	{
		mSpotlightOuterAngle = spotlightOuterAngle;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getSpotlightFalloff(void) const
	{
		return mSpotlightFalloff;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setSpotlightFalloff(Ogre::Real spotlightFalloff)
	{
		mSpotlightFalloff = spotlightFalloff;
	}
	//-----------------------------------------------------------------------
	Ogre::Real LightRenderer::getPowerScale(void) const
	{
		return mPowerScale;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::setPowerScale(Ogre::Real powerScale)
	{
		mPowerScale = powerScale;
	}
	//-----------------------------------------------------------------------
//	void LightRenderer::_notifyStop(void)
//	{
		/** The lights are not made invisible by the ParticleSystem, so it should respond to the 
			_notifyStop() itself. There is no _notifyStart, because in the _updateRenderQueue() function 
			all SceneNodes are set to visible.
		*/
//		_makeNodesVisible(false);
//	}
	//-----------------------------------------------------------------------
	void LightRenderer::setVisible(bool visible)
	{
		ParticleRenderer::setVisible(visible);

		// Always make the nodes invisible; they are made visible in the next _updateRenderQueue() call if visible is true.
		_makeNodesVisible(false);
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_makeNodesVisible(bool visible)
	{
		std::vector<LightRendererVisualData*>::const_iterator it;
		std::vector<LightRendererVisualData*>::const_iterator itEnd = mAllVisualData.end();
		for (it = mAllVisualData.begin(); it != itEnd; ++it)
		{
			(*it)->node->setVisible(visible);
		}
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_prepare(ParticleTechnique* technique)
	{
		if (!technique || mRendererInitialised)
			return;

		std::stringstream ss; 
		ss << this;
		mLightName = ss.str();
		mQuota = technique->getVisualParticleQuota();
		Ogre::SceneNode* parentNode = technique->getParentSystem()->getParentSceneNode();

		if (parentNode)
		{
			// Create number of VisualData objects including SceneNodes
			for (size_t i = 0; i < mQuota; i++)
			{
				LightRendererVisualData* visualData = 
					OGRE_NEW_T(LightRendererVisualData, Ogre::MEMCATEGORY_SCENE_OBJECTS)(parentNode->createChildSceneNode());

				mAllVisualData.push_back(visualData); // Managed by this renderer
				mVisualData.push_back(visualData); // Used to assign to a particle
			}

			// Create number of Lights
			std::vector<LightRendererVisualData*>::const_iterator it;
			std::vector<LightRendererVisualData*>::const_iterator itEnd = mAllVisualData.end();
			size_t j;
			Ogre::Light* light;
			for (it = mAllVisualData.begin(), j = 0; it != itEnd; ++it, ++j)
			{
				light = technique->getParentSystem()->getSceneManager()->createLight(mLightName + Ogre::StringConverter::toString(j));
				light->setType(mLightType);
				light->setAttenuation(mAttenuationRange, mAttenuationConstant, mAttenuationLinear, mAttenuationQuadratic);
				light->setDiffuseColour(mDiffuseColour);
				light->setSpecularColour(mSpecularColour);
				light->setSpotlightFalloff(mSpotlightFalloff);
				light->setSpotlightInnerAngle(mSpotlightInnerAngle);
				light->setSpotlightOuterAngle(mSpotlightOuterAngle);
				light->setPowerScale(mPowerScale);
				light->setRenderQueueGroup(mQueueId);
				light->setVisible(false);
				mLights.push_back(light);
				(*it)->node->attachObject(light);
				(*it)->light = light;
			}
		}

		_makeNodesVisible(false);
		mRendererInitialised = true;
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_unprepare(ParticleTechnique* technique)
	{
		// Todo
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool)
	{
		// Always perform this one
		ParticleRenderer::_updateRenderQueue(queue, pool);

		if (!mVisible)
			return;

		// Fast check to determine whether there are visual particles
		if (pool->isEmpty(Particle::PT_VISUAL))
			return;

		VisualParticle* particle = static_cast<VisualParticle*>(pool->getFirst(Particle::PT_VISUAL));
		while (!pool->end(Particle::PT_VISUAL))
		{
			if (particle)
			{
				if (!particle->visualData && !mVisualData.empty())
				{
					particle->visualData = mVisualData.back();
					mVisualData.pop_back();
				}

				if (particle->visualData)
				{
					Ogre::SceneNode* node = (static_cast<LightRendererVisualData*>(particle->visualData))->node;
					if (node)
					{
						// Must subtract the node position, because it is relative to the parentnode
						node->setPosition(particle->position - mParentTechnique->getParentSystem()->getDerivedPosition());
						node->setDirection(particle->direction, Ogre::Node::TS_WORLD); // Needed for direction of spotlight
						node->setVisible(true);

						// If the particle colour is other than its default, use it.
						if (particle->colour != Ogre::ColourValue::White)
						{
							Ogre::Light* light = (static_cast<LightRendererVisualData*>(particle->visualData))->light;
							light->setDiffuseColour(particle->colour.r, particle->colour.g, particle->colour.b);
						}
					}
				}
			}
			particle = static_cast<VisualParticle*>(pool->getNext(Particle::PT_VISUAL));
		}
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_setMaterialName(const Ogre::String& materialName)
	{
		// No implementation here, a light has no material
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_notifyCurrentCamera(Ogre::Camera* cam)
	{
		// No implementation here
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		// No implementation here
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_notifyParticleQuota(size_t quota)
	{
		mRendererInitialised = false; // Retriggers the _prepare function
		_destroyAll(); // Delete all nodes en entities, they will be rebuild
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth)
	{
		// No implementation here, a light has no dimensions
	}
	//-----------------------------------------------------------------------
	void LightRenderer::_notifyParticleResized(void)
	{
		// No implementation here, a light cannot be resized
	}
	//-----------------------------------------------------------------------
	Ogre::SortMode LightRenderer::_getSortMode(void) const
	{
		return Ogre::SM_DISTANCE; // In fact there is no sorting
	}
	//-----------------------------------------------------------------------
	void LightRenderer::copyAttributesTo (ParticleRenderer* renderer)
	{
		// First copy parent attributes
		ParticleRenderer::copyAttributesTo(renderer);

		// First cast to LightRenderer
		LightRenderer* lightRenderer = static_cast<LightRenderer*>(renderer);

		// Copy attributes
		lightRenderer->mLightType = mLightType;
		lightRenderer->mDiffuseColour = mDiffuseColour;
		lightRenderer->mSpecularColour = mSpecularColour;
		lightRenderer->mAttenuationRange = mAttenuationRange;
		lightRenderer->mAttenuationConstant = mAttenuationConstant;
		lightRenderer->mAttenuationLinear = mAttenuationLinear;
		lightRenderer->mAttenuationQuadratic = mAttenuationQuadratic;
		lightRenderer->mSpotlightInnerAngle = mSpotlightInnerAngle;
		lightRenderer->mSpotlightOuterAngle = mSpotlightOuterAngle;
		lightRenderer->mSpotlightFalloff = mSpotlightFalloff;
		lightRenderer->mPowerScale = mPowerScale;
	}

}
