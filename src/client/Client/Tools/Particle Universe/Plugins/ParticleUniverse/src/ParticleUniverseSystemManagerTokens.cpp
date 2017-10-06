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

#include "ParticleUniverseSystemManagerTokens.h"
#include "ParticleUniverseSystemManager.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_USE_ALIAS = "use_alias";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_ALIAS = "alias";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_SYSTEM = "system";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_ENABLED = "enabled";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_POSITION = "position";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_KEEP_LOCAL = "keep_local";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_CAMERA_DEPENDENCY = "camera_dependency";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_VISUAL_PARTICLE = "visual_particle";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_TECHNIQUE_PARTICLE = "technique_particle";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_EMITTER_PARTICLE = "emitter_particle";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_AFFECTOR_PARTICLE = "affector_particle";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_SYSTEM_PARTICLE = "system_particle";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_VISUAL_PARTICLE_COMPONENT = "visual_particle_component";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_TECHNIQUE_COMPONENT = "technique_component";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_EMITTER_COMPONENT = "emitter_component";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_AFFECTOR_COMPONENT = "affector_component";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_OBSERVER_COMPONENT = "observer_component";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_LESS_THAN = "less_than";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_GREATER_THAN = "greater_than";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_EQUALS = "equals";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_CAMERA_DISTANCE = "camera_distance";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_POINT = "point";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_BOX = "box";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_SPHERE = "sphere";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_PLANE = "plane";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_TRUE = "true";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_FALSE = "false";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_ON = "on";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_OFF = "off";
	const Ogre::String ParticleSystemManagerTokens::KEYWORD_STREAM = "<<";
		
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		// Register more or less generic tokens
		tokenRegister->addLexemeAction("{", &mOpenBraceToken);
		tokenRegister->addLexemeAction("}", &mCloseBraceToken);
		tokenRegister->addLexemeAction(KEYWORD_USE_ALIAS, &mUseAliasToken);
		tokenRegister->addLexemeAction(KEYWORD_ALIAS, &mAliasToken);
		tokenRegister->addLexemeAction(KEYWORD_SYSTEM, &mSystemToken);
		tokenRegister->addLexemeAction(KEYWORD_ENABLED, &mEnabledToken);
		tokenRegister->addLexemeAction(KEYWORD_POSITION, &mPositionToken);
		tokenRegister->addLexemeAction(KEYWORD_KEEP_LOCAL, &mKeepLocalToken);
		tokenRegister->addLexemeAction(KEYWORD_CAMERA_DEPENDENCY, &mCameraDependencyToken);

		// Register particle tokens
		tokenRegister->addTokenId(KEYWORD_VISUAL_PARTICLE, ITokenRegister::ID_PT_VISUAL);
		tokenRegister->addTokenId(KEYWORD_TECHNIQUE_PARTICLE, ITokenRegister::ID_PT_TECHNIQUE);
		tokenRegister->addTokenId(KEYWORD_EMITTER_PARTICLE, ITokenRegister::ID_PT_EMITTER);
		tokenRegister->addTokenId(KEYWORD_AFFECTOR_PARTICLE, ITokenRegister::ID_PT_AFFECTOR);
		tokenRegister->addTokenId(KEYWORD_SYSTEM_PARTICLE, ITokenRegister::ID_PT_SYSTEM);

		// Register component tokens
		tokenRegister->addTokenId(KEYWORD_VISUAL_PARTICLE_COMPONENT, ITokenRegister::ID_CT_VISUAL_PARTICLE);
		tokenRegister->addTokenId(KEYWORD_TECHNIQUE_COMPONENT, ITokenRegister::ID_CT_TECHNIQUE);
		tokenRegister->addTokenId(KEYWORD_EMITTER_COMPONENT, ITokenRegister::ID_CT_EMITTER);
		tokenRegister->addTokenId(KEYWORD_AFFECTOR_COMPONENT, ITokenRegister::ID_CT_AFFECTOR);
		tokenRegister->addTokenId(KEYWORD_OBSERVER_COMPONENT, ITokenRegister::ID_CT_OBSERVER);

		// Comparison operators
		tokenRegister->addTokenId(KEYWORD_LESS_THAN, ITokenRegister::ID_COMPARE_LESS_THAN);
		tokenRegister->addTokenId(KEYWORD_GREATER_THAN, ITokenRegister::ID_COMPARE_GREATER_THAN);
		tokenRegister->addTokenId(KEYWORD_EQUALS, ITokenRegister::ID_COMPARE_EQUALS);

		// Register generic tokens
		tokenRegister->addTokenId(KEYWORD_CAMERA_DISTANCE, ITokenRegister::ID_CAMERA_DISTANCE);
		tokenRegister->addTokenId(KEYWORD_POINT, ITokenRegister::ID_SHAPE_POINT);
		tokenRegister->addTokenId(KEYWORD_BOX, ITokenRegister::ID_SHAPE_BOX);
		tokenRegister->addTokenId(KEYWORD_SPHERE, ITokenRegister::ID_SHAPE_SPHERE);
		tokenRegister->addTokenId(KEYWORD_PLANE, ITokenRegister::ID_SHAPE_PLANE);
		tokenRegister->addTokenId(KEYWORD_TRUE, ITokenRegister::ID_TRUE);
		tokenRegister->addTokenId(KEYWORD_FALSE, ITokenRegister::ID_FALSE);
		tokenRegister->addTokenId(KEYWORD_ON, ITokenRegister::ID_ON);
		tokenRegister->addTokenId(KEYWORD_OFF, ITokenRegister::ID_OFF);
		tokenRegister->addTokenId(KEYWORD_STREAM, ITokenRegister::ID_STREAM);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::OpenBraceToken::parse(ParticleScriptDeserializer* deserializer)
	{
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::CloseBraceToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.endSection();
		if (deserializer->context.getCurrentSectionName() == ALIAS)
		{
			/** Creation of the former object was part of an alias section, so it needs to be stored as an alias 
				template.
			*/

 			IAlias* alias = 0;
			if (deserializer->context.getPreviousSectionName() == TECHNIQUE)
			{
				alias = static_cast<ParticleTechnique*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == RENDERER)
			{
				alias = static_cast<ParticleRenderer*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == EMITTER)
			{
				alias = static_cast<ParticleEmitter*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == AFFECTOR)
			{
				alias = static_cast<ParticleAffector*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == OBSERVER)
			{
				alias = static_cast<ParticleObserver*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == HANDLER)
			{
				alias = static_cast<ParticleEventHandler*>(deserializer->context.getPreviousSectionElement());
			}
			else if (deserializer->context.getPreviousSectionName() == BEHAVIOUR)
			{
				alias = static_cast<ParticleBehaviour*>(deserializer->context.getPreviousSectionElement());
			}
			else
			{
				Ogre::LogManager::getSingleton().logMessage("ParticleUniverse: Unknown alias. Error while assigning alias in CloseBraceToken::parse.");
				return; // Alias of unknown type
			}

			alias->setAliasName(deserializer->context.getCurrentSectionElementName());
			ParticleSystemManager* particleSystemManager = ParticleSystemManager::getSingletonPtr();
			particleSystemManager->addAlias(alias);
		}
		else if (deserializer->context.getPreviousSectionName() == DYNAMIC_ATTRIBUTE)
		{
			// Perform post-parsing actions: Process the controlpoints if the section was a Dynamic Attribute section
			if (deserializer->context.getPreviousSectionElement() && (static_cast<DynamicAttribute*>(deserializer->context.getPreviousSectionElement()))->getType() == DynamicAttribute::DAT_CURVED)
			{
				// Process controlpoints
				DynamicAttributeCurved* dynAttr = static_cast<DynamicAttributeCurved*>(deserializer->context.getPreviousSectionElement());
				dynAttr->processControlPoints();
			}
		}
		else if (deserializer->context.attributeToken)
		{
			// Perform post-parsing actions only if the attributeToken has been set in the context
			deserializer->context.attributeToken->postParse(deserializer);
			deserializer->context.attributeToken = 0;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::AliasToken::parse(ParticleScriptDeserializer* deserializer)
	{
		Ogre::String aliasName = deserializer->getNextTokenLabel();

		// Set the next context to be an alias and store the name in the context (we still want to use it)
		deserializer->context.beginSection(ALIAS, 0, aliasName);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::UseAliasToken::parse(ParticleScriptDeserializer* deserializer)
	{
		IAlias* alias = ParticleSystemManager::getSingletonPtr()->getAlias (deserializer->getNextTokenLabel());
		switch (alias->getAliasType())
		{
			case IAlias::AT_RENDERER:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				ParticleRenderer* renderer = static_cast<ParticleRenderer*>(alias);
				ParticleRenderer* newRenderer = ParticleSystemManager::getSingletonPtr()->cloneRenderer(renderer);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->setRenderer(newRenderer);
			}
			break;

			case IAlias::AT_TECHNIQUE:
			{
				if (deserializer->context.getCurrentSectionName() != SYSTEM)
					return;

				ParticleTechnique* technique = static_cast<ParticleTechnique*>(alias);
				ParticleTechnique* newTechnique = ParticleSystemManager::getSingletonPtr()->cloneTechnique(technique);
				ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
				system->addTechnique(newTechnique);
			}
			break;

			case IAlias::AT_EMITTER:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				ParticleEmitter* emitter = static_cast<ParticleEmitter*>(alias);
				ParticleEmitter* newEmitter = ParticleSystemManager::getSingletonPtr()->cloneEmitter(emitter);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->addEmitter(newEmitter);
			}
			break;

			case IAlias::AT_AFFECTOR:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				ParticleAffector* affector = static_cast<ParticleAffector*>(alias);
				ParticleAffector* newAffector = ParticleSystemManager::getSingletonPtr()->cloneAffector(affector);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->addAffector(newAffector);
			}
			break;

			case IAlias::AT_OBSERVER:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				ParticleObserver* observer = static_cast<ParticleObserver*>(alias);
				ParticleObserver* newObserver = ParticleSystemManager::getSingletonPtr()->cloneObserver(observer);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->addObserver(newObserver);
			}
			break;

			case IAlias::AT_HANDLER:
			{
				if (deserializer->context.getCurrentSectionName() != OBSERVER)
					return;

				ParticleEventHandler* handler = static_cast<ParticleEventHandler*>(alias);
				ParticleEventHandler* newHandler = ParticleSystemManager::getSingletonPtr()->cloneEventHandler(handler);
				ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
				observer->addEventHandler(newHandler);
			}
			break;

			case IAlias::AT_EXTERN:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				Extern* externObject = static_cast<Extern*>(alias);
				Extern* newExternObject = ParticleSystemManager::getSingletonPtr()->cloneExtern(externObject);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->addExtern(newExternObject);
			}
			break;

			case IAlias::AT_BEHAVIOUR:
			{
				if (deserializer->context.getCurrentSectionName() != TECHNIQUE)
					return;

				ParticleBehaviour* behaviour = static_cast<ParticleBehaviour*>(alias);
				ParticleBehaviour* newBehaviour = ParticleSystemManager::getSingletonPtr()->cloneBehaviour(behaviour);
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
				technique->_addBehaviourTemplate(newBehaviour);
			}
			break;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::SystemToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create a particle system
		Ogre::String systemName = deserializer->getNextTokenLabel();
		ParticleSystem* system = ParticleSystemManager::getSingletonPtr()->createParticleSystemTemplate(systemName, deserializer->getGroupName());

		// Set the next context to be a system
		deserializer->context.beginSection(SYSTEM, system);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::EnabledToken::parse(ParticleScriptDeserializer* deserializer)
	{
		bool enabled;
		deserializer->parseBool(enabled);

		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->setEnabled(enabled);

		}
		else if (deserializer->context.getCurrentSectionName() == EMITTER)
		{
			ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
			emitter->setEnabled(enabled);
		}
		else if (deserializer->context.getCurrentSectionName() == AFFECTOR)
		{
			ParticleAffector* affector = static_cast<ParticleAffector*>(deserializer->context.getCurrentSectionElement());
			affector->setEnabled(enabled);
		}
		else if (deserializer->context.getCurrentSectionName() == OBSERVER)
		{
			ParticleObserver* observer = static_cast<ParticleObserver*>(deserializer->context.getCurrentSectionElement());
			observer->setEnabled(enabled);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::PositionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		Ogre::Vector3 position;
		deserializer->parseVector3(position);

		if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->position = position;
			technique->originalPosition = position;

		}
		else if (deserializer->context.getCurrentSectionName() == EMITTER)
		{
			ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
			emitter->position = position;
			emitter->originalPosition = position;
		}
		else if (deserializer->context.getCurrentSectionName() == AFFECTOR)
		{
			ParticleAffector* affector = static_cast<ParticleAffector*>(deserializer->context.getCurrentSectionElement());
			affector->position = position;
			affector->originalPosition = position;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::KeepLocalToken::parse(ParticleScriptDeserializer* deserializer)
	{
		bool keepLocal;
		deserializer->parseBool(keepLocal);

		if (deserializer->context.getCurrentSectionName() == EMITTER)
		{
			ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
			emitter->setKeepLocal(keepLocal);
		}
		else if (deserializer->context.getCurrentSectionName() == SYSTEM)
		{
			ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
			system->setKeepLocal(keepLocal);
		}
		else if (deserializer->context.getCurrentSectionName() == TECHNIQUE)
		{
			ParticleTechnique* technique = static_cast<ParticleTechnique*>(deserializer->context.getCurrentSectionElement());
			technique->setKeepLocal(keepLocal);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManagerTokens::CameraDependencyToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Set the next context to be a dependency
		CameraDependency* dependency = ParticleSystemManager::getSingletonPtr()->createCameraDependency();
		deserializer->context.beginSection(DEPENDENCY, dependency);
	}
}
