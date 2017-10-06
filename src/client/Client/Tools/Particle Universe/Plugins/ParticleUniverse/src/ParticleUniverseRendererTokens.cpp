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
#include "ParticleUniverseRendererTokens.h"
#include "ParticleUniverseRenderer.h"
#include "ParticleRenderers/ParticleUniverseBillboardRenderer.h"


namespace ParticleUniverse
{
	// Constants
	const Ogre::String ParticleRendererTokens::KEYWORD_RENDERER = "renderer";
	const Ogre::String ParticleRendererTokens::KEYWORD_RENDER_Q_GROUP = "render_queue_group";
	const Ogre::String ParticleRendererTokens::KEYWORD_SORTING = "sorting";
	const Ogre::String ParticleRendererTokens::KEYWORD_TEXCOORDS_DEFINE = "texture_coords_define";
	const Ogre::String ParticleRendererTokens::KEYWORD_TEXCOORDS_ROWS = "texture_coords_rows";
	const Ogre::String ParticleRendererTokens::KEYWORD_TEXCOORDS_SET = "texture_coords_set";
	const Ogre::String ParticleRendererTokens::KEYWORD_TEXCOORDS_COLUMNS = "texture_coords_columns";
	const Ogre::String ParticleRendererTokens::KEYWORD_USE_SOFT_PARTICLES = "use_soft_particles";
	const Ogre::String ParticleRendererTokens::KEYWORD_SOFT_PARTICLES_CONTRAST_POWER = "soft_particles_contrast_power";
	const Ogre::String ParticleRendererTokens::KEYWORD_SOFT_PARTICLES_SCALE = "soft_particles_scale";
	const Ogre::String ParticleRendererTokens::KEYWORD_SOFT_PARTICLES_DELTA = "soft_particles_delta";

