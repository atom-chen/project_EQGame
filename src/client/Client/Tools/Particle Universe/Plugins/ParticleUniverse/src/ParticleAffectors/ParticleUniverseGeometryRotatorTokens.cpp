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
#include "ParticleAffectors/ParticleUniverseGeometryRotator.h"
#include "ParticleAffectors/ParticleUniverseGeometryRotatorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String GeometryRotatorTokens::KEYWORD_USE_OWN_ROTATION = "geom_rot_use_own_rotation";
	const Ogre::String GeometryRotatorTokens::KEYWORD_ROTATION_SPEED = "geom_rot_rotation_speed";
	const Ogre::String GeometryRotatorTokens::KEYWORD_ROTATION_AXIS = "geom_rot_axis";

	//-----------------------------------------------------------------------
	void GeometryRotatorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_USE_OWN_ROTATION, &mUseOwnRotationSpeedToken);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_SPEED, &mRotationSpeed);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_AXIS, &mRotationAxis);
	}
	//-----------------------------------------------------------------------
	void GeometryRotatorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a GeometryRotator
		const GeometryRotator* affector = static_cast<const GeometryRotator*>(element);

		// Write the header of the GeometryRotator
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->useOwnRotationSpeed() != GeometryRotator::DEFAULT_USE_OWN) serializer->writeLine(
			KEYWORD_USE_OWN_ROTATION, Ogre::StringConverter::toString(affector->useOwnRotationSpeed()), 12);
		if (affector->getRotationAxis() != GeometryRotator::DEFAULT_ROTATION_AXIS) serializer->writeLine(
			KEYWORD_ROTATION_AXIS, Ogre::StringConverter::toString(affector->getRotationAxis()), 12);

		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(affector->getRotationSpeed()) != GeometryRotator::DEFAULT_ROTATION_SPEED)
		{
			serializer->setKeyword(KEYWORD_ROTATION_SPEED);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, affector->getRotationSpeed());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void GeometryRotatorTokens::UseOwnRotationSpeedToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "GeometryRotatorTokens::UseOwnRotationSpeedToken::parse");
		GeometryRotator* affector = static_cast<GeometryRotator*>(deserializer->context.getCurrentSectionElement());
		bool useOwnRotationSpeed;
		deserializer->parseBool(useOwnRotationSpeed);
		affector->setUseOwnRotationSpeed(useOwnRotationSpeed);
	}
	//-----------------------------------------------------------------------
	void GeometryRotatorTokens::RotationSpeed::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "GeometryRotatorTokens::RotationSpeed::parse");
		GeometryRotator* affector = static_cast<GeometryRotator*>(deserializer->context.getCurrentSectionElement());
		affector->setRotationSpeed(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void GeometryRotatorTokens::RotationAxis::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "GeometryRotatorTokens::InitialRotationVector::parse");
		GeometryRotator* affector = static_cast<GeometryRotator*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 rotationAxis;
		deserializer->parseVector3(rotationAxis);
		affector->setRotationAxis(rotationAxis);
	}

}
