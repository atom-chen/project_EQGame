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
#include "ParticleAffectors/ParticleUniverseTextureAnimator.h"
#include "ParticleAffectors/ParticleUniverseTextureAnimatorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String TextureAnimatorTokens::KEYWORD_TIME_STEP = "time_step_animation";
	const Ogre::String TextureAnimatorTokens::KEYWORD_TEXCOORDS_START = "texture_coords_start";
	const Ogre::String TextureAnimatorTokens::KEYWORD_TEXCOORDS_END = "texture_coords_end";
	const Ogre::String TextureAnimatorTokens::KEYWORD_ANIMATION_TYPE = "texture_animation_type";
	const Ogre::String TextureAnimatorTokens::KEYWORD_START_RANDOM = "texture_start_random";
	const Ogre::String TextureAnimatorTokens::KEYWORD_LOOP = "loop";
	const Ogre::String TextureAnimatorTokens::KEYWORD_UP_DOWN = "up_down";
	const Ogre::String TextureAnimatorTokens::KEYWORD_RANDOM = "random";

	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_TIME_STEP, &mTimeStepTextureAnimationToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_START, &mTextureCoordsStartToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_END, &mTextureCoordsEndToken);
		tokenRegister->addLexemeAction(KEYWORD_ANIMATION_TYPE, &mTextureAnimationTypeToken);
		tokenRegister->addLexemeAction(KEYWORD_START_RANDOM, &mTextureStartRandomToken);
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a TextureAnimator
		const TextureAnimator* affector = static_cast<const TextureAnimator*>(element);

		// Write the header of the TextureAnimator
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getAnimationTimeStep() != TextureAnimator::DEFAULT_TIME_STEP) serializer->writeLine(
			KEYWORD_TIME_STEP, Ogre::StringConverter::toString(affector->getAnimationTimeStep()), 12);
		if (affector->getTextureCoordsStart() != TextureAnimator::DEFAULT_TEXCOORDS_START) serializer->writeLine(
			KEYWORD_TEXCOORDS_START, Ogre::StringConverter::toString(affector->getTextureCoordsStart()), 12);
		if (affector->getTextureCoordsEnd() != TextureAnimator::DEFAULT_TEXCOORDS_END) serializer->writeLine(
			KEYWORD_TEXCOORDS_END, Ogre::StringConverter::toString(affector->getTextureCoordsEnd()), 12);
		if (affector->isStartRandom() != TextureAnimator::DEFAULT_START_RANDOM) serializer->writeLine(
			KEYWORD_START_RANDOM, Ogre::StringConverter::toString(affector->isStartRandom()), 12);

		if (affector->getTextureAnimationType() != TextureAnimator::DEFAULT_ANIMATION_TYPE) 
		{
			Ogre::String animationType = KEYWORD_LOOP;
			if (affector->getTextureAnimationType() == TextureAnimator::TAT_LOOP) animationType = KEYWORD_LOOP;
			else if (affector->getTextureAnimationType() == TextureAnimator::TAT_UP_DOWN) animationType = KEYWORD_UP_DOWN;
			else if (affector->getTextureAnimationType() == TextureAnimator::TAT_RANDOM) animationType = KEYWORD_RANDOM;
			serializer->writeLine(KEYWORD_ANIMATION_TYPE, animationType, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::TimeStepTextureAnimationToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureAnimatorTokens::TimeStepTextureAnimationToken::parse");
		TextureAnimator* affector = static_cast<TextureAnimator*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real animationTimeStep;
		deserializer->parseReal(animationTimeStep);
		affector->setAnimationTimeStep(animationTimeStep);
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::TextureCoordsStartToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureAnimatorTokens::TextureCoordsStartToken::parse");
		TextureAnimator* affector = static_cast<TextureAnimator*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint16 textureCoordsStart;
		deserializer->parseUint16(textureCoordsStart);
		affector->setTextureCoordsStart(textureCoordsStart);
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::TextureCoordsEndToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureAnimatorTokens::TextureCoordsEndToken::parse");
		TextureAnimator* affector = static_cast<TextureAnimator*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint16 textureCoordsEnd;
		deserializer->parseUint16(textureCoordsEnd);
		affector->setTextureCoordsEnd(textureCoordsEnd);
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::TextureAnimationTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureAnimatorTokens::TextureAnimationTypeToken::parse");
		TextureAnimator* affector = static_cast<TextureAnimator*>(deserializer->context.getCurrentSectionElement());
		Ogre::String textureAnimationType = deserializer->getNextTokenLabel();
		if (textureAnimationType == "loop")
		{
			affector->setTextureAnimationType(TextureAnimator::TAT_LOOP);
		}
		else if (textureAnimationType == "up_down")
		{
			affector->setTextureAnimationType(TextureAnimator::TAT_UP_DOWN);
		}
		else if (textureAnimationType == "random")
		{
			affector->setTextureAnimationType(TextureAnimator::TAT_RANDOM);
		}
	}
	//-----------------------------------------------------------------------
	void TextureAnimatorTokens::TextureStartRandomToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "TextureAnimatorTokens::TextureStartRandomToken::parse");
		TextureAnimator* affector = static_cast<TextureAnimator*>(deserializer->context.getCurrentSectionElement());
		bool startRandom;
		deserializer->parseBool(startRandom);
		affector->setStartRandom(startRandom);
	}

}