	//-----------------------------------------------------------------------
	void ParticleRendererTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RENDER_Q_GROUP, &mRenderQueueGroupToken);
		tokenRegister->addLexemeAction(KEYWORD_SORTING, &mSortingToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_DEFINE, &mTextureCoordsToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_ROWS, &mTextureCoordsRowsToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_SET, &mTextureCoordsSetToken);
		tokenRegister->addLexemeAction(KEYWORD_TEXCOORDS_COLUMNS, &mTextureCoordsColumnsToken);
		tokenRegister->addLexemeAction(KEYWORD_USE_SOFT_PARTICLES, &mUseSoftParticlesToken);
		tokenRegister->addLexemeAction(KEYWORD_SOFT_PARTICLES_CONTRAST_POWER, &mSoftParticlesContrastPowerToken);
		tokenRegister->addLexemeAction(KEYWORD_SOFT_PARTICLES_SCALE, &mSoftParticlesScaleToken);
		tokenRegister->addLexemeAction(KEYWORD_SOFT_PARTICLES_DELTA, &mSoftParticlesDeltaToken);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleRenderer
		const ParticleRenderer* renderer = static_cast<const ParticleRenderer*>(element);

		// Write base attributes
		if (renderer->getRenderQueueGroup() != ParticleRenderer::DEFAULT_RENDER_QUEUE_GROUP) serializer->writeLine(
			KEYWORD_RENDER_Q_GROUP, Ogre::StringConverter::toString(renderer->getRenderQueueGroup()), 12);
		if (renderer->isSorted() != ParticleRenderer::DEFAULT_SORTED) serializer->writeLine(
			KEYWORD_SORTING, Ogre::StringConverter::toString(renderer->isSorted()), 12);
		if (renderer->getTextureCoordsRows() != ParticleRenderer::DEFAULT_TEXTURECOORDS_ROWS) serializer->writeLine(
			KEYWORD_TEXCOORDS_ROWS, Ogre::StringConverter::toString(renderer->getTextureCoordsRows()), 12);
		if (renderer->getTextureCoordsColumns() != ParticleRenderer::DEFAULT_TEXTURECOORDS_COLUMNS) serializer->writeLine(
			KEYWORD_TEXCOORDS_COLUMNS, Ogre::StringConverter::toString(renderer->getTextureCoordsColumns()), 12);

		const std::vector<Ogre::FloatRect*> uvList = renderer->getTextureCoords();
		if (!uvList.empty())
		{
			serializer->writeLine(KEYWORD_TEXCOORDS_SET, 12);
			serializer->writeLine("{", 12);
			std::vector<Ogre::FloatRect*>::const_iterator it;
			std::vector<Ogre::FloatRect*>::const_iterator itEnd = uvList.end();
			for (it = uvList.begin(); it != itEnd; ++it)
			{
				serializer->writeLine(KEYWORD_TEXCOORDS_DEFINE, Ogre::StringConverter::toString(*it), 12);
			}
			serializer->writeLine("}", 12);
		}

		if (renderer->getUseSoftParticles() != ParticleRenderer::DEFAULT_USE_SOFT_PARTICLES) serializer->writeLine(
			KEYWORD_USE_SOFT_PARTICLES, Ogre::StringConverter::toString(renderer->getUseSoftParticles()), 12);
		if (renderer->getSoftParticlesContrastPower() != ParticleRenderer::DEFAULT_SOFT_PARTICLES_CONTRAST_POWER) serializer->writeLine(
			KEYWORD_SOFT_PARTICLES_CONTRAST_POWER, Ogre::StringConverter::toString(renderer->getSoftParticlesContrastPower()), 12);
		if (renderer->getSoftParticlesScale() != ParticleRenderer::DEFAULT_SOFT_PARTICLES_SCALE) serializer->writeLine(
			KEYWORD_SOFT_PARTICLES_SCALE, Ogre::StringConverter::toString(renderer->getSoftParticlesScale()), 12);
		if (renderer->getSoftParticlesDelta() != ParticleRenderer::DEFAULT_SOFT_PARTICLES_DELTA) serializer->writeLine(
			KEYWORD_SOFT_PARTICLES_DELTA, Ogre::StringConverter::toString(renderer->getSoftParticlesDelta()), 12);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::RenderQueueGroupToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::RenderQueueGroupToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint8 renderQueueGroup;
		deserializer->parseUint8(renderQueueGroup);
		renderer->setRenderQueueGroup(renderQueueGroup);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::SortingToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::Sorting::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		bool sorted;
		deserializer->parseBool(sorted);
		renderer->setSorted(sorted);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::TextureCoordsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		// The section is on a 'texture_coords_set' level; this is one level deeper than the renderer level
		deserializer->context.validateCurrentSectionName(INNER_CONTEXT, "ParticleRendererTokens::TextureCoordsToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector4 texureCoords;
		deserializer->parseVector4(texureCoords);
		renderer->addTextureCoords(texureCoords.x, texureCoords.y, texureCoords.z, texureCoords.w);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::TextureCoordsSetToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::TextureCoordsToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		deserializer->context.beginSection(INNER_CONTEXT, renderer); // Still part of renderer level
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::TextureCoordsRowsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::TextureCoordsRowsToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ushort textureCoordsRows;
		deserializer->parseUshort(textureCoordsRows);
		renderer->setTextureCoordsRows(textureCoordsRows);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::TextureCoordsColumnsToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::TextureCoordsColumnsToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ushort textureCoordsColumns;
		deserializer->parseUshort(textureCoordsColumns);
		renderer->setTextureCoordsColumns(textureCoordsColumns);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::UseSoftParticlesToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::UseSoftParticlesToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		bool useSoftParticles;
		deserializer->parseBool(useSoftParticles);
		renderer->setUseSoftParticles(useSoftParticles);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::SoftParticlesContrastPowerToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::SoftParticlesContrastPowerToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real contrastPower;
		deserializer->parseReal(contrastPower);
		renderer->setSoftParticlesContrastPower(contrastPower);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::SoftParticlesScaleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::SoftParticlesScaleToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real scale;
		deserializer->parseReal(scale);
		renderer->setSoftParticlesScale(scale);
	}
	//-----------------------------------------------------------------------
	void ParticleRendererTokens::SoftParticlesDeltaToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "ParticleRendererTokens::SoftParticlesDeltaToken::parse");
		ParticleRenderer* renderer = static_cast<ParticleRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real delta;
		deserializer->parseReal(delta);
		renderer->setSoftParticlesDelta(delta);
	}

}
