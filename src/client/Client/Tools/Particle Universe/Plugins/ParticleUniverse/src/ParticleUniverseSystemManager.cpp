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

#include "ParticleUniverseSystemManager.h"

template<> ParticleUniverse::ParticleSystemManager* Ogre::Singleton<ParticleUniverse::ParticleSystemManager>::ms_Singleton = 0;
namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	ParticleSystemManager::ParticleSystemManager (void) :
		mDepthTextureName(Ogre::StringUtil::BLANK),
		mDepthMaterialName(Ogre::StringUtil::BLANK),
		mDepthVertexName(Ogre::StringUtil::BLANK),
		mDepthFragmentName(Ogre::StringUtil::BLANK),
		mLastCreatedParticleSystemTemplateName(Ogre::StringUtil::BLANK),
		mDepthMap(0),
		mDepthTechnique(0),
		mDepthPass(0),
		mDepthMapExtern(false),
		mDebugPanel(0),
		mDebugOverlay(0)
	{
		// Initialise script compilation
		mParticleScriptDeserializer = OGRE_NEW ParticleScriptDeserializer();

		// Initialise script serialisation
		mParticleScriptSerializer = OGRE_NEW ParticleScriptSerializer();

		_scriptPatterns.push_back("*.pua");
		_scriptPatterns.push_back("*.pu");
		Ogre::ResourceGroupManager::getSingleton()._registerScriptLoader(this);

		mParticleSystemFactory = OGRE_NEW ParticleSystemFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mParticleSystemFactory);

		mBoxSetFactory = OGRE_NEW BoxSetFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mBoxSetFactory);

		mSphereSetFactory = OGRE_NEW SphereSetFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mSphereSetFactory);
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager::~ParticleSystemManager (void)
	{
		// Unregister scriptloader
		Ogre::ResourceGroupManager::getSingleton()._unregisterScriptLoader(this);

		// Remove all template + aliasses
		destroyAllParticleSystemTemplates();

		// Delete the Serializer
		if (mParticleScriptSerializer)
		{
			OGRE_DELETE mParticleScriptSerializer;
			mParticleScriptSerializer = 0;
		}

		// Delete the Deserializer
		if (mParticleScriptDeserializer)
		{
			OGRE_DELETE mParticleScriptDeserializer;
			mParticleScriptDeserializer = 0;
		}

		// Delete the factories
		if (mParticleSystemFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mParticleSystemFactory);
			OGRE_DELETE mParticleSystemFactory;
			mParticleSystemFactory = 0;
		}

		if (mBoxSetFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mBoxSetFactory);
			OGRE_DELETE mBoxSetFactory;
			mBoxSetFactory = 0;
		}

		if (mSphereSetFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mSphereSetFactory);
			OGRE_DELETE mSphereSetFactory;
			mSphereSetFactory = 0;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllParticleSystemTemplates() 
	{ 
		// Delete all Particle System Templates 
		ParticleSystemTemplateMap::iterator t;
		for (t = mParticleSystemTemplates.begin(); t != mParticleSystemTemplates.end(); ++t)
		{
			OGRE_DELETE t->second;
		}
		mParticleSystemTemplates.clear();

		/** Clear the aliasmap and destroy the objects in it 
		*/ 
		destroyAllAliasses(); 
	}
	//-----------------------------------------------------------------------
	BoxSet* ParticleSystemManager::createBoxSet(const Ogre::String& name,
		Ogre::SceneManager* sceneManager,
		const Ogre::uint poolSize)
	{
		Ogre::NameValuePairList params;
		params["poolSize"] = Ogre::StringConverter::toString(poolSize);
	
		return static_cast<BoxSet*>(
			sceneManager->createMovableObject(name, BoxSetFactory::PU_FACTORY_TYPE_NAME,
			&params));
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBoxSet(BoxSet* boxSet, Ogre::SceneManager* sceneManager)
	{
		sceneManager->destroyMovableObject(boxSet);
	}
	//-----------------------------------------------------------------------
	SphereSet* ParticleSystemManager::createSphereSet(const Ogre::String& name,
		Ogre::SceneManager* sceneManager,
		const Ogre::uint poolSize)
	{
		Ogre::NameValuePairList params;
		params["poolSize"] = Ogre::StringConverter::toString(poolSize);
	
		return static_cast<SphereSet*>(
			sceneManager->createMovableObject(name, SphereSetFactory::PU_FACTORY_TYPE_NAME,
			&params));
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroySphereSet(SphereSet* sphereSet, Ogre::SceneManager* sceneManager)
	{
		sceneManager->destroyMovableObject(sphereSet);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::registerAttachable(Attachable* attachable, Ogre::SceneManager* sceneManager)
	{
		sceneManager->injectMovableObject(attachable);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::unregisterAttachable(Attachable* attachable, Ogre::SceneManager* sceneManager)
	{
		// Note, that the Attachable is not destroyed
		sceneManager->extractMovableObject(attachable);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addEmitterFactory(ParticleEmitterFactory* factory)
	{
		Ogre::String type = factory->getEmitterType();
		mEmitterFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Emitter Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleEmitterFactory* ParticleSystemManager::getEmitterFactory(const Ogre::String& emitterType)
	{
		EmitterFactoryMap::iterator it = mEmitterFactories.find(emitterType);
		if (it != mEmitterFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEmitterFactory(const Ogre::String& emitterType)
	{
		EmitterFactoryMap::iterator it = mEmitterFactories.find(emitterType);
		if (it == mEmitterFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find emitter factory to destroy.", 
				"ParticleSystemManager::destroyEmitterFactory");
		}

		OGRE_DELETE it->second;
		mEmitterFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleEmitter* ParticleSystemManager::createEmitter(const Ogre::String& emitterType)
	{
		// Locate emitter type
		EmitterFactoryMap::iterator it = mEmitterFactories.find(emitterType);

		if (it == mEmitterFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested emitter type.", 
				"ParticleSystemManager::createEmitter");
		}

		return it->second->createEmitter();
	}
	//-----------------------------------------------------------------------
	ParticleEmitter* ParticleSystemManager::cloneEmitter(ParticleEmitter* emitter)
	{
		if (!emitter)
			return 0;

		ParticleEmitter* clonedEmitter = createEmitter(emitter->getEmitterType());
		emitter->copyAttributesTo(clonedEmitter);
		return clonedEmitter;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEmitter(ParticleEmitter* emitter)
	{
		// Locate emitter type
		EmitterFactoryMap::iterator it = mEmitterFactories.find(emitter->getEmitterType());

		if (it == mEmitterFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested emitter type.", 
				"ParticleSystemManager::destroyEmitter");
		}

		it->second->destroyEmitter(emitter);
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystemManager::createTechnique(void)
	{
		ParticleTechnique* technique = OGRE_NEW_T(ParticleTechnique, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
		technique->setRenderer(createRenderer("Billboard")); // Set default renderer
		return technique;
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystemManager::cloneTechnique(ParticleTechnique* technique)
	{
		if (!technique)
			return 0;

		ParticleTechnique* clonedTechnique = createTechnique();
		technique->copyAttributesTo(clonedTechnique);
		return clonedTechnique;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyTechnique(ParticleTechnique* technique)
	{
		// Deletion must be done by the same object that also created it
		OGRE_DELETE_T(technique, ParticleTechnique, Ogre::MEMCATEGORY_SCENE_OBJECTS);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addAffectorFactory(ParticleAffectorFactory* factory)
	{
		Ogre::String type = factory->getAffectorType();
		mAffectorFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Affector Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleAffectorFactory* ParticleSystemManager::getAffectorFactory(const Ogre::String& affectorType)
	{
		AffectorFactoryMap::iterator it = mAffectorFactories.find(affectorType);
		if (it != mAffectorFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAffectorFactory(const Ogre::String& affectorType)
	{
		AffectorFactoryMap::iterator it = mAffectorFactories.find(affectorType);
		if (it == mAffectorFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find affector factory to destroy.", 
				"ParticleSystemManager::destroyAffectorFactory");
		}

		OGRE_DELETE it->second;
		mAffectorFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleAffector* ParticleSystemManager::createAffector(const Ogre::String& affectorType)
	{
		// Locate affector type
		AffectorFactoryMap::iterator it = mAffectorFactories.find(affectorType);

		if (it == mAffectorFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested affector type.", 
				"ParticleSystemManager::createAffector");
		}

		return it->second->createAffector();
	}
	//-----------------------------------------------------------------------
	ParticleAffector* ParticleSystemManager::cloneAffector(ParticleAffector* affector)
	{
		if (!affector)
			return 0;

		ParticleAffector* clonedAffector = createAffector(affector->getAffectorType());
		affector->copyAttributesTo(clonedAffector);
		return clonedAffector;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAffector(ParticleAffector* affector)
	{
		// Locate affector type
		AffectorFactoryMap::iterator it = mAffectorFactories.find(affector->getAffectorType());

		if (it == mAffectorFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested affector type.", 
				"ParticleSystemManager::destroyAffector");
		}

		it->second->destroyAffector(affector);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addObserverFactory(ParticleObserverFactory* factory)
	{
		Ogre::String type = factory->getObserverType();
		mObserverFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Observer Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleObserverFactory* ParticleSystemManager::getObserverFactory(const Ogre::String& observerType)
	{
		ObserverFactoryMap::iterator it = mObserverFactories.find(observerType);
		if (it != mObserverFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	ParticleObserver* ParticleSystemManager::createObserver(const Ogre::String& observerType)
	{
		// Locate Observer type
		ObserverFactoryMap::iterator it = mObserverFactories.find(observerType);

		if (it == mObserverFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested observer type.", 
				"ParticleSystemManager::createObserver");
		}

		return it->second->createObserver();
	}
	//-----------------------------------------------------------------------
	ParticleObserver* ParticleSystemManager::cloneObserver(ParticleObserver* observer)
	{
		if (!observer)
			return 0;

		ParticleObserver* clonedObserver = createObserver(observer->getObserverType());
		observer->copyAttributesTo(clonedObserver);
		return clonedObserver;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyObserver(ParticleObserver* observer)
	{
		// Locate Observer type
		ObserverFactoryMap::iterator it = mObserverFactories.find(observer->getObserverType());

		if (it == mObserverFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested observer type.", 
				"ParticleSystemManager::destroyObserver");
		}

		it->second->destroyObserver(observer);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addEventHandlerFactory(ParticleEventHandlerFactory* factory)
	{
		Ogre::String type = factory->getEventHandlerType();
		mEventHandlerFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle EventHandler Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleEventHandlerFactory* ParticleSystemManager::getEventHandlerFactory(const Ogre::String& eventHandlerType)
	{
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(eventHandlerType);
		if (it != mEventHandlerFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	ParticleEventHandler* ParticleSystemManager::createEventHandler(const Ogre::String& eventHandlerType)
	{
		// Locate EventHandler type
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(eventHandlerType);

		if (it == mEventHandlerFactories.end())

		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested event handler type.", 
				"ParticleSystemManager::createEventHandler");
		}

		return it->second->createEventHandler();
	}
	//-----------------------------------------------------------------------
	ParticleEventHandler* ParticleSystemManager::cloneEventHandler(ParticleEventHandler* eventHandler)
	{
		if (!eventHandler)
			return 0;

		ParticleEventHandler* clonedEventHandler = createEventHandler(eventHandler->getEventHandlerType());
		eventHandler->copyAttributesTo(clonedEventHandler);
		return clonedEventHandler;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEventHandler(ParticleEventHandler* eventHandler)
	{
		// Locate EventHandler type
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(eventHandler->getEventHandlerType());

		if (it == mEventHandlerFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested event handler type.", 
				"ParticleSystemManager::destroyEventHandler");

		}

		it->second->destroyEventHandler(eventHandler);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addRendererFactory(ParticleRendererFactory* factory)
	{
		Ogre::String type = factory->getRendererType();
		mRendererFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Renderer Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleRendererFactory* ParticleSystemManager::getRendererFactory(const Ogre::String& rendererType)
	{
		RendererFactoryMap::iterator it = mRendererFactories.find(rendererType);
		if (it != mRendererFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyRendererFactory(const Ogre::String& rendererType)
	{
		RendererFactoryMap::iterator it = mRendererFactories.find(rendererType);
		if (it == mRendererFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find renderer factory to destroy.", 
				"ParticleSystemManager::destroyRendererFactory");
		}

		OGRE_DELETE it->second;
		mRendererFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleRenderer* ParticleSystemManager::createRenderer(const Ogre::String& rendererType)
	{
		// Locate renderer type
		RendererFactoryMap::iterator it = mRendererFactories.find(rendererType);

		if (it == mRendererFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested renderer type.", 
				"ParticleSystemManager::createRenderer");
		}

		return it->second->createRenderer();
	}
	//-----------------------------------------------------------------------
	ParticleRenderer* ParticleSystemManager::cloneRenderer(ParticleRenderer* renderer)
	{
		if (!renderer)
			return 0;

		ParticleRenderer* clonedRenderer = createRenderer(renderer->getRendererType());
		renderer->copyAttributesTo(clonedRenderer);
		return clonedRenderer;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyRenderer(ParticleRenderer* renderer)
	{
		// Locate renderer type
		RendererFactoryMap::iterator it = mRendererFactories.find(renderer->getRendererType());

		if (it == mRendererFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested renderer type.", 
                "ParticleSystemManager::destroyRenderer");
		}

		it->second->destroyRenderer(renderer);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addExternFactory(ExternFactory* factory)
	{
		Ogre::String type = factory->getExternType();
		mExternFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Extern Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ExternFactory* ParticleSystemManager::getExternFactory(const Ogre::String& externType)
	{
		ExternFactoryMap::iterator it = mExternFactories.find(externType);
		if (it != mExternFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyExternFactory(const Ogre::String& externType)
	{
		ExternFactoryMap::iterator it = mExternFactories.find(externType);
		if (it == mExternFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find extern factory to destroy.", 
				"ParticleSystemManager::destroyExternFactory");
		}

		OGRE_DELETE it->second;
		mExternFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	Extern* ParticleSystemManager::createExtern(const Ogre::String& externType)
	{
		// Locate Extern type
		ExternFactoryMap::iterator it = mExternFactories.find(externType);

		if (it == mExternFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested extern type.", 
				"ParticleSystemManager::createExtern");
		}

		return it->second->createExtern();
	}
	//-----------------------------------------------------------------------
	Extern* ParticleSystemManager::cloneExtern(Extern* externObject)
	{
		if (!externObject)
			return 0;

		Extern* clonedExtern = createExtern(externObject->getExternType());
		externObject->copyAttributesTo(clonedExtern);
		return clonedExtern;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyExtern(Extern* externObject)
	{
		// Locate extern type
		ExternFactoryMap::iterator it = mExternFactories.find(externObject->getExternType());

		if (it == mExternFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested extern type.", 
                "ParticleSystemManager::destroyExtern");
		}

		it->second->destroyExtern(externObject);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addBehaviourFactory(ParticleBehaviourFactory* factory)
	{
		Ogre::String type = factory->getBehaviourType();
		mBehaviourFactories[type] = factory;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle Behaviour Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleBehaviourFactory* ParticleSystemManager::getBehaviourFactory(const Ogre::String& behaviourType)
	{
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(behaviourType);
		if (it != mBehaviourFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBehaviourFactory(const Ogre::String& behaviourType)
	{
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(behaviourType);
		if (it == mBehaviourFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find behaviour factory to destroy.", 
				"ParticleSystemManager::destroyBehaviourFactory");
		}

		OGRE_DELETE it->second;
		mBehaviourFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleBehaviour* ParticleSystemManager::createBehaviour(const Ogre::String& behaviourType)
	{
		// Locate Behaviour type
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(behaviourType);

	if (it == mBehaviourFactories.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested behaviour type.", 
			"ParticleSystemManager::createBehaviour");
	}

		return it->second->createBehaviour();
	}
	//-----------------------------------------------------------------------
	ParticleBehaviour* ParticleSystemManager::cloneBehaviour(ParticleBehaviour* behaviour)
	{
		if (!behaviour)
			return 0;

		ParticleBehaviour* clonedBehaviour = createBehaviour(behaviour->getBehaviourType());
		behaviour->copyAttributesTo(clonedBehaviour);
		return clonedBehaviour;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBehaviour(ParticleBehaviour* behaviour)
	{
		// Locate Behaviour type
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(behaviour->getBehaviourType());

		if (it == mBehaviourFactories.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find requested behaviour type.", 
                "ParticleSystemManager::destroyBehaviour");
		}

		it->second->destroyBehaviour(behaviour);
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystemTemplate(const Ogre::String& name, const Ogre::String& resourceGroupName)
	{
		// Validate name and add a prefix if needed.
		Ogre::String expName = name;
		while (mParticleSystemTemplates.find(expName) != mParticleSystemTemplates.end())
		{
			Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle System template '" + expName + "' already exists; create copy.");
			expName = Ogre::String("CopyOf") + expName;
		}

		ParticleSystem* particleSystemTemplate = OGRE_NEW ParticleSystem(expName, resourceGroupName);
		addParticleSystemTemplate(expName, particleSystemTemplate);
		mLastCreatedParticleSystemTemplateName = expName;
		return particleSystemTemplate;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::replaceParticleSystemTemplate(const Ogre::String& name, ParticleSystem* system)
	{
		ParticleSystem* systemTemplate = getParticleSystemTemplate(name);
		if (systemTemplate)
		{
			*systemTemplate = *system;
			system->copyAttributesTo(systemTemplate);
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystemManager::getLastCreatedParticleSystemTemplateName(void)
	{
		return mLastCreatedParticleSystemTemplateName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addParticleSystemTemplate(const Ogre::String& name, ParticleSystem* systemTemplate)
	{
		// Validate name
		if (mParticleSystemTemplates.find(name) != mParticleSystemTemplates.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM, 
				"PU: ParticleSystem template with name '" + name + "' already exists.", 
				"ParticleSystemManager::addParticleSystemTemplate");
		}

		mParticleSystemTemplates[name] = systemTemplate;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Particle System Template '" + name + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::getParticleSystemTemplate(const Ogre::String& templateName)
	{
		ParticleSystemTemplateMap::iterator i = mParticleSystemTemplates.find(templateName);
		if (i != mParticleSystemTemplates.end())
		{
			return i->second;
		}
	
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyParticleSystemTemplate(const Ogre::String& templateName)
	{
		ParticleSystemTemplateMap::iterator i = mParticleSystemTemplates.find(templateName);
		if (i != mParticleSystemTemplates.end())
		{
			OGRE_DELETE (i->second);
			mParticleSystemTemplates.erase(i);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::particleSystemTemplateNames(std::vector<Ogre::String>& v)
	{
		std::vector<Ogre::String>::iterator;
		ParticleSystemTemplateMap::iterator it;
		ParticleSystemTemplateMap::iterator itEnd = mParticleSystemTemplates.end();
		for (it = mParticleSystemTemplates.begin(); it != itEnd; ++it)
		{
			v.push_back((*it).first);
		}
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystem(const Ogre::String& name,
		const Ogre::String& templateName,
		Ogre::SceneManager* sceneManager)
	{
		// Validate name
		if (mParticleSystems.find(name) != mParticleSystems.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM, 
				"PU: ParticleSystem with name '" + name + "' already exists.", 
				"ParticleSystemManager::createParticleSystem");
		}

		Ogre::NameValuePairList params;
		params["templateName"] = templateName;
	
		ParticleSystem* system = static_cast<ParticleSystem*>(
			sceneManager->createMovableObject(name, ParticleSystemFactory::PU_FACTORY_TYPE_NAME,
			&params));
		system->setSceneManager(sceneManager);
		system->setTemplateName(templateName);
		mParticleSystems[name] = system;
		return system;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystem(const Ogre::String& name,
		Ogre::SceneManager* sceneManager)
	{
		// Validate name
		if (mParticleSystems.find(name) != mParticleSystems.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM, 
				"PU: ParticleSystem with name '" + name + "' already exists.", 
				"ParticleSystemManager::createParticleSystem");
		}

		Ogre::NameValuePairList params;
		ParticleSystem* system = static_cast<ParticleSystem*>(
		sceneManager->createMovableObject(name, ParticleSystemFactory::PU_FACTORY_TYPE_NAME, 
			&params));
		system->setSceneManager(sceneManager);
		mParticleSystems[name] = system;
		return system;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::getParticleSystem(const Ogre::String& name)
	{
		if (name == Ogre::StringUtil::BLANK)
			return 0;

		ParticleSystemMap::iterator i = mParticleSystems.find(name);
		if (i != mParticleSystems.end())
		{
			return i->second;
		}
	
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyParticleSystem(ParticleSystem* particleSystem, Ogre::SceneManager* sceneManager)
	{
		if (!particleSystem)
			return;

		ParticleSystemMap::iterator i = mParticleSystems.find(particleSystem->getName());
		if (i != mParticleSystems.end())
		{
			mParticleSystems.erase(i);
		}

		// First determine whether the Particle System still exists, before it is really destroyed.
		if (sceneManager->hasMovableObject(particleSystem->getName(), ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
		{
			sceneManager->destroyMovableObject(particleSystem);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyParticleSystem(const Ogre::String& particleSystemName, Ogre::SceneManager* sceneManager)
	{
		// Remove it from the map although it doesn´t exist anymore
		ParticleSystemMap::iterator i = mParticleSystems.find(particleSystemName);
		if (i != mParticleSystems.end())
		{
			mParticleSystems.erase(i);
		}

		// First determine whether the Particle System still exists, before it is really destroyed.
		if (sceneManager->hasMovableObject(particleSystemName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
		{
			sceneManager->destroyMovableObject(particleSystemName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllParticleSystems(Ogre::SceneManager* sceneManager)
	{ 
		// Delete all Particle Systems. This doesn´t include the templates
		ParticleSystemMap::iterator t;
		for (t = mParticleSystems.begin(); t != mParticleSystems.end(); ++t)
		{
			ParticleSystem* particleSystem = t->second;
			if (sceneManager->hasMovableObject(particleSystem->getName(), ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
			{
				sceneManager->destroyMovableObject(particleSystem);
			}
		}
		mParticleSystems.clear();
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::_createSystemImpl(const Ogre::String& name)
	{
		ParticleSystem* sys = OGRE_NEW ParticleSystem(name);
		return sys;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::_createSystemImpl(const Ogre::String& name, const Ogre::String& templateName)
	{
		// Look up template
		ParticleSystem* pTemplate = getParticleSystemTemplate(templateName);
		if (!pTemplate)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find required template '" + templateName + "'", "ParticleSystemManager::createSystem");
		}

		ParticleSystem* sys = OGRE_NEW ParticleSystem(name);
        
		// Copy template settings
		*sys = *pTemplate;
		pTemplate->copyAttributesTo(sys);
		return sys;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::_destroySystemImpl(ParticleSystem* particleSystem)
	{
		OGRE_DELETE particleSystem;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addAlias (IAlias* alias)
	{
		Ogre::String name = alias->getAliasName();
		mAliasMap[name] = alias;
		Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Alias '" + name + "' registered");
	}
	//-----------------------------------------------------------------------
	IAlias* ParticleSystemManager::getAlias (const Ogre::String& aliasName)
	{
		IAlias* alias = 0;
		AliasMap::iterator it = mAliasMap.find(aliasName);
		if (it == mAliasMap.end())
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Cannot find alias.", 
				"ParticleSystemManager::getAlias");
		}

		return it->second;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAlias(IAlias* alias)
	{
		switch (alias->getAliasType())
		{
			case IAlias::AT_RENDERER:
			{
				ParticleRenderer* renderer = static_cast<ParticleRenderer*>(alias);
				destroyRenderer(renderer);
			}
			break;

			case IAlias::AT_TECHNIQUE:
			{
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(alias);
				destroyTechnique(technique);
			}
			break;

			case IAlias::AT_EMITTER:
			{
				ParticleEmitter* emitter = static_cast<ParticleEmitter*>(alias);
				destroyEmitter(emitter);
			}
			break;
	
			case IAlias::AT_AFFECTOR:
			{
				ParticleAffector* affector = static_cast<ParticleAffector*>(alias);
				destroyAffector(affector);
			}
			break;

			case IAlias::AT_OBSERVER:
			{
				ParticleObserver* observer = static_cast<ParticleObserver*>(alias);
				destroyObserver(observer);
			}
			break;

			case IAlias::AT_HANDLER:
			{
				ParticleEventHandler* handler = static_cast<ParticleEventHandler*>(alias);
				destroyEventHandler(handler);
			}
			break;

			case IAlias::AT_EXTERN:
			{
				Extern* externObject = static_cast<Extern*>(alias);
				destroyExtern(externObject);
			}
			break;

			case IAlias::AT_BEHAVIOUR:
			{
				ParticleBehaviour* behaviour = static_cast<ParticleBehaviour*>(alias);
				destroyBehaviour(behaviour);
			}
			break;

			case IAlias::AT_UNDEFINED:
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "PU: Trying to delete alias of undetermined type.", 
			        "ParticleSystemManager::~ParticleSystemManager");
			break;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllAliasses(void)
	{
		AliasMap::iterator it;
		for (it = mAliasMap.begin(); it != mAliasMap.end(); ++it)
		{
			destroyAlias(it->second);
		}
		mAliasMap.clear();
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager* ParticleSystemManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager& ParticleSystemManager::getSingleton(void)
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::parseScript(Ogre::DataStreamPtr& stream, const Ogre::String& groupName)
	{
		mParticleScriptDeserializer->parseScript(stream, groupName);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::writeScript(ParticleSystem* particleSystem, const Ogre::String& fileName)
	{
		mParticleScriptSerializer->writeScript(particleSystem, fileName);
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystemManager::writeScript(ParticleSystem* particleSystem)
	{
		return mParticleScriptSerializer->writeScript(particleSystem);
	}
	//-----------------------------------------------------------------------
	const Ogre::StringVector& ParticleSystemManager::getScriptPatterns(void) const
	{
		return _scriptPatterns;
	}
	//-----------------------------------------------------------------------
	Ogre::Real ParticleSystemManager::getLoadingOrder(void) const
	{
		return 1500.0f;
	}
	//-----------------------------------------------------------------------
	ParticleScriptDeserializer* ParticleSystemManager::getParticleScriptDeserializer(void) const
	{
		return mParticleScriptDeserializer;
	}
	//-----------------------------------------------------------------------
	ParticleScriptSerializer* ParticleSystemManager::getParticleScriptSerializer(void) const
	{
		return mParticleScriptSerializer;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		// Register its ´own´ parse-functions
		mParticleSystemManagerTokens.setupTokenDefinitions(tokenRegister);

		// Register generic attribute parse-functions
		// Note, that generic Extern tokens do not exist, because Externs are very specific.
		mParticleSystemTokens.setupTokenDefinitions(tokenRegister);
		mParticleTechniqueTokens.setupTokenDefinitions(tokenRegister);
		mParticleRendererTokens.setupTokenDefinitions(tokenRegister);
		mParticleEmitterTokens.setupTokenDefinitions(tokenRegister);
		mParticleAffectorTokens.setupTokenDefinitions(tokenRegister);
		mParticleObserverTokens.setupTokenDefinitions(tokenRegister);
		mAttachableTokens.setupTokenDefinitions(tokenRegister);
		mDynamicAttributeTokens.setupTokenDefinitions(tokenRegister);
		mCameraDependencyTokens.setupTokenDefinitions(tokenRegister);

		// Register Renderer factory parse-functions
		RendererFactoryMap::iterator itRenderer;
		for (itRenderer = mRendererFactories.begin(); itRenderer != mRendererFactories.end(); ++itRenderer)
		{
			itRenderer->second->setupTokenDefinitions(tokenRegister);
		}

		// Register Emitter factory parse-functions.
		EmitterFactoryMap::iterator itEmitter;
		for (itEmitter = mEmitterFactories.begin(); itEmitter != mEmitterFactories.end(); ++itEmitter)
		{
			itEmitter->second->setupTokenDefinitions(tokenRegister);
		}

		// Register Affector factory parse-functions
		AffectorFactoryMap::iterator itAffector;
		for (itAffector = mAffectorFactories.begin(); itAffector != mAffectorFactories.end(); ++itAffector)
		{
			itAffector->second->setupTokenDefinitions(tokenRegister);
		}

		// Register Observer factory parse-functions
		ObserverFactoryMap::iterator itObserver;
		for (itObserver = mObserverFactories.begin(); itObserver != mObserverFactories.end(); ++itObserver)
		{
			itObserver->second->setupTokenDefinitions(tokenRegister);
		}

		// Register EventHandler factory parse-functions
		EventHandlerFactoryMap::iterator itEventHandler;
		for (itEventHandler = mEventHandlerFactories.begin(); itEventHandler != mEventHandlerFactories.end(); ++itEventHandler)
		{
			itEventHandler->second->setupTokenDefinitions(tokenRegister);
		}

		// Register Behaviour factory parse-functions
		BehaviourFactoryMap::iterator itBehaviour;
		for (itBehaviour = mBehaviourFactories.begin(); itBehaviour != mBehaviourFactories.end(); ++itBehaviour)
		{
			itBehaviour->second->setupTokenDefinitions(tokenRegister);
		}

		// Register Extern factory parse-functions
		ExternFactoryMap::iterator itExtern;
		for (itExtern = mExternFactories.begin(); itExtern != mExternFactories.end(); ++itExtern)
		{
			itExtern->second->setupTokenDefinitions(tokenRegister);
		}
	}
	//-----------------------------------------------------------------------
	CameraDependency* ParticleSystemManager::createCameraDependency(void)
	{
		mCameraDependency.setThreshold(1000);
		mCameraDependency.setIncrease(false);
		return &mCameraDependency;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::createDepthMap (Ogre::Camera* camera, Ogre::SceneManager* sceneManager)
	{
		// Don´t recreate the depth map
		if (mDepthMap || mDepthMapExtern)
		{
			return;
		}

		// Create a RenderTexture and material if not available already
		if (mDepthTextureName.empty())
		{
			std::stringstream ss1;
			ss1 << this;
			mDepthTextureName = "DepthTexture" + ss1.str();
			Ogre::TexturePtr depthTexturePtr = Ogre::TextureManager::getSingleton().createManual(mDepthTextureName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				Ogre::TEX_TYPE_2D, 
				camera->getViewport()->getActualWidth(), 
				camera->getViewport()->getActualHeight(),
				0, 
				Ogre::PF_FLOAT16_R, 
				Ogre::TU_RENDERTARGET);
		}

		if (mDepthMaterialName.empty())
		{
			// Create a vertex and fragment program (hlsl)
			mDepthVertexName = "ParticleUniverse_DepthMapVP"; // Use ParticleUniverse_ to avoid name conflicts.
			mDepthFragmentName = "ParticleUniverse_DepthMapFP"; // Use ParticleUniverse_ to avoid name conflicts.
			Ogre::HighLevelGpuProgramPtr vertexProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram( 
				mDepthVertexName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				"hlsl",
			Ogre::GPT_VERTEX_PROGRAM);	
			vertexProgram->setSourceFile("pu_depth_sm20.hlsl");
			vertexProgram->setParameter("target", "vs_2_0");
			vertexProgram->setParameter("entry_point", "mainVP"); // Must be same name as in pu_depth.hlsl
			vertexProgram->load();

			Ogre::HighLevelGpuProgramPtr fragmentProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram( 
				mDepthFragmentName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				"hlsl",
				Ogre::GPT_FRAGMENT_PROGRAM);
			fragmentProgram->setSourceFile("pu_depth_sm20.hlsl");
			fragmentProgram->setParameter("target", "ps_2_0");
			fragmentProgram->setParameter("entry_point", "mainFP"); // Must be same name as in pu_depth.hlsl
			fragmentProgram->load();

			// Create a depth rendering technique, which is used for all renderables in the scene
			std::stringstream ss2;
			ss2 << this;
			mDepthMaterialName = "DepthMaterial" + ss2.str();
			Ogre::MaterialPtr depthMaterial = Ogre::MaterialManager::getSingleton().create(mDepthMaterialName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			mDepthTechnique = depthMaterial->getTechnique(0); // There is already a technique and a pass created
			mDepthPass = mDepthTechnique->getPass(0);
			Ogre::TextureUnitState* textureUnitState = mDepthPass->createTextureUnitState(mDepthTextureName);
			mDepthPass->setDepthCheckEnabled(false);
			mDepthPass->setDepthWriteEnabled(false);
			mDepthPass->setLightingEnabled(false);
			mDepthPass->setVertexProgram(mDepthVertexName);
			mDepthPass->setFragmentProgram(mDepthFragmentName);
			Ogre::GpuProgramParametersSharedPtr vertexParams = mDepthPass->getVertexProgramParameters(); 
			vertexParams->setNamedAutoConstant("worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX); // Must be same name as in pu_depth.hlsl
			vertexParams->setNamedAutoConstant("texelOffsets", Ogre::GpuProgramParameters::ACT_TEXEL_OFFSETS); // Must be same name as in pu_depth.hlsl
			vertexParams->setNamedAutoConstant("depthRange", Ogre::GpuProgramParameters::ACT_SCENE_DEPTH_RANGE); // Must be same name as in pu_depth.hlsl
		}

		// Set depth map
		Ogre::TexturePtr depthTexturePtr = Ogre::TextureManager::getSingleton().getByName(mDepthTextureName);
		if (!depthTexturePtr.isNull())
		{
			mDepthMap = depthTexturePtr->getBuffer()->getRenderTarget();
			Ogre::Viewport* viewport = mDepthMap->addViewport(camera);
			viewport->setBackgroundColour(Ogre::ColourValue::Black);
			mDepthMap->addListener(&mDepthMapTargetListener);
			mDepthMap->setAutoUpdated(true);

			// Set attributes to the target listener
			mDepthMapTargetListener.mSceneManager = sceneManager;
			mDepthMapTargetListener.mDepthTechnique = mDepthTechnique;
			mDepthMapTargetListener.mCamera = camera;
			mDepthMapTargetListener.mDepthMap = mDepthMap;
		}

		// Set debug overlay for testing purposes (uncomment if you want to view the depth map in the debug overlay)
		/*
		if (!mDebugOverlay)
		{
			Ogre::MaterialPtr debugMaterial = Ogre::MaterialManager::getSingleton().create("ParticleUniverse_DebugMaterial", 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			debugMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(mDepthTextureName);
			mDebugPanel = (Ogre::OverlayContainer*)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "ParticleUniverse_DebugDepth"));
			mDebugPanel->_setPosition(0.7, 0);
			mDebugPanel->_setDimensions(0.3, 0.3);
			mDebugPanel->setMaterialName("ParticleUniverse_DebugMaterial");
			mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
			mDebugOverlay->add2D(mDebugPanel);
		}
		*/
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyDepthMap (void)
	{
		// Reset the depth map.
		if (mDepthMap && !mDepthMapExtern)
		{
			mDepthMap->removeAllListeners();
			mDepthMap->removeAllViewports();
			mDepthMap = 0;
		}
	}
	//-----------------------------------------------------------------------
	bool ParticleSystemManager::notifyDepthMapNeeded(Ogre::Camera* camera, Ogre::SceneManager* sceneManager)
	{
		if (mDepthMap || mDepthMapExtern)
		{
			return true;
		}
		else
		{
			 if (camera && sceneManager)
			 {
				/** Use the first camera that is passed. Although in theory, other camera's may be passed, they are ignored. The depth map is
					created for this camera viewport only.
				*/ 
				createDepthMap (camera, sceneManager);
				return true;
			 }
			 else
			 {
				 return false;
			 }
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::registerSoftParticlesRenderer(ParticleRenderer* renderer)
	{
		if (!mDepthMapExtern)
		{
			mDepthMapTargetListener.registerRenderer(renderer);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::unregisterSoftParticlesRenderer(ParticleRenderer* renderer)
	{
		if (!mDepthMapExtern)
		{
			// Unregister the renderers and destroy the depthmap, because apparently it isn´t used anymore
			mDepthMapTargetListener.unregisterRenderer(renderer);
			if (mDepthMapTargetListener.registeredRenderersEmpty())
			{
				destroyDepthMap();
			}
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystemManager::getDepthTextureName (void)
	{
		return mDepthTextureName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::setExternDepthTextureName (const Ogre::String& depthTextureName)
	{
		mDepthTextureName = depthTextureName;
		mDepthMapExtern = true;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::resetExternDepthTextureName (void)
	{
		mDepthTextureName = Ogre::StringUtil::BLANK;
		mDepthMapExtern = false;
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* ParticleSystemManager::createDynamicAttribute(DynamicAttribute::DynamicAttributeType type)
	{
		switch (type)
		{
			case DynamicAttribute::DAT_FIXED:
				return OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_RANDOM:
				return OGRE_NEW_T(DynamicAttributeRandom, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_CURVED:
				return OGRE_NEW_T(DynamicAttributeCurved, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_OSCILLATE:
				return OGRE_NEW_T(DynamicAttributeOscillate, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			break;
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		// Exclude the particle system renderers that renders soft particles, by setting visibility to false
		std::vector<ParticleRenderer*>::iterator it;
		std::vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			(*it)->setVisible(false);
		}

		// Exclude the overlays
		mCamera->getViewport()->setOverlaysEnabled(false);

		// Add the DepthMapTargetListener as a RenderableListener to replace the technique for all renderables
		Ogre::RenderQueue* queue = mSceneManager->getRenderQueue();
		queue->setRenderableListener(this);
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		// Include the particle system renderers again
		std::vector<ParticleRenderer*>::iterator it;
		std::vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			(*it)->setVisible(true);
		}

		// Reset the RenderableListener
		Ogre::RenderQueue* queue = mSceneManager->getRenderQueue();
		queue->setRenderableListener(0);

		// Reset the overlays
		mCamera->getViewport()->setOverlaysEnabled(true);
	}
	//-----------------------------------------------------------------------
	bool DepthMapTargetListener::renderableQueued(Ogre::Renderable* rend, Ogre::uint8 groupID, Ogre::ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue)
	{
		// Replace the technique of all renderables
		*ppTech = mDepthTechnique;
		return true;
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::registerRenderer(ParticleRenderer* renderer)
	{
		mRenderers.push_back(renderer);
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::unregisterRenderer(ParticleRenderer* renderer)
	{
		std::vector<ParticleRenderer*>::iterator it;
		std::vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			if (*it == renderer)
			{
				// Remove it
				mRenderers.erase(it);
				return;
			}
		}
	}
	//-----------------------------------------------------------------------
	bool DepthMapTargetListener::registeredRenderersEmpty(void)
	{
		return mRenderers.empty();
	}
	//-----------------------------------------------------------------------
	Ogre::String ParticleSystemFactory::PU_FACTORY_TYPE_NAME = "PUParticleSystem";
	//-----------------------------------------------------------------------
	Ogre::MovableObject* ParticleSystemFactory::createInstanceImpl( const Ogre::String& name, 
		const Ogre::NameValuePairList* params)
	{
		if (params != 0)
		{
			Ogre::NameValuePairList::const_iterator ni = params->find("templateName");
			if (ni != params->end())
			{
				Ogre::String templateName = ni->second;
				
				// Create using manager
				return ParticleSystemManager::getSingleton()._createSystemImpl(name, templateName);
			}
		}
		
		// Not template based, just create one with the given name
		return ParticleSystemManager::getSingleton()._createSystemImpl(name);
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystemFactory::getType(void) const
	{
		return PU_FACTORY_TYPE_NAME;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemFactory::destroyInstance(Ogre::MovableObject* obj)
	{
		// Use manager
		ParticleSystemManager::getSingleton()._destroySystemImpl(static_cast<ParticleSystem*>(obj));
	}

}
