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
#include "ParticleRenderers/ParticleUniverseLightRendererTokens.h"
#include "ParticleRenderers/ParticleUniverseLightRenderer.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String LightRendererTokens::KEYWORD_LIGHT_TYPE = "light_renderer_light_type";
	const Ogre::String LightRendererTokens::KEYWORD_RENDER_QUEUE = "light_renderer_queue_group";
	const Ogre::String LightRendererTokens::KEYWORD_DIFFUSE = "light_renderer_diffuse";
	const Ogre::String LightRendererTokens::KEYWORD_SPECULAR = "light_renderer_specular";
	const Ogre::String LightRendererTokens::KEYWORD_ATT_RANGE = "light_renderer_att_range";
	const Ogre::String LightRendererTokens::KEYWORD_ATT_CONSTANT = "light_renderer_att_constant";
	const Ogre::String LightRendererTokens::KEYWORD_ATT_LINEAR = "light_renderer_att_linear";
	const Ogre::String LightRendererTokens::KEYWORD_ATT_QUADRATIC = "light_renderer_att_quadratic";
	const Ogre::String LightRendererTokens::KEYWORD_SPOT_INNER = "light_renderer_spot_inner";
	const Ogre::String LightRendererTokens::KEYWORD_SPOT_OUTER = "light_renderer_spot_outer";
	const Ogre::String LightRendererTokens::KEYWORD_FALLOFF = "light_renderer_falloff";
	const Ogre::String LightRendererTokens::KEYWORD_POWERSCALE = "light_renderer_powerscale";
	const Ogre::String LightRendererTokens::KEYWORD_POINT = "point";
	const Ogre::String LightRendererTokens::KEYWORD_SPOT = "spot";

	//-----------------------------------------------------------------------
	void LightRendererTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_LIGHT_TYPE, &mLightTypeToken);
		tokenRegister->addLexemeAction(KEYWORD_RENDER_QUEUE, &mQueueIdToken);
		tokenRegister->addLexemeAction(KEYWORD_DIFFUSE, &mDiffuseColourToken);
		tokenRegister->addLexemeAction(KEYWORD_SPECULAR, &mSpecularColourToken);
		tokenRegister->addLexemeAction(KEYWORD_ATT_RANGE, &mAttenuationRangeToken);
		tokenRegister->addLexemeAction(KEYWORD_ATT_CONSTANT, &mAttenuationConstantToken);
		tokenRegister->addLexemeAction(KEYWORD_ATT_LINEAR, &mAttenuationLinearToken);
		tokenRegister->addLexemeAction(KEYWORD_ATT_QUADRATIC, &mAttenuationQuadraticToken);
		tokenRegister->addLexemeAction(KEYWORD_SPOT_INNER, &mSpotlightInnerAngleToken);
		tokenRegister->addLexemeAction(KEYWORD_SPOT_OUTER, &mSpotlightOuterAngleToken);
		tokenRegister->addLexemeAction(KEYWORD_FALLOFF, &mSpotlightFalloffToken);
		tokenRegister->addLexemeAction(KEYWORD_POWERSCALE, &mPowerScaleToken);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a LightRenderer
		const LightRenderer* renderer = static_cast<const LightRenderer*>(element);

		// Write the header of the LightRenderer
		serializer->writeLine(KEYWORD_RENDERER, renderer->getRendererType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleRendererTokens::write(serializer, element);

		// Write own attributes
		if (renderer->getLightType() != LightRenderer::DEFAULT_LIGHT_TYPE)
		{
			Ogre::String lightType = KEYWORD_POINT;
			if (renderer->getLightType() == Ogre::Light::LT_SPOTLIGHT) lightType = KEYWORD_SPOT;
			serializer->writeLine(KEYWORD_LIGHT_TYPE, lightType, 12);
		}
		if (renderer->getRenderQueueGroup() != LightRenderer::DEFAULT_RENDER_QUEUE_GROUP) serializer->writeLine(
			KEYWORD_RENDER_Q_GROUP, Ogre::StringConverter::toString(renderer->getRenderQueueGroup()), 12);
		if (renderer->getDiffuseColour() != LightRenderer::DEFAULT_DIFFUSE) serializer->writeLine(
			KEYWORD_DIFFUSE, Ogre::StringConverter::toString(renderer->getDiffuseColour()), 12);
		if (renderer->getSpecularColour() != LightRenderer::DEFAULT_SPECULAR) serializer->writeLine(
			KEYWORD_SPECULAR, Ogre::StringConverter::toString(renderer->getSpecularColour()), 12);
		if (renderer->getAttenuationRange() != LightRenderer::DEFAULT_ATT_RANGE) serializer->writeLine(
			KEYWORD_ATT_RANGE, Ogre::StringConverter::toString(renderer->getAttenuationRange()), 12);
		if (renderer->getAttenuationConstant() != LightRenderer::DEFAULT_ATT_CONSTANT) serializer->writeLine(
			KEYWORD_ATT_CONSTANT, Ogre::StringConverter::toString(renderer->getAttenuationConstant()), 12);
		if (renderer->getAttenuationLinear() != LightRenderer::DEFAULT_ATT_LINEAR) serializer->writeLine(
			KEYWORD_ATT_LINEAR, Ogre::StringConverter::toString(renderer->getAttenuationLinear()), 12);
		if (renderer->getAttenuationQuadratic() != LightRenderer::DEFAULT_ATT_QUADRATIC) serializer->writeLine(
			KEYWORD_ATT_QUADRATIC, Ogre::StringConverter::toString(renderer->getAttenuationQuadratic()), 12);
		if (renderer->getSpotlightInnerAngle() != LightRenderer::DEFAULT_SPOT_INNER_ANGLE) serializer->writeLine(
			KEYWORD_SPOT_INNER, Ogre::StringConverter::toString(renderer->getSpotlightInnerAngle().valueDegrees()), 12);
		if (renderer->getSpotlightOuterAngle() != LightRenderer::DEFAULT_SPOT_OUTER_ANGLE) serializer->writeLine(
			KEYWORD_SPOT_OUTER, Ogre::StringConverter::toString(renderer->getSpotlightOuterAngle().valueDegrees()), 12);
		if (renderer->getSpotlightFalloff() != LightRenderer::DEFAULT_FALLOFF) serializer->writeLine(
			KEYWORD_FALLOFF, Ogre::StringConverter::toString(renderer->getSpotlightFalloff()), 12);
		if (renderer->getPowerScale() != LightRenderer::DEFAULT_POWER_SCALE) serializer->writeLine(
			KEYWORD_POWERSCALE, Ogre::StringConverter::toString(renderer->getPowerScale()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::LightTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::LightTypeToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());

		// Parse the string.
		Ogre::String type = deserializer->getNextTokenLabel();
		if (type == KEYWORD_POINT)
		{
			renderer->setLightType(Ogre::Light::LT_POINT);
		}
		else if (type == KEYWORD_SPOT)
		{
			renderer->setLightType(Ogre::Light::LT_SPOTLIGHT);
		}
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::QueueIdToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::QueueIdToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::uint8 renderQueueGroup;
		deserializer->parseUint8(renderQueueGroup);
		renderer->setRenderQueueGroup(renderQueueGroup);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::DiffuseColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::DiffuseColourToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		renderer->setDiffuseColour(colour);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::SpecularColourToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::SpecularColourToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::ColourValue colour;
		deserializer->parseColourValue(colour);
		renderer->setSpecularColour(colour);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::AttenuationRangeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::AttenuationRangeToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real range;
		deserializer->parseReal(range);
		renderer->setAttenuationRange(range);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::AttenuationConstantToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::AttenuationConstantToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real constant;
		deserializer->parseReal(constant);
		renderer->setAttenuationConstant(constant);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::AttenuationLinearToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::AttenuationLinearToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real linear;
		deserializer->parseReal(linear);
		renderer->setAttenuationLinear(linear);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::AttenuationQuadraticToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::AttenuationQuadraticToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real quadratic;
		deserializer->parseReal(quadratic);
		renderer->setAttenuationQuadratic(quadratic);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::SpotlightInnerAngleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::SpotlightInnerAngleToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real angle;
		deserializer->parseReal(angle);
		renderer->setSpotlightInnerAngle(Ogre::Degree(angle));
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::SpotlightOuterAngleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::SpotlightOuterAngleToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real angle;
		deserializer->parseReal(angle);
		renderer->setSpotlightOuterAngle(Ogre::Degree(angle));
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::SpotlightFalloffToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::SpotlightFalloffToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real falloff;
		deserializer->parseReal(falloff);
		renderer->setSpotlightFalloff(falloff);
	}
	//-----------------------------------------------------------------------
	void LightRendererTokens::PowerScaleToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "LightRendererTokens::PowerScaleToken::parse");
		LightRenderer* renderer = static_cast<LightRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real powerScale;
		deserializer->parseReal(powerScale);
		renderer->setPowerScale(powerScale);
	}

}
