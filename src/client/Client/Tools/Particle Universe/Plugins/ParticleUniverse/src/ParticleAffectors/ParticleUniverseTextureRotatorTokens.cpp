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
#include "ParticleAffectors/ParticleUniverseTextureRotator.h"
#include "ParticleAffectors/ParticleUniverseTextureRotatorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String TextureRotatorTokens::KEYWORD_USE_OWN_ROTATION = "tex_rot_use_own_rotation";
	const Ogre::String TextureRotatorTokens::KEYWORD_ROTATION_SPEED = "tex_rot_speed";
	const Ogre::String TextureRotatorTokens::KEYWORD_ROTATION = "tex_rot_rotation";

	//-----------------------------------------------------------------------
	void TextureRotatorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_USE_OWN_ROTATION, &mUseOwnRotationSpeedToken);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_SPEED, &mRotationSpeed);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION, &mRotation);
	}
	//-----------------------------------------------------------------------
	void TextureRotatorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a TextureRotator
		const TextureRotator* affector = static_cast<const TextureRotator*>(element);

		// Write the header of the TextureRotator
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->useOwnRotationSpeed() != TextureRotator::DEFAULT_USE_OWN_SPEED) serializer->writeLine(
			KEYWORD_USE_OWN_ROTATION, Ogre::StringConverter::toString(affector->useOwnRotationSpeed()), 12);
		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(affector->getRotation()) != TextureRotator::DEFAULT_ROTATION)
		{
			serializer->setKeyword(KEYWORD_ROTATION);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, affector->getRotation());
		}
		if (dynamicAttributeFactory._getDefaultValue(affector->getRotationSpeed()) != TextureRotator::DEFAULT_ROTATION_SPEED)
		{
			serializer->setKeyword(KEYWORD_ROTATION_SPEED);
			dynamicAttributeFactory.write(serializer, affector->getRotationSpeed());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void TextureRotatorTokens::UseOwnRotationSpeedToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureRotatorTokens::UseOwnRotationSpeedToken::parse");
		TextureRotator* affector = static_cast<TextureRotator*>(deserializer->context.getCurrentSectionElement());
		bool useOwnRotationSpeed;
		deserializer->parseBool(useOwnRotationSpeed);
		affector->setUseOwnRotationSpeed(useOwnRotationSpeed);
	}
	//-----------------------------------------------------------------------
	void TextureRotatorTokens::Rotation::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureRotatorTokens::Rotation::parse");
		TextureRotator* affector = static_cast<TextureRotator*>(deserializer->context.getCurrentSectionElement());
		affector->setRotation(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void TextureRotatorTokens::RotationSpeed::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureRotatorTokens::RotationSpeed::parse");
		TextureRotator* affector = static_cast<TextureRotator*>(deserializer->context.getCurrentSectionElement());
		affector->setRotationSpeed(deserializer->parseDynamicAttribute());
	}

}
