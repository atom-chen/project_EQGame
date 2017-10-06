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
#include "ParticleAffectors/ParticleUniverseBoxCollider.h"
#include "ParticleAffectors/ParticleUniverseBoxColliderTokens.h"
#include "Externs/ParticleUniverseBoxColliderExtern.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String BoxColliderTokens::KEYWORD_WIDTH = "box_collider_width";
	const Ogre::String BoxColliderTokens::KEYWORD_HEIGHT = "box_collider_height";
	const Ogre::String BoxColliderTokens::KEYWORD_DEPTH = "box_collider_depth";

	//-----------------------------------------------------------------------
	void BoxColliderTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_WIDTH, &mWidthToken);
		tokenRegister->addLexemeAction(KEYWORD_HEIGHT, &mHeightToken);
		tokenRegister->addLexemeAction(KEYWORD_DEPTH, &mDepthToken);
	}
	//-----------------------------------------------------------------------
	void BoxColliderTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a BoxCollider
		const BoxCollider* affector = static_cast<const BoxCollider*>(element);

		// Write the header of the BoxCollider
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ColliderTokens::write(serializer, element);

		// Write own attributes
		if (affector->getWidth() != BoxCollider::DEFAULT_WIDTH) serializer->writeLine(
			KEYWORD_WIDTH, Ogre::StringConverter::toString(affector->getWidth()), 12);
		if (affector->getHeight() != BoxCollider::DEFAULT_HEIGHT) serializer->writeLine(
			KEYWORD_HEIGHT, Ogre::StringConverter::toString(affector->getHeight()), 12);
		if (affector->getDepth() != BoxCollider::DEFAULT_DEPTH) serializer->writeLine(
			KEYWORD_DEPTH, Ogre::StringConverter::toString(affector->getDepth()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void BoxColliderTokens::WidthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// The BoxColliderExtern uses the scripting posibilities of the BoxCollider.
		Ogre::Real width;
		deserializer->parseReal(width);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			BoxColliderExtern* boxColliderExtern = static_cast<BoxColliderExtern*>(externObject);
			boxColliderExtern->setWidth(width);
		}
		else
		{
			BoxCollider* affector = static_cast<BoxCollider*>(deserializer->context.getCurrentSectionElement());
			affector->setWidth(width);
		}
	}
	//-----------------------------------------------------------------------
	void BoxColliderTokens::HeightToken::parse(ParticleScriptDeserializer* deserializer)
	{
		Ogre::Real height;
		deserializer->parseReal(height);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			BoxColliderExtern* boxColliderExtern = static_cast<BoxColliderExtern*>(externObject);
			boxColliderExtern->setHeight(height);
		}
		else
		{
			BoxCollider* affector = static_cast<BoxCollider*>(deserializer->context.getCurrentSectionElement());
			affector->setHeight(height);
		}
	}
	//-----------------------------------------------------------------------
	void BoxColliderTokens::DepthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		Ogre::Real depth;
		deserializer->parseReal(depth);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			BoxColliderExtern* boxColliderExtern = static_cast<BoxColliderExtern*>(externObject);
			boxColliderExtern->setDepth(depth);
		}
		else
		{
			BoxCollider* affector = static_cast<BoxCollider*>(deserializer->context.getCurrentSectionElement());
			affector->setDepth(depth);
		}
	}

}
