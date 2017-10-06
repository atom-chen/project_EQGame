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
#include "ParticleRenderers/ParticleUniverseBillboardRendererTokens.h"
#include "ParticleRenderers/ParticleUniverseBillboardRenderer.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String BillboardRendererTokens::KEYWORD_BILLBOARD_TYPE = "billboard_type";
	const Ogre::String BillboardRendererTokens::KEYWORD_BILLBOARD_ORIGIN = "billboard_origin";
	const Ogre::String BillboardRendererTokens::KEYWORD_ROTATION_TYPE = "billboard_rotation_type";
	const Ogre::String BillboardRendererTokens::KEYWORD_COMMON_DIRECTION = "common_direction";
	const Ogre::String BillboardRendererTokens::KEYWORD_COMMON_UP_VECTOR = "common_up_vector";
	const Ogre::String BillboardRendererTokens::KEYWORD_POINT_RENDERING = "point_rendering";
	const Ogre::String BillboardRendererTokens::KEYWORD_ACCURATE_FACING = "accurate_facing";
	const Ogre::String BillboardRendererTokens::KEYWORD_ORIENTED_COMMON = "oriented_common";
	const Ogre::String BillboardRendererTokens::KEYWORD_ORIENTED_SELF = "oriented_self";
	const Ogre::String BillboardRendererTokens::KEYWORD_ORIENTED_SHAPE = "oriented_shape";
	const Ogre::String BillboardRendererTokens::KEYWORD_PERPENDICULAR_COMMON = "perpendicular_common";
	const Ogre::String BillboardRendererTokens::KEYWORD_POINT = "point";
	const Ogre::String BillboardRendererTokens::KEYWORD_PERPENDICULAR_SELF = "perpendicular_self";
	const Ogre::String BillboardRendererTokens::KEYWORD_TOP_LEFT = "top_left";
	const Ogre::String BillboardRendererTokens::KEYWORD_TOP_CENTER = "top_center";
	const Ogre::String BillboardRendererTokens::KEYWORD_TOP_RIGHT = "top_right";
	const Ogre::String BillboardRendererTokens::KEYWORD_CENTER_LEFT = "center_left";
	const Ogre::String BillboardRendererTokens::KEYWORD_CENTER_RIGHT = "center_right";
	const Ogre::String BillboardRendererTokens::KEYWORD_CENTER = "center";
	const Ogre::String BillboardRendererTokens::KEYWORD_BOTTON_LEFT = "bottom_left";
	const Ogre::String BillboardRendererTokens::KEYWORD_BOTTOM_CENTER = "bottom_center";
	const Ogre::String BillboardRendererTokens::KEYWORD_BOTTOM_RIGHT = "bottom_right";
	const Ogre::String BillboardRendererTokens::KEYWORD_VERTEX = "vertex";
	const Ogre::String BillboardRendererTokens::KEYWORD_TEXCOORD = "texcoord";

	//-----------------------------------------------------------------------
	void BillboardRendererTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_BILLBOARD_TYPE, &mBillboardTypeToken);
		tokenRegister->addLexemeAction(KEYWORD_BILLBOARD_ORIGIN, &mBillboardOriginToken);
		tokenRegister->addLexemeAction(KEYWORD_ROTATION_TYPE, &mBillboardRotationTypeToken);
		tokenRegister->addLexemeAction(KEYWORD_COMMON_DIRECTION, &mCommonDirectionToken);
		tokenRegister->addLexemeAction(KEYWORD_COMMON_UP_VECTOR, &mCommonUpVectorToken);
		tokenRegister->addLexemeAction(KEYWORD_POINT_RENDERING, &mPointRenderingToken);
		tokenRegister->addLexemeAction(KEYWORD_ACCURATE_FACING, &mAccurateFacingToken);

		// Billboard type tokens
		tokenRegister->addTokenId(KEYWORD_ORIENTED_COMMON, ITokenRegister::ID_BBT_ORIENTED_COMMON);
		tokenRegister->addTokenId(KEYWORD_ORIENTED_SELF, ITokenRegister::ID_BBT_ORIENTED_SELF);
		tokenRegister->addTokenId(KEYWORD_ORIENTED_SHAPE, ITokenRegister::ID_BBT_ORIENTED_SHAPE);
		tokenRegister->addTokenId(KEYWORD_PERPENDICULAR_COMMON, ITokenRegister::ID_BBT_PERPENDICULAR_COMMON);
		tokenRegister->addTokenId(KEYWORD_PERPENDICULAR_SELF, ITokenRegister::ID_BBT_PERPENDICULAR_SELF);
		// Note: "point" is already registered on a higher level

		// Billboard origin tokens
		tokenRegister->addTokenId(KEYWORD_TOP_LEFT, ITokenRegister::ID_BBT_TOP_LEFT);
		tokenRegister->addTokenId(KEYWORD_TOP_CENTER, ITokenRegister::ID_BBT_TOP_CENTER);
		tokenRegister->addTokenId(KEYWORD_TOP_RIGHT, ITokenRegister::ID_BBT_TOP_RIGHT);
		tokenRegister->addTokenId(KEYWORD_CENTER_LEFT, ITokenRegister::ID_BBT_CENTER_LEFT);
		tokenRegister->addTokenId(KEYWORD_CENTER_RIGHT, ITokenRegister::ID_BBT_CENTER_RIGHT);
		tokenRegister->addTokenId(KEYWORD_CENTER, ITokenRegister::ID_BBT_CENTER); // Must be put at the end because of a bug in Compiler2Pass
		tokenRegister->addTokenId(KEYWORD_BOTTON_LEFT, ITokenRegister::ID_BBT_BOTTOM_LEFT);
		tokenRegister->addTokenId(KEYWORD_BOTTOM_CENTER, ITokenRegister::ID_BBT_BOTTOM_CENTER);
		tokenRegister->addTokenId(KEYWORD_BOTTOM_RIGHT, ITokenRegister::ID_BBT_BOTTOM_RIGHT);

		// Billboard rotation type tokens
		tokenRegister->addTokenId(KEYWORD_VERTEX, ITokenRegister::ID_BBT_VERTEX);
		tokenRegister->addTokenId(KEYWORD_TEXCOORD, ITokenRegister::ID_BBT_TEX_COORD);
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a BillboardRenderer
		const BillboardRenderer* renderer = static_cast<const BillboardRenderer*>(element);

		// Write the header of the BillboardRenderer
		serializer->writeLine(KEYWORD_RENDERER, renderer->getRendererType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleRendererTokens::write(serializer, element);

		// Write own attributes
		if (renderer->getBillboardType() != BillboardRenderer::DEFAULT_BILLBOARD_TYPE)
		{
			Ogre::String billboardType = KEYWORD_POINT;
			if (renderer->getBillboardType() == BillboardRenderer::BBT_ORIENTED_COMMON) billboardType = KEYWORD_ORIENTED_COMMON;
			else if (renderer->getBillboardType() == BillboardRenderer::BBT_ORIENTED_SELF) billboardType = KEYWORD_ORIENTED_SELF;
			else if (renderer->getBillboardType() == BillboardRenderer::BBT_ORIENTED_SHAPE) billboardType = KEYWORD_ORIENTED_SHAPE;
			else if (renderer->getBillboardType() == BillboardRenderer::BBT_PERPENDICULAR_COMMON) billboardType = KEYWORD_PERPENDICULAR_COMMON;
			else if (renderer->getBillboardType() == BillboardRenderer::BBT_PERPENDICULAR_SELF) billboardType = KEYWORD_PERPENDICULAR_SELF;
			serializer->writeLine(KEYWORD_BILLBOARD_TYPE, billboardType, 12);
		}

		if (renderer->getBillboardOrigin() != BillboardRenderer::DEFAULT_ORIGIN)
		{
			Ogre::String origin = KEYWORD_CENTER;
			if (renderer->getBillboardOrigin() == Ogre::BBO_BOTTOM_CENTER) origin = KEYWORD_BOTTOM_CENTER;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_BOTTOM_LEFT) origin = KEYWORD_BOTTON_LEFT;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_BOTTOM_RIGHT) origin = KEYWORD_BOTTOM_RIGHT;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_CENTER_LEFT) origin = KEYWORD_CENTER_LEFT;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_CENTER_RIGHT) origin = KEYWORD_CENTER_RIGHT;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_TOP_CENTER) origin = KEYWORD_TOP_CENTER;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_TOP_LEFT) origin = KEYWORD_TOP_LEFT;
			else if (renderer->getBillboardOrigin() == Ogre::BBO_TOP_RIGHT) origin = KEYWORD_TOP_RIGHT;
			serializer->writeLine(KEYWORD_BILLBOARD_ORIGIN, origin, 12);
		}

		if (renderer->getBillboardRotationType() != BillboardRenderer::DEFAULT_ROTATION_TYPE)
		{
			Ogre::String rotationType = KEYWORD_VERTEX;
			if (renderer->getBillboardRotationType() == Ogre::BBR_TEXCOORD) rotationType = KEYWORD_TEXCOORD;
			serializer->writeLine(KEYWORD_ROTATION_TYPE, rotationType, 12);
		}

		if (renderer->getCommonDirection() != BillboardRenderer::DEFAULT_COMMON_DIRECTION) serializer->writeLine(
			KEYWORD_COMMON_DIRECTION, Ogre::StringConverter::toString(renderer->getCommonDirection()), 12);
		if (renderer->getCommonUpVector() != BillboardRenderer::DEFAULT_COMMON_UP_VECTOR) serializer->writeLine(
			KEYWORD_COMMON_UP_VECTOR, Ogre::StringConverter::toString(renderer->getCommonUpVector()), 12);
		if (renderer->isPointRenderingEnabled() != BillboardRenderer::DEFAULT_POINT_RENDERING) serializer->writeLine(
			KEYWORD_POINT_RENDERING, Ogre::StringConverter::toString(renderer->isPointRenderingEnabled()), 12);
		if (renderer->isUseAccurateFacing() != BillboardRenderer::DEFAULT_ACCURATE_FACING) serializer->writeLine(
			KEYWORD_ACCURATE_FACING, Ogre::StringConverter::toString(renderer->isUseAccurateFacing()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::BillboardTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::BillboardTypeToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_ORIENTED_COMMON))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_ORIENTED_COMMON);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_ORIENTED_SELF))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_ORIENTED_SELF);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_ORIENTED_SHAPE))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_ORIENTED_SHAPE);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_PERPENDICULAR_COMMON))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_PERPENDICULAR_COMMON);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_PERPENDICULAR_SELF))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_PERPENDICULAR_SELF);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_SHAPE_POINT))
		{
			renderer->setBillboardType(BillboardRenderer::BBT_POINT);
		}
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::BillboardOriginToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::BillboardOriginToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_TOP_LEFT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_TOP_LEFT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_TOP_CENTER))
		{
			renderer->setBillboardOrigin(Ogre::BBO_TOP_CENTER);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_TOP_RIGHT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_TOP_RIGHT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_CENTER_LEFT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_CENTER_LEFT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_CENTER))
		{
			renderer->setBillboardOrigin(Ogre::BBO_CENTER);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_CENTER_RIGHT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_CENTER_RIGHT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_BOTTOM_LEFT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_BOTTOM_LEFT);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_BOTTOM_CENTER))
		{
			renderer->setBillboardOrigin(Ogre::BBO_BOTTOM_CENTER);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_BOTTOM_RIGHT))
		{
			renderer->setBillboardOrigin(Ogre::BBO_BOTTOM_RIGHT);
		}
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::BillboardRotationTypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::BillboardRotationTypeToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_VERTEX))
		{
			renderer->setBillboardRotationType(Ogre::BBR_VERTEX);
		}
		else if (deserializer->testNextTokenID(ITokenRegister::ID_BBT_TEX_COORD))
		{
			renderer->setBillboardRotationType(Ogre::BBR_TEXCOORD);
		}
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::CommonDirectionToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::CommonDirectionToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 commonDirection;
		deserializer->parseVector3(commonDirection);
		renderer->setCommonDirection(commonDirection);
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::CommonUpVectorToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::CommonUpVectorToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector3 commonUpVector;
		deserializer->parseVector3(commonUpVector);
		renderer->setCommonUpVector(commonUpVector);
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::PointRenderingToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::PointRenderingToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		bool pointRendering;
		deserializer->parseBool(pointRendering);
		renderer->setPointRenderingEnabled(pointRendering);
	}
	//-----------------------------------------------------------------------
	void BillboardRendererTokens::AccurateFacingToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(RENDERER, "BillboardRendererTokens::AccurateFacingToken::parse");
		BillboardRenderer* renderer = static_cast<BillboardRenderer*>(deserializer->context.getCurrentSectionElement());
		if (deserializer->testNextTokenID(ITokenRegister::ID_ON))
		{
			renderer->setUseAccurateFacing(true);
		}
		else
			renderer->setUseAccurateFacing(false);
	}
}
