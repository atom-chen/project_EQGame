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
#include "ParticleRenderers/ParticleUniverseRibbonTrailRendererTokens.h"
#include "ParticleRenderers/ParticleUniverseRibbonTrailRenderer.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_VERTEX_COLOURS = "ribbontrail_vertex_colours";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_MAX_ELEMENTS = "ribbontrail_max_elements";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_LENGTH = "ribbontrail_length";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_WIDTH = "ribbontrail_width";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_RANDOM_INITIAL_COLOUR = "ribbontrail_random_initial_colour";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_INITIAL_COLOUR = "ribbontrail_initial_colour";
	const Ogre::String RibbonTrailRendererTokens::KEYWORD_COLOUR_CHANGE = "ribbontrail_colour_change";

	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_VERTEX_COLOURS, &mVertexColoursToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX_ELEMENTS, &mMaxChainElementsToken);
		tokenRegister->addLexemeAction(KEYWORD_LENGTH, &mLengthToken);
		tokenRegister->addLexemeAction(KEYWORD_WIDTH, &mWidthToken);
		tokenRegister->addLexemeAction(KEYWORD_RANDOM_INITIAL_COLOUR, &mRandomInitialColourToken);
		tokenRegister->addLexemeAction(KEYWORD_INITIAL_COLOUR, &mInitialColourToken);
		tokenRegister->addLexemeAction(KEYWORD_COLOUR_CHANGE, &mColourChangeToken);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a RibbonTrailRenderer
		const RibbonTrailRenderer* renderer = static_cast<const RibbonTrailRenderer*>(element);

		// Write the header of the LightRenderer
		serializer->writeLine(KEYWORD_RENDERER, renderer->getRendererType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleRendererTokens::write(serializer, element);

		// Write own attributes
		if (renderer->isUseVertexColours() != RibbonTrailRenderer::DEFAULT_USE_VERTEX_COLOURS) serializer->writeLine(
			KEYWORD_VERTEX_COLOURS, Ogre::StringConverter::toString(renderer->isUseVertexColours()), 12);
		if (renderer->getMaxChainElements() != RibbonTrailRenderer::DEFAULT_MAX_ELEMENTS) serializer->writeLine(
			KEYWORD_MAX_ELEMENTS, Ogre::StringConverter::toString(renderer->getMaxChainElements()), 12);
		if (renderer->getTrailLength() != RibbonTrailRenderer::DEFAULT_LENGTH) serializer->writeLine(
			KEYWORD_LENGTH, Ogre::StringConverter::toString(renderer->getTrailLength()), 12);
		if (renderer->getTrailWidth() != RibbonTrailRenderer::DEFAULT_WIDTH) serializer->writeLine(
			KEYWORD_WIDTH, Ogre::StringConverter::toString(renderer->getTrailWidth()), 12);
		if (renderer->isRandomInitialColour() != RibbonTrailRenderer::DEFAULT_RANDOM_INITIAL_COLOUR) serializer->writeLine(
			KEYWORD_RANDOM_INITIAL_COLOUR, Ogre::StringConverter::toString(renderer->isRandomInitialColour()), 12);
		if (renderer->getInitialColour() != RibbonTrailRenderer::DEFAULT_INITIAL_COLOUR) serializer->writeLine(
			KEYWORD_INITIAL_COLOUR, Ogre::StringConverter::toString(renderer->getInitialColour()), 12);
		if (renderer->getColourChange() != RibbonTrailRenderer::DEFAULT_COLOUR_CHANGE) serializer->writeLine(
			KEYWORD_COLOUR_CHANGE, Ogre::StringConverter::toString(renderer->getColourChange()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::VertexColoursToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::VertexColoursToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		bool useVertexColours;
		deserializer->parseBool(useVertexColours);
		renderer->setUseVertexColours(useVertexColours);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::MaxChainElementsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::MaxChainElementsToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		size_t maxChainElements;
		deserializer->parseUint(maxChainElements);
		renderer->setMaxChainElements(maxChainElements);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::LengthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::LengthToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real length;
		deserializer->parseReal(length);
		renderer->setTrailLength(length);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::WidthToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::WidthToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real width;
		deserializer->parseReal(width);
		renderer->setTrailWidth(width);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::RandomInitialColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::RandomInitialColourToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		bool randomInitialColour;
		deserializer->parseBool(randomInitialColour);
		renderer->setRandomInitialColour(randomInitialColour);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::InitialColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::InitialColourToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		renderer->setInitialColour(colour);
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererTokens::ColourChangeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "RibbonTrailRendererTokens::ColourChangeToken::parse");
		RibbonTrailRenderer* renderer = static_cast<RibbonTrailRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		renderer->setColourChange(colour);
	}

}
