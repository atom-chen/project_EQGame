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
#include "Externs/ParticleUniverseGravityExtern.h"
#include "Externs/ParticleUniverseGravityExternTokens.h"
#include "ParticleAffectors/ParticleUniverseGravityAffector.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String GravityExternTokens::KEYWORD_GRAVITY = "gravity";

	//-----------------------------------------------------------------------
	void GravityExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a GravityExtern
		const Extern* externObject = static_cast<const Extern*>(element);
		const GravityExtern* gravityExtern = static_cast<const GravityExtern*>(externObject);
		serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ExternTokens::write(serializer, element);
		AttachableTokens::write(serializer, element);

		// Write own attributes
		if (gravityExtern->getGravity() != GravityAffector::DEFAULT_GRAVITY) serializer->writeLine(
			KEYWORD_GRAVITY, Ogre::StringConverter::toString(gravityExtern->getGravity()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
