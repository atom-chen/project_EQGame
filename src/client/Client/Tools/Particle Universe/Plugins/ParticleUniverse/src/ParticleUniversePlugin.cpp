/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#include "ParticleUniversePlugin.h"
#include "OgreRoot.h"

namespace ParticleUniverse
{
	const Ogre::String sPluginName = "ParticleUniverse";
	//---------------------------------------------------------------------
	ParticleUniversePlugin::ParticleUniversePlugin()
	{
	}
	//---------------------------------------------------------------------
	const Ogre::String& ParticleUniversePlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void ParticleUniversePlugin::install()
	{
#ifdef PU_PHYSICS_PHYSX
		// ----------------------- Create the PhysXBridge -----------------------
		mPhysXBridge = OGRE_NEW_T(PhysXBridge, Ogre::MEMCATEGORY_SCENE_CONTROL)();
#endif // PU_PHYSICS_PHYSX

		// ----------------------- Create the particle system manager -----------------------
		mParticleSystemManager = OGRE_NEW_T(ParticleSystemManager, Ogre::MEMCATEGORY_SCENE_CONTROL)();
		mParticleScriptDeserializer = mParticleSystemManager->getParticleScriptDeserializer();
		mGrammar = OGRE_NEW_T(Grammar, Ogre::MEMCATEGORY_SCRIPTING)();
		mParticleScriptDeserializer->addGrammar(mGrammar);

		// ----------------------- Create renderer factories -----------------------
		ParticleRendererFactory* particleRendererFactory;
		particleRendererFactory = OGRE_NEW BillboardRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		particleRendererFactory = OGRE_NEW BoxRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		particleRendererFactory = OGRE_NEW SphereRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		particleRendererFactory = OGRE_NEW EntityRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		particleRendererFactory = OGRE_NEW RibbonTrailRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		particleRendererFactory = OGRE_NEW LightRendererFactory();
		mParticleSystemManager->addRendererFactory(particleRendererFactory);
		mRendererFactories.push_back(particleRendererFactory);

		// ----------------------- Create emitter factories -----------------------
		ParticleEmitterFactory* particleEmitterFactory;
		particleEmitterFactory = OGRE_NEW PointEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW LineEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW BoxEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW CircleEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW SphereSurfaceEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW VertexEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);
		
		particleEmitterFactory = OGRE_NEW MeshSurfaceEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW PositionEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		particleEmitterFactory = OGRE_NEW SlaveEmitterFactory();
		mParticleSystemManager->addEmitterFactory(particleEmitterFactory);
		mEmitterFactories.push_back(particleEmitterFactory);

		// ----------------------- Create affector factories -----------------------
		ParticleAffectorFactory* particleAffectorFactory;
		particleAffectorFactory = OGRE_NEW ForceAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW LinearForceAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW GravityAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW ParticleFollowerFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW VortexAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW RandomiserFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW LineAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW ScaleAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW GeometryRotatorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW TextureRotatorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW TextureAnimatorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW JetAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW AlignAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW FlockCenteringAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW CollisionAvoidanceAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW VelocityMatchingAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW ColourAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW SineForceAffectorFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW ColliderFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW SphereColliderFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW PlaneColliderFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW BoxColliderFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW InterParticleColliderFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		particleAffectorFactory = OGRE_NEW PathFollowerFactory();
		mParticleSystemManager->addAffectorFactory(particleAffectorFactory);
		mAffectorFactories.push_back(particleAffectorFactory);

		// ----------------------- Create observer factories -----------------------
		ParticleObserverFactory* particleObserverFactory;
		particleObserverFactory = OGRE_NEW OnExpireObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnEmissionObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnCountObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnEventFlagObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnCollisionObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnVelocityObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnTimeObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnPositionObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnClearObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnQuotaObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		particleObserverFactory = OGRE_NEW OnRandomObserverFactory();
		mParticleSystemManager->addObserverFactory(particleObserverFactory);
		mObserverFactories.push_back(particleObserverFactory);

