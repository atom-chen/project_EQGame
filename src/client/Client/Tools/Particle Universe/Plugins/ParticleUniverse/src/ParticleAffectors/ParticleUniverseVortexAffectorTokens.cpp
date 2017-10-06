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
#include "ParticleAffectors/ParticleUniverseVortexAffector.h"
#include "ParticleAffectors/ParticleUniverseVortexAffectorTokens.h"
#include "Externs/ParticleUniverseVortexExtern.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String VortexAffectorTokens::KEYWORD_ROTATION_VECTOR = "vortex_aff_vector";
	const Ogre::String VortexAffectorTokens::KEYWORD_ROTATION_SPEED = "vortex_aff_speed";

	//-----------------------------------------------------------------------
	void VortexAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_VECTOR, &mRotationVectorToken);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_SPEED, &mRotationSpeedToken);
	}
	//-----------------------------------------------------------------------
	void VortexAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a VortexAffector
		const VortexAffector* affector = static_cast<const VortexAffector*>(element);

		// Write the header of the VortexAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getRotationVector() != VortexAffector::DEFAULT_ROTATION_VECTOR) serializer->writeLine(
			KEYWORD_ROTATION_VECTOR, Ogre::StringConverter::toString(affector->getRotationVector()), 12);
		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(affector->getRotationSpeed()) != VortexAffector::DEFAULT_ROTATION_SPEED)
		{
			serializer->setKeyword(KEYWORD_ROTATION_SPEED);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, affector->getRotationSpeed());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void VortexAffectorTokens::RotationVectorToken::parse(ParticleScriptDeserializer* deserializer)
	{
		/** The VortexExtern (mis)uses the scripting posibilities of the VortexAffector. That is why
			the validation is removed from this function (because mulitple sectionnames are possible now).
		*/
		Ogre::Vector3 rotationVector;
		deserializer->parseVector3(rotationVector);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			/** Explicit cast to VortexExtern* is needed apparently. If the section element is a VortexExtern 
				object and casted to a VortexAffector* and the gravity is set, it somehow looses this value.
				If the object later on is casted to VortexExtern* again, the gravity value is gone.
			*/
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			VortexExtern* vortexExtern = static_cast<VortexExtern*>(externObject);
			vortexExtern->setRotationVector(rotationVector);
		}
		else
		{
			VortexAffector* affector = static_cast<VortexAffector*>(deserializer->context.getCurrentSectionElement());
			affector->setRotationVector(rotationVector);
		}
	}
	//-----------------------------------------------------------------------
	void VortexAffectorTokens::RotationSpeedToken::parse(ParticleScriptDeserializer* deserializer)
	{
		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			VortexExtern* vortexExtern = static_cast<VortexExtern*>(externObject);
			vortexExtern->setRotationSpeed(deserializer->parseDynamicAttribute());
		}
		else
		{
			VortexAffector* affector = static_cast<VortexAffector*>(deserializer->context.getCurrentSectionElement());
			affector->setRotationSpeed(deserializer->parseDynamicAttribute());
		}
	}

}
