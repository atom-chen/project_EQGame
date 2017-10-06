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

#include "ParticleRenderers/ParticleUniverseSphereRenderer.h"
#include "ParticleRenderers/ParticleUniverseSphere.h"
#include "ParticleUniverseSystem.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	SphereRenderer::SphereRenderer(void) : ParticleRenderer()
	{
		// Create sphere set
		mSphereSet = OGRE_NEW SphereSet("", 0, true);

		// World-relative axes
		mSphereSet->setSpheresInWorldSpace(true);
	}
	//-----------------------------------------------------------------------
	SphereRenderer::~SphereRenderer(void)
	{
		OGRE_DELETE mSphereSet;
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_prepare(ParticleTechnique* technique)
	{
		// Use the given technique, although it should be the same as mParentTechnique (must be set already)
		if (!technique || mRendererInitialised)
			return;

		_notifyParticleQuota(technique->getVisualParticleQuota());

		// Notify attached, but only if the parentnode exists
		if (technique->getParentSystem()->getParentNode())
		{		
			_notifyAttached(technique->getParentSystem()->getParentNode(), 
							technique->getParentSystem()->isParentIsTagPoint());
		}
		_notifyDefaultDimensions(_mRendererScale.x * technique->getDefaultWidth(),
								_mRendererScale.y * technique->getDefaultHeight(),
								_mRendererScale.z * technique->getDefaultDepth());
		_setMaterialName(technique->getMaterialName());
		mSphereSet->setRenderQueueGroup(mQueueId);
		mRendererInitialised = true;
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_unprepare(ParticleTechnique* technique)
	{
		// Todo
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool)
	{
		// Always perform this one
		ParticleRenderer::_updateRenderQueue(queue, pool);

		if (!mVisible)
			return;

		// Fast check to determine whether there are visual particles
		if (pool->isEmpty(Particle::PT_VISUAL))
			return;

		mSphereSet->setCullIndividually(mCullIndividual);

		mSphereSet->beginSpheres();
		Sphere sphere;
		
		VisualParticle* particle = static_cast<VisualParticle*>(pool->getFirst(Particle::PT_VISUAL));

		// Activate texture rotation if needed and use the speed of the first particle.
		// The implementation of the texture rotation is low-profile, because its value is limited. For 3d
		// particles the geometry rotation is more interesting.
		if (mSphereSet->isZRotated())
		{
			mSphereSet->rotateTexture(particle->zRotationSpeed.valueRadians());
		}

		// Loop through the particles
		while (!pool->end(Particle::PT_VISUAL))
		{
			if (particle)
			{
				sphere.mPosition = particle->position;
				sphere.mColour = particle->colour;
				sphere.mOrientation = particle->orientation;

				if (sphere.mOwnRadius = particle->ownDimensions)
				{
					sphere.mOwnRadius = true;
					sphere.setRadius (0.5 * particle->width);
				}
					
				mSphereSet->injectSphere(sphere);
			}
			
			particle = static_cast<VisualParticle*>(pool->getNext(Particle::PT_VISUAL));
		}

        mSphereSet->endSpheres();

		// Update the queue
		mSphereSet->_updateRenderQueue(queue);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		mSphereSet->_notifyAttached(parent, isTagPoint);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_setMaterialName(const Ogre::String& materialName)
	{
		mSphereSet->setMaterialName(materialName);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyCurrentCamera(Ogre::Camera* cam)
	{
		mSphereSet->_notifyCurrentCamera(cam);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyParticleQuota(size_t quota)
	{
		mSphereSet->setPoolSize(static_cast<unsigned int>(quota));
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyDefaultDimensions(Ogre::Real width, Ogre::Real height, Ogre::Real depth)
	{
		mSphereSet->setDefaultRadius(0.5 * width);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyParticleResized(void)
	{	
		mSphereSet->_notifyResized();
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::_notifyParticleZRotated(void)
	{
		mSphereSet->_notifyZRotated();
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::setRenderQueueGroup(Ogre::uint8 queueId)
	{
		mQueueId = queueId;
		if (mSphereSet)
		{
			mSphereSet->setRenderQueueGroup(queueId);
		}
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::setVisible(bool visible)
	{
		ParticleRenderer::setVisible(visible);
		mSphereSet->setVisible(visible);
	}
	//-----------------------------------------------------------------------
	void SphereRenderer::copyAttributesTo (ParticleRenderer* renderer)
	{
		// First copy parent attributes
		ParticleRenderer::copyAttributesTo(renderer);

		// First cast to SphereRenderer
		SphereRenderer* sphereRenderer = static_cast<SphereRenderer*>(renderer);

		// Copy attributes in case there is a SphereSet (which should be available)
		if (!sphereRenderer->getSphereSet())
			return;
	}

}
