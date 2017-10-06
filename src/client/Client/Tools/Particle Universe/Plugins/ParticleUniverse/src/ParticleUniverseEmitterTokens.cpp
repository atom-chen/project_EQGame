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
#include "ParticleUniverseScriptSerializer.h"
#include "ParticleUniverseEmitterTokens.h"
#include "ParticleUniverseCameraDependency.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleEmitterTokens::KEYWORD_EMITTER = "emitter";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ENABLED = "enabled";
	const Ogre::String ParticleEmitterTokens::KEYWORD_POSITION = "position";
	const Ogre::String ParticleEmitterTokens::KEYWORD_KEEP_LOCAL = "keep_local";
	const Ogre::String ParticleEmitterTokens::KEYWORD_DIRECTION = "direction";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ORIENTATION = "orientation";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ORIENTATION_RANGE_START = "range_start_orientation";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ORIENTATION_RANGE_END = "range_end_orientation";
	const Ogre::String ParticleEmitterTokens::KEYWORD_VELOCITY = "velocity";
	const Ogre::String ParticleEmitterTokens::KEYWORD_DURATION = "duration";
	const Ogre::String ParticleEmitterTokens::KEYWORD_REPEAT_DELAY = "repeat_delay";
	const Ogre::String ParticleEmitterTokens::KEYWORD_EMITS = "emits";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ANGLE = "angle";
	const Ogre::String ParticleEmitterTokens::KEYWORD_EMISSION_RATE = "emission_rate";
	const Ogre::String ParticleEmitterTokens::KEYWORD_TIME_TO_LIVE = "time_to_live";
	const Ogre::String ParticleEmitterTokens::KEYWORD_MASS = "mass";
	const Ogre::String ParticleEmitterTokens::KEYWORD_START_TEXCOORDS = "start_texture_coords";
	const Ogre::String ParticleEmitterTokens::KEYWORD_END_TEXCOORDS = "end_texture_coords";
	const Ogre::String ParticleEmitterTokens::KEYWORD_TEXCOORDS = "texture_coords";
	const Ogre::String ParticleEmitterTokens::KEYWORD_START_COLOUR_RANGE = "start_colour_range";
	const Ogre::String ParticleEmitterTokens::KEYWORD_END_COLOUR_RANGE = "end_colour_range";
	const Ogre::String ParticleEmitterTokens::KEYWORD_COLOUR = "colour";
	const Ogre::String ParticleEmitterTokens::KEYWORD_ALL_PARTICLE_DIM = "all_particle_dimensions";
	const Ogre::String ParticleEmitterTokens::KEYWORD_PARTICLE_WIDTH = "particle_width";
	const Ogre::String ParticleEmitterTokens::KEYWORD_PARTICLE_HEIGHT = "particle_height";
	const Ogre::String ParticleEmitterTokens::KEYWORD_PARTICLE_DEPTH = "particle_depth";
	const Ogre::String ParticleEmitterTokens::KEYWORD_AUTO_DIRECTION = "auto_direction";
	const Ogre::String ParticleEmitterTokens::KEYWORD_FORCE_EMISISON = "force_emission";
	const Ogre::String ParticleEmitterTokens::KEYWORD_EMITTER_PARTICLE = "emitter_particle";
	const Ogre::String ParticleEmitterTokens::KEYWORD_AFFECTOR_PARTICLE = "affector_particle";
	const Ogre::String ParticleEmitterTokens::KEYWORD_TECHNIQUE_PARTICLE = "technique_particle";
	const Ogre::String ParticleEmitterTokens::KEYWORD_SYSTEM_PARTICLE = "system_particle";

	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_DIRECTION, &mDirectionToken);
		tokenRegister->addLexemeAction(KEYWORD_ORIENTATION_RANGE_START, &mOrientationRangeStartToken);
		tokenRegister->addLexemeAction(KEYWORD_ORIENTATION_RANGE_END, &mOrientationRangeEndToken);
		tokenRegister->addLexemeAction(KEYWORD_ORIENTATION, &mOrientationToken);
		tokenRegister->addLexemeAction(KEYWORD_VELOCITY, &mVelocityToken);
		tokenRegister->addLexemeAction(KEYWORD_DURATION, &mDurationToken);
		tokenRegister->addLexemeAction(KEYWORD_REPEAT_DELAY, &mRepeatDelayToken);
		tokenRegister->addLexemeAction(KEYWORD_EMITS, &mEmitsToken);
		tokenRegister->addLexemeAction(KEYWORD_ANGLE, &mAngleToken);
		tokenRegister->addLexemeAction(KEYWORD_EMISSION_RATE, &mEmissionRateToken);
		tokenRegister->addLexemeAction(KEYWORD_TIME_TO_LIVE, &mTimeToLiveToken);
		tokenRegister->addLexemeAction(KEYWORD_MASS, &mMassToken);
		tokenRegister->addLexemeAction(KEYWORD_START_TEXCOORDS, &mTextureCoordsRangeStartToken);
		tokenRegister->addLexemeAction(KEYWORD_END_TEXCOORDS, &mTextureCoordsRangeEndToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS, &mTextureCoordsToken);
		tokenRegister->addLexemeAction(KEYWORD_START_COLOUR_RANGE, &mColourRangeStartToken);
		tokenRegister->addLexemeAction(KEYWORD_END_COLOUR_RANGE, &mColourRangeEndToken);
		tokenRegister->addLexemeAction(KEYWORD_COLOUR, &mColourToken);
		tokenRegister->addLexemeAction(KEYWORD_ALL_PARTICLE_DIM, &mParticleAllDimensionsToken);
		tokenRegister->addLexemeAction(KEYWORD_PARTICLE_WIDTH, &mParticleWidthToken);
		tokenRegister->addLexemeAction(KEYWORD_PARTICLE_HEIGHT, &mParticleHeightToken);
		tokenRegister->addLexemeAction(KEYWORD_PARTICLE_DEPTH, &mParticleDepthToken);
		tokenRegister->addLexemeAction(KEYWORD_AUTO_DIRECTION, &mAutoDirectionToken);
		tokenRegister->addLexemeAction(KEYWORD_FORCE_EMISISON, &mForceEmissionToken);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleEmitter
		const ParticleEmitter* emitter = static_cast<const ParticleEmitter*>(element);

		// Write own attributes
		DynamicAttributeFactory dynamicAttributeFactory;

		if (emitter->getDynEmissionRate()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynEmissionRate()->getValue() != ParticleEmitter::DEFAULT_EMISSION_RATE)
		{
			serializer->setKeyword(KEYWORD_EMISSION_RATE);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, emitter->getDynEmissionRate());
		}

		if (emitter->getDynAngle()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynAngle()->getValue() != ParticleEmitter::DEFAULT_ANGLE)
		{
			serializer->setKeyword(KEYWORD_ANGLE);
			dynamicAttributeFactory.write(serializer, emitter->getDynAngle());
		}

		if (emitter->getDynTotalTimeToLive()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynTotalTimeToLive()->getValue() != ParticleEmitter::DEFAULT_TIME_TO_LIVE)
		{
			serializer->setKeyword(KEYWORD_TIME_TO_LIVE);
			dynamicAttributeFactory.write(serializer, emitter->getDynTotalTimeToLive());
		}

		if (emitter->getDynParticleMass()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynParticleMass()->getValue() != ParticleEmitter::DEFAULT_MASS)
		{
			serializer->setKeyword(KEYWORD_MASS);
			dynamicAttributeFactory.write(serializer, emitter->getDynParticleMass());
		}

		if (emitter->getDynVelocity()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynVelocity()->getValue() != ParticleEmitter::DEFAULT_VELOCITY)
		{
			serializer->setKeyword(KEYWORD_VELOCITY);
			dynamicAttributeFactory.write(serializer, emitter->getDynVelocity());
		}

		if (emitter->getDynDuration()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynDuration()->getValue() != ParticleEmitter::DEFAULT_DURATION)
		{
			serializer->setKeyword(KEYWORD_DURATION);
			dynamicAttributeFactory.write(serializer, emitter->getDynDuration());
		}

		if (emitter->getDynRepeatDelay()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynRepeatDelay()->getValue() != ParticleEmitter::DEFAULT_REPEAT_DELAY)
		{
			serializer->setKeyword(KEYWORD_REPEAT_DELAY);
			dynamicAttributeFactory.write(serializer, emitter->getDynRepeatDelay());
		}

		if (emitter->getDynParticleAllDimensions()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynParticleAllDimensions()->getValue() != ParticleEmitter::DEFAULT_DIMENSIONS)
		{
			serializer->setKeyword(KEYWORD_ALL_PARTICLE_DIM);
			dynamicAttributeFactory.write(serializer, emitter->getDynParticleAllDimensions());
		}

		if (emitter->getDynParticleWidth()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynParticleWidth()->getValue() != ParticleEmitter::DEFAULT_WIDTH)
		{
			serializer->setKeyword(KEYWORD_PARTICLE_WIDTH);
			dynamicAttributeFactory.write(serializer, emitter->getDynParticleWidth());
		}

		if (emitter->getDynParticleHeight()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynParticleHeight()->getValue() != ParticleEmitter::DEFAULT_HEIGHT)
		{
			serializer->setKeyword(KEYWORD_PARTICLE_HEIGHT);
			dynamicAttributeFactory.write(serializer, emitter->getDynParticleHeight());
		}

		if (emitter->getDynParticleDepth()->getType() != DynamicAttribute::DAT_FIXED || 
			emitter->getDynParticleDepth()->getValue() != ParticleEmitter::DEFAULT_DEPTH)
		{
			serializer->setKeyword(KEYWORD_PARTICLE_DEPTH);
			dynamicAttributeFactory.write(serializer, emitter->getDynParticleDepth());
		}

		if (emitter->_getOriginalEnabled() != ParticleEmitter::DEFAULT_ENABLED) serializer->writeLine(
			KEYWORD_ENABLED, Ogre::StringConverter::toString(emitter->_getOriginalEnabled()), 12);
		if (emitter->position != ParticleEmitter::DEFAULT_POSITION) serializer->writeLine(
			KEYWORD_POSITION, Ogre::StringConverter::toString(emitter->position), 12);
		if (emitter->isKeepLocal() != ParticleEmitter::DEFAULT_KEEP_LOCAL) serializer->writeLine(
			KEYWORD_KEEP_LOCAL, Ogre::StringConverter::toString(emitter->isKeepLocal()), 12);
		if (emitter->getOriginalParticleDirection() != ParticleEmitter::DEFAULT_DIRECTION) serializer->writeLine(
			KEYWORD_DIRECTION, Ogre::StringConverter::toString(emitter->getOriginalParticleDirection()), 12);
		if (emitter->getParticleOrientation() != ParticleEmitter::DEFAULT_ORIENTATION) serializer->writeLine(
			KEYWORD_ORIENTATION, Ogre::StringConverter::toString(emitter->getParticleOrientation()), 12);
		if (emitter->getParticleOrientationRangeStart() != ParticleEmitter::DEFAULT_ORIENTATION_RANGE_START) serializer->writeLine(
			KEYWORD_ORIENTATION_RANGE_START, Ogre::StringConverter::toString(emitter->getParticleOrientationRangeStart()), 12);
		if (emitter->getParticleOrientationRangeEnd() != ParticleEmitter::DEFAULT_ORIENTATION_RANGE_END) serializer->writeLine(
			KEYWORD_ORIENTATION_RANGE_END, Ogre::StringConverter::toString(emitter->getParticleOrientationRangeEnd()), 12);
		if (emitter->getEmitsType() != ParticleEmitter::DEFAULT_EMITS)
		{
			Ogre::String particle = KEYWORD_EMITTER_PARTICLE;
			if (emitter->getEmitsType() == Particle::PT_AFFECTOR) particle = KEYWORD_AFFECTOR_PARTICLE;
			else if (emitter->getEmitsType() == Particle::PT_TECHNIQUE) particle = KEYWORD_TECHNIQUE_PARTICLE;
			else if (emitter->getEmitsType() == Particle::PT_SYSTEM) particle = KEYWORD_SYSTEM_PARTICLE;
			serializer->writeLine(KEYWORD_EMITS, particle, emitter->getEmitsName(), 12);
		}
		if (emitter->getParticleTextureCoordsRangeStart() != ParticleEmitter::DEFAULT_START_TEXTURE_COORDS) serializer->writeLine(
			KEYWORD_START_TEXCOORDS, Ogre::StringConverter::toString(emitter->getParticleTextureCoordsRangeStart()), 12);
		if (emitter->getParticleTextureCoordsRangeEnd() != ParticleEmitter::DEFAULT_END_TEXTURE_COORDS) serializer->writeLine(
			KEYWORD_END_TEXCOORDS, Ogre::StringConverter::toString(emitter->getParticleTextureCoordsRangeEnd()), 12);
		if (emitter->getParticleTextureCoords() != ParticleEmitter::DEFAULT_TEXTURE_COORDS) serializer->writeLine(
			KEYWORD_TEXCOORDS, Ogre::StringConverter::toString(emitter->getParticleTextureCoords()), 12);
		if (emitter->getParticleColourRangeStart() != ParticleEmitter::DEFAULT_START_COLOUR_RANGE) serializer->writeLine(
			KEYWORD_START_COLOUR_RANGE, Ogre::StringConverter::toString(emitter->getParticleColourRangeStart()), 12);
		if (emitter->getParticleColourRangeEnd() != ParticleEmitter::DEFAULT_END_COLOUR_RANGE) serializer->writeLine(
			KEYWORD_END_COLOUR_RANGE, Ogre::StringConverter::toString(emitter->getParticleColourRangeEnd()), 12);
		if (emitter->getParticleColour() != ParticleEmitter::DEFAULT_COLOUR) serializer->writeLine(
			KEYWORD_COLOUR, Ogre::StringConverter::toString(emitter->getParticleColour()), 12);
		if (emitter->isAutoDirection() != ParticleEmitter::DEFAULT_AUTO_DIRECTION) serializer->writeLine(
			KEYWORD_AUTO_DIRECTION, Ogre::StringConverter::toString(emitter->isAutoDirection()), 12);
		if (emitter->isForceEmission() != ParticleEmitter::DEFAULT_FORCE_EMISSION) serializer->writeLine(
			KEYWORD_FORCE_EMISISON, Ogre::StringConverter::toString(emitter->isForceEmission()), 12);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::DirectionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "DirectionToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 direction;
		deserializer->parseVector3(direction);
		emitter->setParticleDirection(direction);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::OrientationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "OrientationToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Quaternion orientation;
		deserializer->parseQuaternion(orientation);
		emitter->setParticleOrientation(orientation);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::OrientationRangeStartToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "OrientationRangeStartToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Quaternion orientationRangeStart;
		deserializer->parseQuaternion(orientationRangeStart);
		emitter->setParticleOrientationRangeStart(orientationRangeStart);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::OrientationRangeEndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "OrientationRangeEndToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Quaternion orientationRangeEnd;
		deserializer->parseQuaternion(orientationRangeEnd);
		emitter->setParticleOrientationRangeEnd(orientationRangeEnd);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::VelocityToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "VelocityToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynVelocity(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::DurationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "DurationToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynDuration(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::RepeatDelayToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "RepeatDelayToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynRepeatDelay(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::EmitsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "EmitsToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());

		if (deserializer->testNextTokenID(ITokenRegister::ID_PT_EMITTER))
		{
			emitter->setEmitsType(Particle::PT_EMITTER);
			deserializer->getNextTokenID();
			emitter->setEmitsName(deserializer->getNextTokenLabel());
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_AFFECTOR))
		{
			emitter->setEmitsType(Particle::PT_AFFECTOR);
			deserializer->getNextTokenID();
			emitter->setEmitsName(deserializer->getNextTokenLabel());
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_SYSTEM))
		{
			emitter->setEmitsType(Particle::PT_SYSTEM);
			deserializer->getNextTokenID();
			emitter->setEmitsName(deserializer->getNextTokenLabel());
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_TECHNIQUE))
		{
			emitter->setEmitsType(Particle::PT_TECHNIQUE);
			deserializer->getNextTokenID();
			emitter->setEmitsName(deserializer->getNextTokenLabel());
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_PT_VISUAL))
		{
			deserializer->getNextTokenID();
			emitter->setEmitsType(Particle::PT_VISUAL);
			emitter->setEmitsName(Ogre::StringUtil::BLANK);
		}
		else 
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "invalid particle type.", 
		            "EmitsToken::parse");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::AngleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "AngleToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynAngle(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::EmissionRateToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "EmissionRateToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynEmissionRate(deserializer->parseDynamicAttribute());

		if (deserializer->testNextTokenID(ITokenRegister::ID_STREAM))
		{
			// Set 'this' token for processing the camera dependency in a later stage
			deserializer->context.attributeToken = this;
			deserializer->getNextTokenID();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::EmissionRateToken::postParse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "EmissionRateToken::postParse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());

		// Get the dependency (= camera dependency) and copy its attributes
		CameraDependency* cameraDependency = static_cast<CameraDependency*>(deserializer->context.getPreviousSectionElement());
		if (cameraDependency)
		{
			Ogre::Real threshold = cameraDependency->getThreshold();
			emitter->setEmissionRateCameraDependency(threshold * threshold, cameraDependency->isIncrease());
		}
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::TimeToLiveToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "TimeToLiveToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynTotalTimeToLive(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::MassToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "MassToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynParticleMass(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ParticleAllDimensionsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ParticleAllDimensionsToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynParticleAllDimensions(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ParticleWidthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ParticleWidthToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynParticleWidth(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ParticleHeightToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ParticleHeightToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynParticleHeight(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ParticleDepthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ParticleDepthToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setDynParticleDepth(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::AutoDirectionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "AutoDirectionToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		bool autoDirection;
		deserializer->parseBool(autoDirection);
		emitter->setAutoDirection(autoDirection);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ForceEmissionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ForceEmissionToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		bool forceEmission;
		deserializer->parseBool(forceEmission);
		emitter->setForceEmission(forceEmission);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ColourToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		emitter->setParticleColour(colour);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ColourRangeStartToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ColourRangeStartToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		emitter->setParticleColourRangeStart(colour);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::ColourRangeEndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "ColourRangeEndToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		emitter->setParticleColourRangeEnd(colour);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::TextureCoordsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "TextureCoordsToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint16 textureCoords;
		deserializer->parseUint16(textureCoords);
		emitter->setParticleTextureCoords(textureCoords);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::TextureCoordsRangeStartToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "TextureCoordsRangeStartToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint16 textureCoords;
		deserializer->parseUint16(textureCoords);
		emitter->setParticleTextureCoordsRangeStart(textureCoords);
	}
	//-----------------------------------------------------------------------
	void ParticleEmitterTokens::TextureCoordsRangeEndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "TextureCoordsRangeEndToken::parse");
		ParticleEmitter* emitter = static_cast<ParticleEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint16 textureCoords;
		deserializer->parseUint16(textureCoords);
		emitter->setParticleTextureCoordsRangeEnd(textureCoords);
	}

}
