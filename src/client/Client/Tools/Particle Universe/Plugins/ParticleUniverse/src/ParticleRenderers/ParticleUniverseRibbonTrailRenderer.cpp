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

#include "ParticleRenderers/ParticleUniverseRibbonTrailRenderer.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	// Constants
	const bool RibbonTrailRenderer::DEFAULT_USE_VERTEX_COLOURS = true;
	const size_t RibbonTrailRenderer::DEFAULT_MAX_ELEMENTS = 10;
	const Ogre::Real RibbonTrailRenderer::DEFAULT_LENGTH = 400;
	const Ogre::Real RibbonTrailRenderer::DEFAULT_WIDTH = 5;
	const bool RibbonTrailRenderer::DEFAULT_RANDOM_INITIAL_COLOUR = true;
	const Ogre::ColourValue RibbonTrailRenderer::DEFAULT_INITIAL_COLOUR = Ogre::ColourValue::White;
	const Ogre::ColourValue RibbonTrailRenderer::DEFAULT_COLOUR_CHANGE = Ogre::ColourValue(0.5, 0.5, 0.5, 0.5);

	//-----------------------------------------------------------------------
	RibbonTrailRenderer::RibbonTrailRenderer(void) : 
		ParticleRenderer(),
		mQuota(0),
		mTrail(0),
		mUseVertexColours(DEFAULT_USE_VERTEX_COLOURS),
		mMaxChainElements(DEFAULT_MAX_ELEMENTS),
		mTrailLength(DEFAULT_LENGTH),
		mSetLength(false),
		mTrailWidth(5),
		mSetWidth(DEFAULT_WIDTH),
		mRandomInitialColour(DEFAULT_RANDOM_INITIAL_COLOUR),
		mInitialColour(DEFAULT_INITIAL_COLOUR),
		mColourChange(DEFAULT_COLOUR_CHANGE)
	{
		std::stringstream ss; 
		ss << this;
		mRibbonTrailName = "RibbonTrail" + ss.str();
	}
	//-----------------------------------------------------------------------
	RibbonTrailRenderer::~RibbonTrailRenderer(void)
	{
		if (!mParentTechnique)
			return;
		
		_destroyAll();
	}
	//-----------------------------------------------------------------------
	bool RibbonTrailRenderer::isUseVertexColours(void) const
	{
		return mUseVertexColours;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setUseVertexColours(bool useVertexColours)
	{
		mUseVertexColours = useVertexColours;
	} 
	//-----------------------------------------------------------------------
	size_t RibbonTrailRenderer::getMaxChainElements(void) const
	{
		return mMaxChainElements;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setMaxChainElements(size_t maxChainElements)
	{
		mMaxChainElements = maxChainElements;
	} 
	//-----------------------------------------------------------------------
	Ogre::Real RibbonTrailRenderer::getTrailLength(void) const
	{
		return mTrailLength;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setTrailLength(Ogre::Real trailLength)
	{
		mTrailLength = trailLength;
		mSetLength = true;
	} 
	//-----------------------------------------------------------------------
	Ogre::Real RibbonTrailRenderer::getTrailWidth(void) const
	{
		return mTrailWidth;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setTrailWidth(Ogre::Real trailWidth)
	{
		mTrailWidth = trailWidth;
		mSetWidth = true;
	} 
	//-----------------------------------------------------------------------
	bool RibbonTrailRenderer::isRandomInitialColour(void) const
	{
		return mRandomInitialColour;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setRandomInitialColour(bool randomInitialColour)
	{
		mRandomInitialColour = randomInitialColour;
	} 
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& RibbonTrailRenderer::getInitialColour(void) const
	{
		return mInitialColour;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setInitialColour(const Ogre::ColourValue& initialColour)
	{
		mInitialColour = initialColour;
	} 
	//-----------------------------------------------------------------------
	const Ogre::ColourValue& RibbonTrailRenderer::getColourChange(void) const
	{
		return mColourChange;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setColourChange(const Ogre::ColourValue& colourChange)
	{
		mColourChange = colourChange;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_destroyAll(void)
	{
		if (!mParentTechnique)
			return;

		Ogre::SceneNode* parentNode = mParentTechnique->getParentSystem()->getParentSceneNode();
		if (parentNode)
		{
			parentNode->detachObject(mTrail);
			// Note: The created ChildSceneNodes are destroyed when the ParticleSystem is destroyed
		}

		// Delete the Ribbontrail
		Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
		if (sceneManager && sceneManager->hasRibbonTrail(mRibbonTrailName))
		{
			sceneManager->destroyRibbonTrail(mRibbonTrailName);
		}

		// Delete the visual data
		std::vector<RibbonTrailRendererVisualData*>::const_iterator it;
		std::vector<RibbonTrailRendererVisualData*>::const_iterator itEnd = mAllVisualData.end();
		for (it = mAllVisualData.begin(); it != itEnd; ++it)
		{
			OGRE_DELETE_T(*it, RibbonTrailRendererVisualData, Ogre::MEMCATEGORY_SCENE_OBJECTS);
		}

		mAllVisualData.clear();
		mVisualData.clear();

		// Reset the visual data in the pool
		mParentTechnique->initVisualDataInPool();
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setVisible(bool visible)
	{
		if (mTrail)
		{
			mTrail->setVisible(visible);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_prepare(ParticleTechnique* technique)
	{
		if (!technique || mRendererInitialised)
			return;

		mQuota = technique->getVisualParticleQuota() + 
			technique->getEmittedEmitterQuota() + 
			technique->getEmittedAffectorQuota() + 
			technique->getEmittedTechniqueQuota();
		Ogre::SceneNode* parentNode = technique->getParentSystem()->getParentSceneNode();

		if (parentNode)
		{
			// Create RibbonTrail
			Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
			mTrail = sceneManager->createRibbonTrail(mRibbonTrailName);
			mTrail->setNumberOfChains(mQuota);
			mTrail->setMaxChainElements(mMaxChainElements);
			mTrail->setMaterialName(technique->getMaterialName());
			mTrail->setRenderQueueGroup(mQueueId);
			if (mSetLength)
			{
				mTrail->setTrailLength(mTrailLength);
			}
			else
			{
				mTrail->setTrailLength(technique->getDefaultHeight());
			}
			mTrail->setUseVertexColours(mUseVertexColours);

			// Create number of VisualData objects
			for (size_t i = 0; i < mQuota; i++)
			{
				RibbonTrailRendererVisualData* visualData = 
					OGRE_NEW_T(RibbonTrailRendererVisualData, Ogre::MEMCATEGORY_SCENE_OBJECTS)(parentNode->createChildSceneNode(), mTrail);
				visualData->index = i;
				mAllVisualData.push_back(visualData); // Managed by this renderer
				mVisualData.push_back(visualData); // Used to assign to a particle
				if (mRandomInitialColour)
				{
					mTrail->setInitialColour(i, Ogre::Math::UnitRandom(), Ogre::Math::UnitRandom(), Ogre::Math::UnitRandom());
				}
				else
				{
					mTrail->setInitialColour(i, mInitialColour);
				}
				mTrail->setColourChange(i, mColourChange);
				if (mSetWidth)
				{
					mTrail->setInitialWidth(i, mTrailWidth);
				}
				else
				{
					mTrail->setInitialWidth(i, mParentTechnique->getDefaultWidth());
				}
			}
			parentNode->attachObject(mTrail);
		}

		mRendererInitialised = true;
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_unprepare(ParticleTechnique* technique)
	{
		// Todo: The childnodes still exist, but because the parent node can also have other childnodes, they need to be deleted with caution
		mRendererInitialised = false; // Retriggers the _prepare function
		_destroyAll(); // Delete all nodes, they will be rebuild
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool)
	{
		// Always perform this one
		ParticleRenderer::_updateRenderQueue(queue, pool);

		if (!mVisible)
			return;

		Particle* particle = static_cast<Particle*>(pool->getFirst());
		RibbonTrailRendererVisualData* visualData = 0;
		while (!pool->end())
		{
			if (particle)
			{
				if (!particle->visualData && !mVisualData.empty())
				{
					particle->visualData = mVisualData.back();
					mVisualData.pop_back();
					visualData = static_cast<RibbonTrailRendererVisualData*>(particle->visualData);
				}
				visualData = static_cast<RibbonTrailRendererVisualData*>(particle->visualData);
				if (visualData)
				{
					Ogre::SceneNode* node = visualData->node;
					// Must subtract the node position, because it is relative to the parentnode
					node->setPosition(particle->position - mParentTechnique->getParentSystem()->getDerivedPosition());

					// Set the width of the trail if required
					if (particle->particleType == Particle::PT_VISUAL)
					{
						VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
						if (visualParticle->ownDimensions)
						{
							mTrail->setInitialWidth(visualData->index, visualParticle->width);
						}
					}
					visualData->setVisible(true);
				}
			}
			particle = static_cast<Particle*>(pool->getNext());
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		// No implementation here
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_setMaterialName(const Ogre::String& materialName)
	{
		if (mTrail)
		{
			mTrail->setMaterialName(materialName);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyCurrentCamera(Ogre::Camera* cam)
	{
		if (mTrail)
		{
			mTrail->_notifyCurrentCamera(cam);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyParticleQuota(size_t quota)
	{
		mRendererInitialised = false; // Retriggers the _prepare function
		_destroyAll(); // Delete all nodes, they will be rebuild
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyParticleResized(void)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::_notifyParticleZRotated(void)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::setRenderQueueGroup(Ogre::uint8 queueId)
	{
		mQueueId = queueId;
		if (mTrail)
		{
			mTrail->setRenderQueueGroup(queueId);
		}
	}
	//-----------------------------------------------------------------------
	Ogre::SortMode RibbonTrailRenderer::_getSortMode(void) const
	{
		return Ogre::SM_DISTANCE; // In fact there is no sorting
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRenderer::copyAttributesTo (ParticleRenderer* renderer)
	{
		// First copy parent attributes
		ParticleRenderer::copyAttributesTo(renderer);

		// First cast to RibbonTrailRenderer
		RibbonTrailRenderer* ribbonTrailRenderer = static_cast<RibbonTrailRenderer*>(renderer);
		ribbonTrailRenderer->mUseVertexColours = mUseVertexColours;
		ribbonTrailRenderer->mMaxChainElements = mMaxChainElements;
		ribbonTrailRenderer->mTrailLength = mTrailLength;
		ribbonTrailRenderer->mSetLength = mSetLength;
		ribbonTrailRenderer->mTrailWidth = mTrailWidth;
		ribbonTrailRenderer->mSetWidth = mSetWidth;
		ribbonTrailRenderer->mRandomInitialColour = mRandomInitialColour;
		ribbonTrailRenderer->mInitialColour = mInitialColour;
		ribbonTrailRenderer->mColourChange = mColourChange;
	}

}
