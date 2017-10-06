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

#include "ParticleUniverseScriptDeserializer.h"
#include "ParticleUniverseTechniqueTokens.h"
#include "ParticleUniverseCameraDependency.h"
#include "ParticleUniverseSystemManager.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleTechniqueTokens::KEYWORD_ENABLED = "enabled";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_POSITION = "position";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_KEEP_LOCAL = "keep_local";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_TECHNIQUE = "technique";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_RENDERER = "renderer";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EMITTER = "emitter";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_AFFECTOR = "affector";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_OBSERVER = "observer";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_HANDLER = "handler";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_BEHAVIOUR = "behaviour";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EXTERN = "extern";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_VISUAL_PARTICLE_QUOTA = "visual_particle_quota";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EMITTED_EMITTER_QUOTA = "emitted_emitter_quota";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EMITTED_AFFECTOR_QUOTA = "emitted_affector_quota";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EMITTED_TECHNIQUE_QUOTA = "emitted_technique_quota";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_EMITTED_SYSTEM_QUOTA = "emitted_system_quota";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_MATERIAL = "material";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_LOD_INDEX = "lod_index";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_DEFAULT_PARTICLE_WIDTH = "default_particle_width";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_DEFAULT_PARTICLE_HEIGHT = "default_particle_height";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_DEFAULT_PARTICLE_DEPTH = "default_particle_depth";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_SPHASHING_CELL_DIMENSION = "spatial_hashing_cell_dimension";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_SPHASHING_CELL_OVERLAP = "spatial_hashing_cell_overlap";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_SPHASHING_SIZE = "spatial_hashtable_size";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_SPHASHING_UPDATE_INTERVAL = "spatial_hashing_update_interval";
	const Ogre::String ParticleTechniqueTokens::KEYWORD_MAX_VELOCITY = "max_velocity";

	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RENDERER, &mRendererToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITTER, &mEmitterToken);
		tokenRegister->addLexemeAction(KEYWORD_AFFECTOR, &mAffectorToken);
		tokenRegister->addLexemeAction(KEYWORD_OBSERVER, &mObserverToken);
		tokenRegister->addLexemeAction(KEYWORD_HANDLER, &mEventHandlerToken);
		tokenRegister->addLexemeAction(KEYWORD_BEHAVIOUR, &mBehaviourToken);
		tokenRegister->addLexemeAction(KEYWORD_EXTERN, &mExternToken);
		tokenRegister->addLexemeAction(KEYWORD_VISUAL_PARTICLE_QUOTA, &mVisualParticleQuotaToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITTED_EMITTER_QUOTA, &mEmittedEmitterQuotaToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITTED_TECHNIQUE_QUOTA, &mEmittedTechniqueQuotaToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITTED_AFFECTOR_QUOTA, &mEmittedAffectorQuotaToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITTED_SYSTEM_QUOTA, &mEmittedSystemQuotaToken);
		tokenRegister->addLexemeAction(KEYWORD_MATERIAL, &mMaterialToken);
		tokenRegister->addLexemeAction(KEYWORD_LOD_INDEX, &mLodIndexToken);
		tokenRegister->addLexemeAction(KEYWORD_DEFAULT_PARTICLE_WIDTH, &mWidthToken);
		tokenRegister->addLexemeAction(KEYWORD_DEFAULT_PARTICLE_HEIGHT, &mHeightToken);
		tokenRegister->addLexemeAction(KEYWORD_DEFAULT_PARTICLE_DEPTH, &mDepthToken);
		tokenRegister->addLexemeAction(KEYWORD_SPHASHING_CELL_DIMENSION, &mSpatialHashingCellDimensionToken);
		tokenRegister->addLexemeAction(KEYWORD_SPHASHING_CELL_OVERLAP, &mSpatialHashingCellOverlapToken);
		tokenRegister->addLexemeAction(KEYWORD_SPHASHING_SIZE, &mSpatialHashTableSizeToken);
		tokenRegister->addLexemeAction(KEYWORD_SPHASHING_UPDATE_INTERVAL, &mSpatialHashingUpdateIntervalToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_VELOCITY, &mMaxVelocityToken);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleTechnique
		const ParticleTechnique* technique = static_cast<const ParticleTechnique*>(element);

		// Write the header of the ParticleTechnique
		serializer->writeLine(KEYWORD_TECHNIQUE, technique->getName(), 4, 1);
		serializer->writeLine("{", 4);

		// Write attributes
		if (technique->_getOriginalEnabled() != ParticleTechnique::DEFAULT_ENABLED) serializer->writeLine(
			KEYWORD_ENABLED, Ogre::StringConverter::toString(technique->_getOriginalEnabled()), 8);
		if (technique->position != ParticleTechnique::DEFAULT_POSITION) serializer->writeLine(
			KEYWORD_POSITION, Ogre::StringConverter::toString(technique->position), 8);
		if (technique->isKeepLocal() != ParticleTechnique::DEFAULT_KEEP_LOCAL) serializer->writeLine(
			KEYWORD_KEEP_LOCAL, Ogre::StringConverter::toString(technique->isKeepLocal()), 8);
		if (technique->getVisualParticleQuota() != ParticleTechnique::DEFAULT_VISUAL_PARTICLE_QUOTA) serializer->writeLine(
			KEYWORD_VISUAL_PARTICLE_QUOTA, Ogre::StringConverter::toString(technique->getVisualParticleQuota()), 8);
		if (technique->getEmittedEmitterQuota() != ParticleTechnique::DEFAULT_EMITTED_EMITTER_QUOTA) serializer->writeLine(
			KEYWORD_EMITTED_EMITTER_QUOTA, Ogre::StringConverter::toString(technique->getEmittedEmitterQuota()), 8);
		if (technique->getEmittedTechniqueQuota() != ParticleTechnique::DEFAULT_EMITTED_TECHNIQUE_QUOTA) serializer->writeLine(
			KEYWORD_EMITTED_TECHNIQUE_QUOTA, Ogre::StringConverter::toString(technique->getEmittedTechniqueQuota()), 8);
		if (technique->getEmittedAffectorQuota() != ParticleTechnique::DEFAULT_EMITTED_AFFECTOR_QUOTA) serializer->writeLine(
			KEYWORD_EMITTED_AFFECTOR_QUOTA, Ogre::StringConverter::toString(technique->getEmittedAffectorQuota()), 8);
		if (technique->getEmittedSystemQuota() != ParticleTechnique::DEFAULT_EMITTED_SYSTEM_QUOTA) serializer->writeLine(
			KEYWORD_EMITTED_SYSTEM_QUOTA, Ogre::StringConverter::toString(technique->getEmittedSystemQuota()), 8);
		serializer->writeLine(KEYWORD_MATERIAL, technique->getMaterialName(), 8);
		if (technique->getLodIndex() != ParticleTechnique::DEFAULT_LOD_INDEX) serializer->writeLine(
			KEYWORD_LOD_INDEX, Ogre::StringConverter::toString(technique->getLodIndex()), 8);
		if (technique->getDefaultWidth() != ParticleTechnique::DEFAULT_DEFAULT_WIDTH) serializer->writeLine(
			KEYWORD_DEFAULT_PARTICLE_WIDTH, Ogre::StringConverter::toString(technique->getDefaultWidth()), 8);
		if (technique->getDefaultHeight() != ParticleTechnique::DEFAULT_DEFAULT_HEIGHT) serializer->writeLine(
			KEYWORD_DEFAULT_PARTICLE_HEIGHT, Ogre::StringConverter::toString(technique->getDefaultHeight()), 8);
		if (technique->getDefaultDepth() != ParticleTechnique::DEFAULT_DEFAULT_DEPTH) serializer->writeLine(
			KEYWORD_DEFAULT_PARTICLE_DEPTH, Ogre::StringConverter::toString(technique->getDefaultDepth()), 8);
		if (technique->getSpatialHashingCellDimension() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_CELL_DIM) serializer->writeLine(
			KEYWORD_SPHASHING_CELL_DIMENSION, Ogre::StringConverter::toString(technique->getSpatialHashingCellDimension()), 8);
		if (technique->getSpatialHashingCellOverlap() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_CELL_OVERLAP) serializer->writeLine(
			KEYWORD_SPHASHING_CELL_OVERLAP, Ogre::StringConverter::toString(technique->getSpatialHashingCellOverlap()), 8);
		if (technique->getSpatialHashTableSize() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_TABLE_SIZE) serializer->writeLine(
			KEYWORD_SPHASHING_SIZE, Ogre::StringConverter::toString(technique->getSpatialHashTableSize()), 8);
		if (technique->getSpatialHashingInterval() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_INTERVAL) serializer->writeLine(
			KEYWORD_SPHASHING_UPDATE_INTERVAL, Ogre::StringConverter::toString(technique->getSpatialHashingInterval()), 8);
		if (technique->getMaxVelocity() != ParticleTechnique::DEFAULT_MAX_VELOCITY) serializer->writeLine(
			KEYWORD_MAX_VELOCITY, Ogre::StringConverter::toString(technique->getMaxVelocity()), 8);

		// Write renderer
		serializer->context.beginSection(RENDERER, 0); // Don´t set the renderer
		ParticleRenderer* renderer = technique->getRenderer();
		ParticleRendererFactory* particleRendererFactory = ParticleSystemManager::getSingletonPtr()->getRendererFactory(renderer->getRendererType());
		if (particleRendererFactory)
		{
			particleRendererFactory->write(serializer, renderer);
		}
		serializer->context.endSection();

		// Write emitters
		serializer->context.beginSection(EMITTER, 0); // Don´t set the emitter
		size_t numberOfEmitters = technique->getNumEmitters();
		for (size_t i = 0; i < numberOfEmitters; ++i)
		{
			ParticleEmitter* emitter = technique->getEmitter(i);
			ParticleEmitterFactory* particleEmitterFactory = ParticleSystemManager::getSingletonPtr()->getEmitterFactory(emitter->getEmitterType());
			if (particleEmitterFactory)
			{
				particleEmitterFactory->write(serializer, emitter);
			}
		}
		serializer->context.endSection();

		// Write affectors
		serializer->context.beginSection(AFFECTOR, 0); // Don´t set the affector
		size_t numberOfAffectors = technique->getNumAffectors();
		for (size_t i = 0; i < numberOfAffectors; ++i)
		{
			ParticleAffector* affector = technique->getAffector(i);
			ParticleAffectorFactory* particleAffectorFactory = ParticleSystemManager::getSingletonPtr()->getAffectorFactory(affector->getAffectorType());
			if (particleAffectorFactory)
			{
				particleAffectorFactory->write(serializer, affector);
			}
		}
		serializer->context.endSection();

		// Write observers
		serializer->context.beginSection(OBSERVER, 0); // Don´t set the observer
		size_t numberOfObservers = technique->getNumObservers();
		for (size_t i = 0; i < numberOfObservers; ++i)
		{
			ParticleObserver* observer = technique->getObserver(i);
			ParticleObserverFactory* particleObserverFactory = ParticleSystemManager::getSingletonPtr()->getObserverFactory(observer->getObserverType());
			if (particleObserverFactory)
			{
				particleObserverFactory->write(serializer, observer);
			}
		}
		serializer->context.endSection();

		// Write behaviours
		serializer->context.beginSection(BEHAVIOUR, 0); // Don´t set the behaviour
		size_t numberOfBehaviours = technique->_getNumBehaviourTemplates();
		for (size_t i = 0; i < numberOfBehaviours; ++i)
		{
			ParticleBehaviour* behaviour = technique->_getBehaviourTemplate(i);
			ParticleBehaviourFactory* particleBehaviourFactory = ParticleSystemManager::getSingletonPtr()->getBehaviourFactory(behaviour->getBehaviourType());
			if (particleBehaviourFactory)
			{
				particleBehaviourFactory->write(serializer, behaviour);
			}
		}
		serializer->context.endSection();

		// Write externs
		serializer->context.beginSection(EXTERN, 0); // Don´t set the extern
		size_t numberOfExterns = technique->getNumExterns();
		for (size_t i = 0; i < numberOfExterns; ++i)
		{
			Extern* externObject = technique->getExtern(i);
			ExternFactory* externFactory = ParticleSystemManager::getSingletonPtr()->getExternFactory(externObject->getExternType());
			if (externFactory)
			{
				externFactory->write(serializer, externObject);
			}
		}
		serializer->context.endSection();

		// Write the close bracket
		serializer->writeLine("}", 4);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::RendererToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create a Renderer
		Ogre::String rendererType = deserializer->getNextTokenLabel();
		ParticleRenderer* renderer = ParticleSystemManager::getSingletonPtr()->createRenderer(rendererType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->setRenderer(renderer);
		}

		// Set the next context to be a renderer
		deserializer->context.beginSection(RENDERER, renderer);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EmitterToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create an emitter
		Ogre::String emitterName = Ogre::StringUtil::BLANK;
		ParticleEmitter* emitter = 0;

		Ogre::String emitterType = deserializer->getNextTokenLabel();
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			emitterName = deserializer->getNextTokenLabel();
		}

		emitter = ParticleSystemManager::getSingletonPtr()->createEmitter(emitterType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->addEmitter(emitter);
		}

		// Set the next context to be an emitter
		emitter->setName(emitterName);
		deserializer->context.beginSection(EMITTER, emitter);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::AffectorToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create an affector
		Ogre::String affectorName = Ogre::StringUtil::BLANK;
		ParticleAffector* affector = 0;

		Ogre::String affectorType = deserializer->getNextTokenLabel();
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			affectorName = deserializer->getNextTokenLabel();
		}

		affector = ParticleSystemManager::getSingletonPtr()->createAffector(affectorType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->addAffector(affector);
		}

		// Set the next context to be an affector
		affector->setName(affectorName);
		deserializer->context.beginSection(AFFECTOR, affector);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::ObserverToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create an observer
		Ogre::String observerName = Ogre::StringUtil::BLANK;
		ParticleObserver* observer = 0;
		Ogre::String observerType = deserializer->getNextTokenLabel();
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			observerName = deserializer->getNextTokenLabel();
		}

		observer = ParticleSystemManager::getSingletonPtr()->createObserver(observerType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->addObserver(observer);
		}

		// Set the next context to be an observer
		observer->setName(observerName);
		deserializer->context.beginSection(OBSERVER, observer);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EventHandlerToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create an event handler
		Ogre::String eventHandlerName = Ogre::StringUtil::BLANK;
		ParticleEventHandler* eventHandler = 0;
		Ogre::String eventHandlerType = deserializer->getNextTokenLabel();
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			eventHandlerName = deserializer->getNextTokenLabel();
		}

		eventHandler = ParticleSystemManager::getSingletonPtr()->createEventHandler(eventHandlerType);
		if (deserializer->context.getCurrentSectionName() == OBSERVER)
		{
			ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
			observer->addEventHandler(eventHandler);
		}

		// Set the next context to be an event handler
		eventHandler->setName(eventHandlerName);
		deserializer->context.beginSection(HANDLER, eventHandler);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::BehaviourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create a behaviour
		ParticleBehaviour* behaviour = 0;
		Ogre::String behaviourType = deserializer->getNextTokenLabel();
		behaviour = ParticleSystemManager::getSingletonPtr()->createBehaviour(behaviourType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->_addBehaviourTemplate(behaviour);
		}

		// Set the next context to be a behaviour
		deserializer->context.beginSection(BEHAVIOUR, behaviour);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::ExternToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create an external object
		Extern* externObject = 0;
		Ogre::String externName = Ogre::StringUtil::BLANK;
		Ogre::String externType = deserializer->getNextTokenLabel();
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			externName = deserializer->getNextTokenLabel();
		}
		externObject = ParticleSystemManager::getSingletonPtr()->createExtern(externType);
		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->addExtern(externObject);
		}

		// Set the next context to be an extern object
		externObject->setName(externName);
		deserializer->context.beginSection(EXTERN, externObject);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::VisualParticleQuotaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "VisualParticleQuotaToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quota;
		deserializer->parseReal(quota);
		technique->setVisualParticleQuota(quota);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EmittedEmitterQuotaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "EmittedEmitterQuotaToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quota;
		deserializer->parseReal(quota);
		technique->setEmittedEmitterQuota(quota);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EmittedTechniqueQuotaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "EmittedTechniqueQuotaToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quota;
		deserializer->parseReal(quota);
		technique->setEmittedTechniqueQuota(quota);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EmittedAffectorQuotaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "EmittedAffectorQuotaToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quota;
		deserializer->parseReal(quota);
		technique->setEmittedAffectorQuota(quota);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::EmittedSystemQuotaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "EmittedSystemQuotaToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quota;
		deserializer->parseReal(quota);
		technique->setEmittedSystemQuota(quota);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::MaterialToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "MaterialToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		technique->setMaterialName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::LodIndexToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "LodIndexToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real lodIndex;
		deserializer->parseReal(lodIndex);
		technique->setLodIndex(lodIndex);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::WidthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "WidthToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real width = 0;
		deserializer->parseReal(width);
		technique->setDefaultWidth(width);

		if (deserializer->testNextTokenID(ITokenRegister::ID_STREAM))
		{
			// Set 'this' token for processing the camera dependency in a later stage
			deserializer->context.attributeToken = this;
			deserializer->getNextTokenID();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::WidthToken::postParse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "WidthToken::postParse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		
		// Get the dependency (= camera dependency) and copy its attributes
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getPreviousSectionElement());
		if (cameraDependency)
		{
			Ogre::Real threshold = cameraDependency->getThreshold();
			bool increase = cameraDependency->isIncrease();
			technique->setWidthCameraDependency(threshold * threshold, increase);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::HeightToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "HeightToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real height = 0;
		deserializer->parseReal(height);
		technique->setDefaultHeight(height);

		if (deserializer->testNextTokenID(ITokenRegister::ID_STREAM))
		{
			// Set 'this' token for processing the camera dependency in a later stage
			deserializer->context.attributeToken = this;
			deserializer->getNextTokenID();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::HeightToken::postParse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "HeightToken::postParse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		
		// Get the dependency (= camera dependency) and copy its attributes
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getPreviousSectionElement());
		if (cameraDependency)
		{
			Ogre::Real threshold = cameraDependency->getThreshold();
			bool increase = cameraDependency->isIncrease();
			technique->setHeightCameraDependency(threshold * threshold, increase);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::DepthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "DepthToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real depth = 0;
		deserializer->parseReal(depth);
		technique->setDefaultDepth(depth);

		if (deserializer->testNextTokenID(ITokenRegister::ID_STREAM))
		{
			// Set 'this' token for processing the camera dependency in a later stage
			deserializer->context.attributeToken = this;
			deserializer->getNextTokenID();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::DepthToken::postParse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "DepthToken::postParse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		
		// Get the dependency (= camera dependency) and copy its attributes
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getPreviousSectionElement());
		if (cameraDependency)
		{
			Ogre::Real threshold = cameraDependency->getThreshold();
			bool increase = cameraDependency->isIncrease();
			technique->setDepthCameraDependency(threshold * threshold, increase);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::SpatialHashingCellDimensionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "ParticleTechniqueTokens::SpatialHashingCellDimensionToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		unsigned short cellDimension;
		deserializer->parseUshort(cellDimension);
		technique->setSpatialHashingCellDimension(cellDimension);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::SpatialHashingCellOverlapToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "ParticleTechniqueTokens::SpatialHashingCellOverlapToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		unsigned short cellOverlap;
		deserializer->parseUshort(cellOverlap);
		technique->setSpatialHashingCellOverlap(cellOverlap);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::SpatialHashTableSizeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "ParticleTechniqueTokens::SpatialHashTableSizeToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		unsigned int hashtableSize;
		deserializer->parseUint(hashtableSize);
		technique->setSpatialHashTableSize(hashtableSize);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::SpatialHashingUpdateIntervalToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "ParticleTechniqueTokens::SpatialHashingUpdateIntervalToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real updateInterval;
		deserializer->parseReal(updateInterval);
		technique->setSpatialHashingInterval(updateInterval);
	}
	//-----------------------------------------------------------------------
	void ParticleTechniqueTokens::MaxVelocityToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(TECHNIQUE, "ParticleTechniqueTokens::MaxVelocityToken::parse");
		ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real maxVelocity;
		deserializer->parseReal(maxVelocity);
		technique->setMaxVelocity(maxVelocity);
	}

}