		// ----------------------- Create event handler factories -----------------------
		ParticleEventHandlerFactory* particleEventHandlerFactory;
		particleEventHandlerFactory = OGRE_NEW DoExpireEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoFreezeEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoPlacementParticleEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoStopSystemEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoEnableComponentEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoAffectorEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		particleEventHandlerFactory = OGRE_NEW DoScaleEventHandlerFactory();
		mParticleSystemManager->addEventHandlerFactory(particleEventHandlerFactory);
		mEventHandlerFactories.push_back(particleEventHandlerFactory);

		// ----------------------- Create extern factories -----------------------
		ExternFactory* externFactory;
		externFactory = OGRE_NEW GravityExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);

		externFactory = OGRE_NEW SphereColliderExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);

		externFactory = OGRE_NEW BoxColliderExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);

		externFactory = OGRE_NEW VortexExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);

		externFactory = OGRE_NEW SceneDecoratorExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);

#ifdef PU_PHYSICS_PHYSX
		externFactory = OGRE_NEW PhysXExternFactory();
		mParticleSystemManager->addExternFactory(externFactory);
		mExternFactories.push_back(externFactory);
#endif PU_PHYSICS_PHYSX

		// ----------------------- Create behaviour factories -----------------------
		ParticleBehaviourFactory* particleBehaviourFactory;
		particleBehaviourFactory = OGRE_NEW SlaveBehaviourFactory();
		mParticleSystemManager->addBehaviourFactory(particleBehaviourFactory);
		mBehaviourFactories.push_back(particleBehaviourFactory);
	}
	//---------------------------------------------------------------------
	void ParticleUniversePlugin::initialise()
	{
	}
	//---------------------------------------------------------------------
	void ParticleUniversePlugin::shutdown()
	{
		// Delete the grammar
		OGRE_DELETE_T(mGrammar, Grammar, Ogre::MEMCATEGORY_SCRIPTING);

		// Delete the manager.
		// The manager must be deleted before all factories are deleted (also the factories of other plugins)
		OGRE_DELETE_T(mParticleSystemManager, ParticleSystemManager, Ogre::MEMCATEGORY_SCENE_CONTROL);
	}
	//---------------------------------------------------------------------
	void ParticleUniversePlugin::uninstall()
	{
		// Delete all factories created in dllStartPlugin (after deleting the ParticleSystemManager)
		std::vector<ParticleRendererFactory*>::iterator ri;
		for (ri = mRendererFactories.begin(); ri != mRendererFactories.end(); ++ri)
		{
			OGRE_DELETE *ri;
		}

		std::vector<ParticleEmitterFactory*>::iterator ei;
		for (ei = mEmitterFactories.begin(); ei != mEmitterFactories.end(); ++ei)
		{
			OGRE_DELETE *ei;
		}

		std::vector<ParticleAffectorFactory*>::iterator ai;
		for (ai = mAffectorFactories.begin(); ai != mAffectorFactories.end(); ++ai)
		{
			OGRE_DELETE *ai;
		}
		
		std::vector<ParticleObserverFactory*>::iterator oi;
		for (oi = mObserverFactories.begin(); oi != mObserverFactories.end(); ++oi)
		{
			OGRE_DELETE *oi;
		}
		
		std::vector<ParticleEventHandlerFactory*>::iterator ehi;
		for (ehi = mEventHandlerFactories.begin(); ehi != mEventHandlerFactories.end(); ++ehi)
		{
			OGRE_DELETE *ehi;
		}

		std::vector<ExternFactory*>::iterator exi;
		for (exi = mExternFactories.begin(); exi != mExternFactories.end(); ++exi)
		{
			OGRE_DELETE *exi;
		}

		std::vector<ParticleBehaviourFactory*>::iterator bi;
		for (bi = mBehaviourFactories.begin(); bi != mBehaviourFactories.end(); ++bi)
		{
			OGRE_DELETE *bi;
		}

#ifdef PU_PHYSICS_PHYSX
		// Delete the PhysXBridge
		OGRE_DELETE_T(mPhysXBridge, PhysXBridge, Ogre::MEMCATEGORY_SCENE_CONTROL);
#endif // PU_PHYSICS_PHYSX
	}
}
