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
#include "ParticleAffectors/ParticleUniverseScaleAffector.h"
#include "ParticleAffectors/ParticleUniverseScaleAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String ScaleAffectorTokens::KEYWORD_XYZ_SCALE = "xyz_scale";
	const Ogre::String ScaleAffectorTokens::KEYWORD_X_SCALE = "x_scale";
	const Ogre::String ScaleAffectorTokens::KEYWORD_Y_SCALE = "y_scale";
	const Ogre::String ScaleAffectorTokens::KEYWORD_Z_SCALE = "z_scale";

	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_XYZ_SCALE, &mScaleXYZToken);
		tokenRegister->addLexemeAction(KEYWORD_X_SCALE, &mScaleXToken);
		tokenRegister->addLexemeAction(KEYWORD_Y_SCALE, &mScaleYToken);
		tokenRegister->addLexemeAction(KEYWORD_Z_SCALE, &mScaleZToken);
	}
	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ScaleAffector
		const ScaleAffector* affector = static_cast<const ScaleAffector*>(element);

		// Write the header of the ScaleAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(affector->getDynScaleXYZ()) != ScaleAffector::DEFAULT_XYZ_SCALE)
		{
			serializer->setKeyword(KEYWORD_XYZ_SCALE);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, affector->getDynScaleXYZ());
		}
		if (dynamicAttributeFactory._getDefaultValue(affector->getDynScaleX()) != ScaleAffector::DEFAULT_X_SCALE)
		{
			serializer->setKeyword(KEYWORD_X_SCALE);
			dynamicAttributeFactory.write(serializer, affector->getDynScaleX());
		}
		if (dynamicAttributeFactory._getDefaultValue(affector->getDynScaleY()) != ScaleAffector::DEFAULT_Y_SCALE)
		{
			serializer->setKeyword(KEYWORD_Y_SCALE);
			dynamicAttributeFactory.write(serializer, affector->getDynScaleY());
		}
		if (dynamicAttributeFactory._getDefaultValue(affector->getDynScaleZ()) != ScaleAffector::DEFAULT_Z_SCALE)
		{
			serializer->setKeyword(KEYWORD_Z_SCALE);
			dynamicAttributeFactory.write(serializer, affector->getDynScaleZ());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::ScaleXYZToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ScaleAffectorTokens::ScaleXYZToken::parse");
		ScaleAffector* affector = static_cast<ScaleAffector*>(deserializer->context.getCurrentSectionElement());
		affector->setDynScaleXYZ(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::ScaleXToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ScaleAffectorTokens::ScaleXToken::parse");
		ScaleAffector* affector = static_cast<ScaleAffector*>(deserializer->context.getCurrentSectionElement());
		affector->setDynScaleX(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::ScaleYToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ScaleAffectorTokens::ScaleYToken::parse");
		ScaleAffector* affector = static_cast<ScaleAffector*>(deserializer->context.getCurrentSectionElement());
		affector->setDynScaleY(deserializer->parseDynamicAttribute());
	}
	//-----------------------------------------------------------------------
	void ScaleAffectorTokens::ScaleZToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "ScaleAffectorTokens::ScaleZToken::parse");
		ScaleAffector* affector = static_cast<ScaleAffector*>(deserializer->context.getCurrentSectionElement());
		affector->setDynScaleZ(deserializer->parseDynamicAttribute());
	}

}
