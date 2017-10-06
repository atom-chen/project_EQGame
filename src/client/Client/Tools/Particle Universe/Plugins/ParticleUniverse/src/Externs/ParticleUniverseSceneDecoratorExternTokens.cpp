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
#include "ParticleUniverseScriptDeserializer.h"
#include "Externs/ParticleUniverseSceneDecoratorExtern.h"
#include "Externs/ParticleUniverseSceneDecoratorExternTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String SceneDecoratorExternTokens::KEYWORD_MESH_NAME = "scene_mesh_name";
	const Ogre::String SceneDecoratorExternTokens::KEYWORD_MATERIAL_NAME = "scene_material_name";
	const Ogre::String SceneDecoratorExternTokens::KEYWORD_SCALE = "scene_node_scale";
	const Ogre::String SceneDecoratorExternTokens::KEYWORD_POSITION = "scene_node_position";

	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MESH_NAME, &mMeshToken);
		tokenRegister->addLexemeAction(KEYWORD_MATERIAL_NAME, &mMaterialNameToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE, &mScaleToken);
		tokenRegister->addLexemeAction(KEYWORD_POSITION, &mPositionToken);
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SceneDecoratorExtern
		const Extern* externObject = static_cast<const Extern*>(element);
		const SceneDecoratorExtern* sceneDecoratorExtern = static_cast<const SceneDecoratorExtern*>(externObject);
		serializer->writeLine(KEYWORD_EXTERN, externObject->getExternType(), externObject->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ExternTokens::write(serializer, element);

		// Write own attributes
		if (!sceneDecoratorExtern->getMeshName().empty()) serializer->writeLine(
			KEYWORD_MESH_NAME, sceneDecoratorExtern->getMeshName(), 12);
		if (!sceneDecoratorExtern->getMaterialName().empty()) serializer->writeLine(
			KEYWORD_MATERIAL_NAME, sceneDecoratorExtern->getMaterialName(), 12);
		if (sceneDecoratorExtern->getScale() != Ogre::Vector3::UNIT_SCALE) serializer->writeLine(
			KEYWORD_SCALE, Ogre::StringConverter::toString(sceneDecoratorExtern->getScale()), 12);
		if (sceneDecoratorExtern->getPosition() != Ogre::Vector3::ZERO) serializer->writeLine(
			KEYWORD_POSITION, Ogre::StringConverter::toString(sceneDecoratorExtern->getPosition()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::MeshToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EXTERN, "SceneDecoratorExternTokens::MeshToken::parse");
		SceneDecoratorExtern* externObject = static_cast<SceneDecoratorExtern*>(deserializer->context.getCurrentSectionElement());
		externObject->setMeshName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::MaterialNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EXTERN, "SceneDecoratorExternTokens::MaterialNameToken::parse");
		SceneDecoratorExtern* externObject = static_cast<SceneDecoratorExtern*>(deserializer->context.getCurrentSectionElement());
		externObject->setMaterialName(deserializer->getNextTokenLabel());
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::ScaleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EXTERN, "SceneDecoratorExternTokens::ScaleToken::parse");
		SceneDecoratorExtern* externObject = static_cast<SceneDecoratorExtern*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 scale;
		deserializer->parseVector3(scale);
		externObject->setScale(scale);
	}
	//-----------------------------------------------------------------------
	void SceneDecoratorExternTokens::PositionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EXTERN, "SceneDecoratorExternTokens::PositionToken::parse");
		SceneDecoratorExtern* externObject = static_cast<SceneDecoratorExtern*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 position;
		deserializer->parseVector3(position);
		externObject->setPosition(position);
	}

}
