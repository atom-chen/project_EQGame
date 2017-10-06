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
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseSystemTokens.h"
#include "ParticleUniverseSystemManager.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleSystemTokens::KEYWORD_SYSTEM = "system";
	const Ogre::String ParticleSystemTokens::KEYWORD_TECHNIQUE = "technique";
	const Ogre::String ParticleSystemTokens::KEYWORD_KEEP_LOCAL = "keep_local";
	const Ogre::String ParticleSystemTokens::KEYWORD_ITERATION_INTERVAL = "iteration_interval";
	const Ogre::String ParticleSystemTokens::KEYWORD_FIXED_TIMEOUT = "fixed_timeout";
	const Ogre::String ParticleSystemTokens::KEYWORD_NONVIS_UPDATE_TIMEOUT = "nonvisible_update_timeout";
	const Ogre::String ParticleSystemTokens::KEYWORD_LOD_DISTANCES = "lod_distances";
	const Ogre::String ParticleSystemTokens::KEYWORD_SMOOTH_LOD = "smooth_lod";
	const Ogre::String ParticleSystemTokens::KEYWORD_FAST_FORWARD = "fast_forward";
	const Ogre::String ParticleSystemTokens::KEYWORD_MAIN_CAMERA_NAME = "main_camera_name";
	const Ogre::String ParticleSystemTokens::KEYWORD_SCALE_VELOCITY = "scale_velocity";
	const Ogre::String ParticleSystemTokens::KEYWORD_SCALE_TIME = "scale_time";
	const Ogre::String ParticleSystemTokens::KEYWORD_SCALE = "scale";
	const Ogre::String ParticleSystemTokens::KEYWORD_TIGHT_BOUNDING_BOX = "tight_bounding_box";
	
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_TECHNIQUE, &mTechniqueToken);
		tokenRegister->addLexemeAction(KEYWORD_ITERATION_INTERVAL, &mIterationIntervalToken);
		tokenRegister->addLexemeAction(KEYWORD_FIXED_TIMEOUT, &mFixedTimeoutToken);
		tokenRegister->addLexemeAction(KEYWORD_NONVIS_UPDATE_TIMEOUT, &mNonVisibleUpdateTimeoutToken);
		tokenRegister->addLexemeAction(KEYWORD_LOD_DISTANCES, &mLodDistancesToken);
		tokenRegister->addLexemeAction(KEYWORD_SMOOTH_LOD, &mSmoothLodToken);
		tokenRegister->addLexemeAction(KEYWORD_FAST_FORWARD, &mFastForwardToken);
		tokenRegister->addLexemeAction(KEYWORD_MAIN_CAMERA_NAME, &mMainCameraNameToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE_VELOCITY, &mScaleVelocityToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE_TIME, &mScaleTimeToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE, &mScaleToken);
		tokenRegister->addLexemeAction(KEYWORD_TIGHT_BOUNDING_BOX, &mTightBoundingBoxToken);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleSystem
		const ParticleSystem* system = static_cast<const ParticleSystem*>(element);

		// Write the header of the ParticleSystem
		Ogre::String name = system->getTemplateName();
		if (name.empty())
		{
			name = system->getName();
		}
		serializer->writeLine(KEYWORD_SYSTEM, name, 0, 1);
		serializer->writeLine("{");

		// Write attributes (only the changed ones)
		if (system->isKeepLocal() != ParticleSystem::DEFAULT_KEEP_LOCAL) serializer->writeLine(
			KEYWORD_KEEP_LOCAL, Ogre::StringConverter::toString(system->isKeepLocal()), 4);
		if (system->getFixedTimeout() != ParticleSystem::DEFAULT_FIXED_TIMEOUT) serializer->writeLine(
			KEYWORD_FIXED_TIMEOUT, Ogre::StringConverter::toString(system->getFixedTimeout()), 4);
		if (system->getIterationInterval() != ParticleSystem::DEFAULT_ITERATION_INTERVAL) serializer->writeLine(
			KEYWORD_ITERATION_INTERVAL, Ogre::StringConverter::toString(system->getIterationInterval()), 4);
		if (system->getNonVisibleUpdateTimeout() != ParticleSystem::DEFAULT_NON_VISIBLE_UPDATE_TIMEOUT) serializer->writeLine(
			KEYWORD_NONVIS_UPDATE_TIMEOUT, Ogre::StringConverter::toString(system->getNonVisibleUpdateTimeout()), 4);
		if (system->isSmoothLod() != ParticleSystem::DEFAULT_SMOOTH_LOD) serializer->writeLine(
			KEYWORD_SMOOTH_LOD, Ogre::StringConverter::toString(system->isSmoothLod()), 4);
		if (system->getFastForwardTime() != ParticleSystem::DEFAULT_FAST_FORWARD_TIME ||
			system->getFastForwardInterval() != 0.0f)
		{
			serializer->writeLine(KEYWORD_FAST_FORWARD, Ogre::StringConverter::toString(system->getFastForwardTime()) + " " +
				Ogre::StringConverter::toString(system->getFastForwardInterval()), 4);
		}
		if (system->getMainCameraName() != ParticleSystem::DEFAULT_MAIN_CAMERA_NAME) serializer->writeLine(
			KEYWORD_MAIN_CAMERA_NAME, system->getMainCameraName(), 4);
		if (system->getScaleVelocity() != ParticleSystem::DEFAULT_SCALE_VELOCITY) serializer->writeLine(
			KEYWORD_SCALE_VELOCITY, Ogre::StringConverter::toString(system->getScaleVelocity()), 4);
		if (system->getScaleTime() != ParticleSystem::DEFAULT_SCALE_TIME) serializer->writeLine(
			KEYWORD_SCALE_TIME, Ogre::StringConverter::toString(system->getScaleTime()), 4);
		if (system->getScale() != ParticleSystem::DEFAULT_SCALE) serializer->writeLine(
			KEYWORD_SCALE, Ogre::StringConverter::toString(system->getScale()), 4);
		if (system->hasTightBoundingBox() != ParticleSystem::DEFAULT_TIGHT_BOUNDINGBOX) serializer->writeLine(
			KEYWORD_TIGHT_BOUNDING_BOX, Ogre::StringConverter::toString(system->hasTightBoundingBox()), 4);

		// Write lod distances
		if (!(system->getLodDistances()).empty())
		{
			serializer->writeLine(
				KEYWORD_LOD_DISTANCES, ParticleScriptSerializer::toString(system->getLodDistances(), true), 4);
		}

		// Continue with the techniques
		size_t numerOfTechnique = system->getNumTechniques();
		serializer->context.beginSection(TECHNIQUE, 0);
		for (size_t i = 0; i < numerOfTechnique; ++i)
		{
			mParticleTechniqueTokens.write(serializer , system->getTechnique(i));
		}
		serializer->context.endSection();

		// Write the close bracket
		serializer->writeLine("}");
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::TechniqueToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// Create the technique
		ParticleTechnique* technique = ParticleSystemManager::getSingletonPtr()->createTechnique();
		if (deserializer->context.getCurrentSectionName() == SYSTEM)
		{
			ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
			system->addTechnique(technique);
		}

		// Set the next context to be a technique
		Ogre::String techniqueName = Ogre::StringUtil::BLANK;
		const size_t paramCount = deserializer->getRemainingTokensForAction();
		if (paramCount == 1)
		{
			techniqueName = deserializer->getNextTokenLabel();
		}

		technique->setName(techniqueName);
		technique->suppressNotifyEmissionChange(true); // Don't notify if emitters are added
		deserializer->context.beginSection(TECHNIQUE, technique);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::FixedTimeoutToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "FixedTimeoutToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real fixedTimeout = 0;
		deserializer->parseReal(fixedTimeout);
		system->setFixedTimeout(fixedTimeout);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::IterationIntervalToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "IterationIntervalToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real iterationInterval = 0;
		deserializer->parseReal(iterationInterval);
		system->setIterationInterval(iterationInterval);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::NonVisibleUpdateTimeoutToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "NonVisibleUpdateTimeoutToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real nonVisibleUpdateTimeout = 0;
		deserializer->parseReal(nonVisibleUpdateTimeout);
		system->setNonVisibleUpdateTimeout(nonVisibleUpdateTimeout);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::LodDistancesToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "LodDistancesToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		ParticleSystem::LodDistanceList lodDistances;

		while (deserializer->getRemainingTokensForAction() > 0)
		{
            lodDistances.push_back(deserializer->getNextTokenValue());
        }

		system->setLodDistances(lodDistances);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::SmoothLodToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "SmoothLodToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		bool smoothLod;
		deserializer->parseBool(smoothLod);
		system->setSmoothLod(smoothLod);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::FastForwardToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "FastForwardToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real time = 0;
		Ogre::Real interval = 0;
		deserializer->parseReal(time);
		deserializer->parseReal(interval);
		system->setFastForward(time, interval);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::MainCameraNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "MainCameraNameToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		system->setMainCameraName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::ScaleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "ScaleToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 scale;
		deserializer->parseVector3(scale);
		system->setScale(scale);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::ScaleVelocityToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "ScaleVelocityToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real scaleVelocity = 0;
		deserializer->parseReal(scaleVelocity);
		system->setScaleVelocity(scaleVelocity);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::ScaleTimeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "TimeScaleToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real scaleTime = 0;
		deserializer->parseReal(scaleTime);
		system->setScaleTime(scaleTime);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemTokens::TightBoundingBoxToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(SYSTEM, "TightBoundingBoxToken::parse");
		ParticleSystem* system = static_cast<ParticleSystem*>(deserializer->context.getCurrentSectionElement());
		bool tightBoundingBox = false;
		deserializer->parseBool(tightBoundingBox);
		system->setTightBoundingBox(tightBoundingBox);
	}
}
