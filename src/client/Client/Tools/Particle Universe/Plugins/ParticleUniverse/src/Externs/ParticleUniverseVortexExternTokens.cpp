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

#include "ParticleUniverseScriptSerializer.h"
#include "Externs/ParticleUniverseVortexExtern.h"
#include "Externs/ParticleUniverseVortexExternTokens.h"
#include "ParticleAffectors/ParticleUniverseVortexAffector.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String VortexExternTokens::KEYWORD_ROTATION_VECTOR = "vortex_aff_vector";
	const Ogre::String VortexExternTokens::KEYWORD_ROTATION_SPEED = "vortex_aff_speed";

	//-----------------------------------------------------------------------
	void VortexExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a VortexExtern
		const Extern* externObject = static_cast<const Extern*>(element);
		const VortexExtern* vortexExtern = static_cast<const VortexExtern*>(externObject);
		serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ExternTokens::write(serializer, element);
		AttachableTokens::write(serializer, element);

		// Write own attributes
		if (vortexExtern->getRotationVector() != VortexAffector::DEFAULT_ROTATION_VECTOR) serializer->writeLine(
			KEYWORD_ROTATION_VECTOR, Ogre::StringConverter::toString(vortexExtern->getRotationVector()), 12);
		DynamicAttributeFactory dynamicAttributeFactory;
		if (dynamicAttributeFactory._getDefaultValue(vortexExtern->getRotationSpeed()) != VortexAffector::DEFAULT_ROTATION_SPEED)
		{
			serializer->setKeyword(KEYWORD_ROTATION_SPEED);
			serializer->setIndentation(12);
			dynamicAttributeFactory.write(serializer, vortexExtern->getRotationSpeed());
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
