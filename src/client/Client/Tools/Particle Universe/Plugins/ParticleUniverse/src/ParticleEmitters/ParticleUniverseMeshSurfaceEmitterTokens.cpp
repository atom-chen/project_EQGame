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
#include "ParticleEmitters/ParticleUniverseMeshSurfaceEmitter.h"
#include "ParticleEmitters/ParticleUniverseMeshSurfaceEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_MESH_NAME = "mesh_surface_mesh_name";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_DISTRIBUTION = "mesh_surface_distribution";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_SCALE = "mesh_surface_scale";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_EDGE = "edge";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_HETEROGENEOUS_1 = "heterogeneous_1";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_HETEROGENEOUS_2 = "heterogeneous_2";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_HOMOGENEOUS = "homogeneous";
	const Ogre::String MeshSurfaceEmitterTokens::KEYWORD_VERTEX = "vertex";

	//-----------------------------------------------------------------------
	void MeshSurfaceEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_MESH_NAME, &mMeshNameToken);
		tokenRegister->addLexemeAction(KEYWORD_DISTRIBUTION, &mDistributionToken);
		tokenRegister->addLexemeAction(KEYWORD_SCALE, &mScaleToken);
	}
	//-----------------------------------------------------------------------
	void MeshSurfaceEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a MeshSurfaceEmitter
		const MeshSurfaceEmitter* emitter = static_cast<const MeshSurfaceEmitter*>(element);

		// Write the header of the MeshSurfaceEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getMeshName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_MESH_NAME, emitter->getMeshName(), 12);
		if (emitter->getScale() != MeshSurfaceEmitter::DEFAULT_SCALE) serializer->writeLine(
			KEYWORD_SCALE, Ogre::StringConverter::toString(emitter->getScale()), 12);
		if (emitter->getDistribution() != MeshSurfaceEmitter::DEFAULT_DISTRIBUTION) 
		{
			Ogre::String distribution = KEYWORD_HOMOGENEOUS;
			if (emitter->getDistribution() == MeshInfo::MSD_HOMOGENEOUS) distribution = KEYWORD_HOMOGENEOUS;
			else if (emitter->getDistribution() == MeshInfo::MSD_EDGE) distribution = KEYWORD_EDGE;
			else if (emitter->getDistribution() == MeshInfo::MSD_HETEROGENEOUS_1) distribution = KEYWORD_HETEROGENEOUS_1;
			else if (emitter->getDistribution() == MeshInfo::MSD_HETEROGENEOUS_2) distribution = KEYWORD_HETEROGENEOUS_2;
			else if (emitter->getDistribution() == MeshInfo::MSD_VERTEX) distribution = KEYWORD_VERTEX;
			serializer->writeLine(KEYWORD_DISTRIBUTION, distribution, 12);
		}


		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void MeshSurfaceEmitterTokens::MeshNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "MeshSurfaceEmitterTokens::MeshNameToken::parse");
		MeshSurfaceEmitter* emitter = static_cast<MeshSurfaceEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setMeshName(deserializer->getNextTokenLabel()); // doesn't build the data needed for emission (which is what we want).
	}
	//-----------------------------------------------------------------------
	void MeshSurfaceEmitterTokens::DistributionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "MeshSurfaceEmitterTokens::DistributionToken::parse");
		MeshSurfaceEmitter* emitter = static_cast<MeshSurfaceEmitter*>(deserializer->context.getCurrentSectionElement());

		// Parse the string. A better solution would be to use Id's instead.
		Ogre::String distribution = deserializer->getNextTokenLabel();
		if (distribution == KEYWORD_EDGE)
		{
			emitter->setDistribution(MeshInfo::MSD_EDGE);
		}
		else if (distribution == KEYWORD_HETEROGENEOUS_1)
		{
			emitter->setDistribution(MeshInfo::MSD_HETEROGENEOUS_1);
		}
		else if (distribution == KEYWORD_HETEROGENEOUS_2)
		{
			emitter->setDistribution(MeshInfo::MSD_HETEROGENEOUS_2);
		}
		else if (distribution == KEYWORD_HOMOGENEOUS)
		{
			emitter->setDistribution(MeshInfo::MSD_HOMOGENEOUS);
		}
		else if (distribution == KEYWORD_VERTEX)
		{
			emitter->setDistribution(MeshInfo::MSD_VERTEX);
		}
	}
	//-----------------------------------------------------------------------
	void MeshSurfaceEmitterTokens::ScaleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "MeshSurfaceEmitterTokens::ScaleToken::parse");
		MeshSurfaceEmitter* emitter = static_cast<MeshSurfaceEmitter*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 scale;
		deserializer->parseVector3(scale);
		emitter->setScale(scale);
	}

}
