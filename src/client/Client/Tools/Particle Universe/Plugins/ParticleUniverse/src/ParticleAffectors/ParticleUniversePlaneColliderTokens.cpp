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
#include "ParticleAffectors/ParticleUniversePlaneCollider.h"
#include "ParticleAffectors/ParticleUniversePlaneColliderTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String PlaneColliderTokens::KEYWORD_NORMAL = "plane_collider_normal";

	//-----------------------------------------------------------------------
	void PlaneColliderTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_NORMAL, &mNormalToken);
	}
	//-----------------------------------------------------------------------
	void PlaneColliderTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a PlaneCollider
		const PlaneCollider* affector = static_cast<const PlaneCollider*>(element);

		// Write the header of the PlaneCollider
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ColliderTokens::write(serializer, element);

		// Write own attributes
		if (affector->getNormal() != PlaneCollider::DEFAULT_NORMAL) serializer->writeLine(
			KEYWORD_NORMAL, Ogre::StringConverter::toString(affector->getNormal()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void PlaneColliderTokens::NormalToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "PlaneColliderTokens::NormalToken::parse");
		PlaneCollider* affector = static_cast<PlaneCollider*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 normal;
		deserializer->parseVector3(normal);
		affector->setNormal(normal);
	}

}
