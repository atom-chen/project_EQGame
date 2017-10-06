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
#include "ParticleEmitters/ParticleUniverseVertexEmitter.h"
#include "ParticleEmitters/ParticleUniverseVertexEmitterTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String VertexEmitterTokens::KEYWORD_STEP = "vertex_em_step";
	const Ogre::String VertexEmitterTokens::KEYWORD_SEGMENTS = "vertex_em_segments";
	const Ogre::String VertexEmitterTokens::KEYWORD_ITERATIONS = "vertex_em_iterations";
	const Ogre::String VertexEmitterTokens::KEYWORD_MESH_NAME = "vertex_em_mesh_name";

	//-----------------------------------------------------------------------
	void VertexEmitterTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_STEP, &mStepToken);
		tokenRegister->addLexemeAction(KEYWORD_SEGMENTS, &mSegmentsToken);
		tokenRegister->addLexemeAction(KEYWORD_ITERATIONS, &mIterationsToken);
		tokenRegister->addLexemeAction(KEYWORD_MESH_NAME, &mMeshNameToken);
	}
	//-----------------------------------------------------------------------
	void VertexEmitterTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a VertexEmitter
		const VertexEmitter* emitter = static_cast<const VertexEmitter*>(element);

		// Write the header of the VertexEmitter
		serializer->writeLine(KEYWORD_EMITTER, emitter->getEmitterType(), emitter->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleEmitterTokens::write(serializer, element);

		// Write own attributes
		if (emitter->getStep() != VertexEmitter::DEFAULT_STEP) serializer->writeLine(
			KEYWORD_STEP, Ogre::StringConverter::toString(emitter->getStep()), 12);
		if (emitter->getSegments() != VertexEmitter::DEFAULT_SEGMENTS) serializer->writeLine(
			KEYWORD_SEGMENTS, Ogre::StringConverter::toString(emitter->getSegments()), 12);
		if (emitter->getIterations() != VertexEmitter::DEFAULT_ITERATIONS) serializer->writeLine(
			KEYWORD_ITERATIONS, Ogre::StringConverter::toString(emitter->getIterations()), 12);
		if (emitter->getMeshName() != Ogre::StringUtil::BLANK) serializer->writeLine(
			KEYWORD_MESH_NAME, emitter->getMeshName(), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void VertexEmitterTokens::StepToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "VertexEmitterTokens::StepToken::parse");
		VertexEmitter* emitter = static_cast<VertexEmitter*>(deserializer->context.getCurrentSectionElement());
		unsigned short step;
		deserializer->parseUshort(step);
		emitter->setStep(step);
	}
	//-----------------------------------------------------------------------
	void VertexEmitterTokens::SegmentsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "VertexEmitterTokens::SegmentsToken::parse");
		VertexEmitter* emitter = static_cast<VertexEmitter*>(deserializer->context.getCurrentSectionElement());
		unsigned short segments;
		deserializer->parseUshort(segments);
		emitter->setSegments(segments);
	}
	//-----------------------------------------------------------------------
	void VertexEmitterTokens::IterationsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "VertexEmitterTokens::IterationsToken::parse");
		VertexEmitter* emitter = static_cast<VertexEmitter*>(deserializer->context.getCurrentSectionElement());
		unsigned short iterations;
		deserializer->parseUshort(iterations);
		emitter->setIterations(iterations);
	}
	//-----------------------------------------------------------------------
	void VertexEmitterTokens::MeshNameToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(EMITTER, "VertexEmitterTokens::MeshNameToken::parse");
		VertexEmitter* emitter = static_cast<VertexEmitter*>(deserializer->context.getCurrentSectionElement());
		emitter->setMeshName(deserializer->getNextTokenLabel());
	}

}
