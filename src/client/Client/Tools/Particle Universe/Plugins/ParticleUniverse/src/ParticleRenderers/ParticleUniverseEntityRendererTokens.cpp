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
#include "ParticleRenderers/ParticleUniverseEntityRendererTokens.h"
#include "ParticleRenderers/ParticleUniverseEntityRenderer.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String EntityRendererTokens::KEYWORD_MESH_NAME = "entity_renderer_mesh_name";
	const Ogre::String EntityRendererTokens::KEYWORD_ORIENTATION_TYPE = "entity_orientation_type";
	const Ogre::String EntityRendererTokens::KEYWORD_ORIENTED_SELF = "ent_oriented_self";
	const Ogre::String EntityRendererTokens::KEYWORD_ORIENTED_SELF_MIRRORED = "ent_oriented_self_mirrored";
	const Ogre::String EntityRendererTokens::KEYWORD_ORIENTED_SHAPE = "ent_oriented_shape";

	//-----------------------------------------------------------------------
	void EntityRendererTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MESH_NAME, &mMeshNameToken);
		tokenRegister->addLexemeAction(KEYWORD_ORIENTATION_TYPE, &mEntityOrientationTypeToken);
	}
	//-----------------------------------------------------------------------
	void EntityRendererTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a EntityRenderer
		const EntityRenderer* renderer = static_cast<const EntityRenderer*>(element);

		// Write the header of the EntityRenderer
		serializer->writeLine(KEYWORD_RENDERER, renderer->getRendererType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleRendererTokens::write(serializer, element);

		// Write own attributes
		if (renderer->getMeshName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_MESH_NAME, renderer->getMeshName(), 12);
		if (renderer->getEntityOrientationType() != EntityRenderer::DEFAULT_ORIENTATION_TYPE)
		{
			Ogre::String orientationType = KEYWORD_ORIENTED_SELF;
			if (renderer->getEntityOrientationType() == EntityRenderer::ENT_ORIENTED_SHAPE) orientationType = KEYWORD_ORIENTED_SELF;
			else if (renderer->getEntityOrientationType() == EntityRenderer::ENT_ORIENTED_SELF_MIRRORED) orientationType = KEYWORD_ORIENTED_SELF_MIRRORED;
			serializer->writeLine(KEYWORD_ORIENTATION_TYPE, orientationType, 12);
		}

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void EntityRendererTokens::MeshNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "EntityRendererTokens::MeshNameToken::parse");
		EntityRenderer* renderer = static_cast<EntityRenderer*>(deserializer->context.getCurrentSectionElement());
		renderer->setMeshName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void EntityRendererTokens::EntityOrientationTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "EntityRendererTokens::EntityOrientationTypeToken::parse");
		EntityRenderer* renderer = static_cast<EntityRenderer*>(deserializer->context.getCurrentSectionElement());

		// Parse the string.
		Ogre::String orientationType = deserializer->getNextTokenLabel();
		if (orientationType == KEYWORD_ORIENTED_SELF)
		{
			renderer->setEntityOrientationType(EntityRenderer::ENT_ORIENTED_SELF);
		}
		else if (orientationType == KEYWORD_ORIENTED_SELF_MIRRORED)
		{
			renderer->setEntityOrientationType(EntityRenderer::ENT_ORIENTED_SELF_MIRRORED);
		}
		else if (orientationType == KEYWORD_ORIENTED_SHAPE)
		{
			renderer->setEntityOrientationType(EntityRenderer::ENT_ORIENTED_SHAPE);
		}
	}

}
