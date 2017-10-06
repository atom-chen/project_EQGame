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
#include "ParticleAffectors/ParticleUniverseSphereCollider.h"
#include "ParticleAffectors/ParticleUniverseSphereColliderTokens.h"
#include "Externs/ParticleUniverseSphereColliderExtern.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SphereColliderTokens::KEYWORD_RADIUS = "sphere_collider_radius";

	//-----------------------------------------------------------------------
	void SphereColliderTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RADIUS, &mRadiusToken);
	}
	//-----------------------------------------------------------------------
	void SphereColliderTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SphereCollider
		const SphereCollider* affector = static_cast<const SphereCollider*>(element);

		// Write the header of the SphereCollider
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ColliderTokens::write(serializer, element);

		// Write own attributes
		if (affector->getRadius() != SphereCollider::DEFAULT_RADIUS) serializer->writeLine(
			KEYWORD_RADIUS, Ogre::StringConverter::toString(affector->getRadius()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void SphereColliderTokens::RadiusToken::parse(ParticleScriptDeserializer* deserializer)
	{
		/** The SphereColliderExtern uses the scripting posibilities of the SphereCollider. That is why
			the validation is removed from this function (because mulitple sectionnames are possible now).
		*/
		Ogre::Real radius;
		deserializer->parseReal(radius);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			SphereColliderExtern* sphereColliderExtern = static_cast<SphereColliderExtern*>(externObject);
			sphereColliderExtern->setRadius(radius);
		}
		else
		{
			SphereCollider* affector = static_cast<SphereCollider*>(deserializer->context.getCurrentSectionElement());
			affector->setRadius(radius);
		}
	}

}
