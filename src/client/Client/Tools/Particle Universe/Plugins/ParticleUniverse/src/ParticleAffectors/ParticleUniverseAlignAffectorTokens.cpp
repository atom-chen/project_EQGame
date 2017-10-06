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
#include "ParticleAffectors/ParticleUniverseAlignAffector.h"
#include "ParticleAffectors/ParticleUniverseAlignAffectorTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String AlignAffectorTokens::KEYWORD_RESIZE = "align_aff_resize";

	//-----------------------------------------------------------------------
	void AlignAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_RESIZE, &mResizeToken);
	}
	//-----------------------------------------------------------------------
	void AlignAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a AlignAffector
		const AlignAffector* affector = static_cast<const AlignAffector*>(element);

		// Write the header of the AlignAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->isResize() != AlignAffector::DEFAULT_RESIZE) serializer->writeLine(
			KEYWORD_RESIZE, Ogre::StringConverter::toString(affector->isResize()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void AlignAffectorTokens::ResizeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "AlignAffectorTokens::ResizeToken::parse");
		AlignAffector* affector = static_cast<AlignAffector*>(deserializer->context.getCurrentSectionElement());
		bool resize;
		deserializer->parseBool(resize);
		affector->setResize(resize);
	}

}
