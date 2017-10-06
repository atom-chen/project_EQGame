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
#include "ParticleAffectors/ParticleUniversePathFollower.h"
#include "ParticleAffectors/ParticleUniversePathFollowerTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String PathFollowerTokens::KEYWORD_POINT = "path_follower_point";
	
	//-----------------------------------------------------------------------
	void PathFollowerTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_POINT, &mPointToken);
	}
	//-----------------------------------------------------------------------
	void PathFollowerTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a PathFollower
		const PathFollower* affector = static_cast<const PathFollower*>(element);

		// Write the header of the PathFollower
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		unsigned short numberOfPoints = affector->getNumPoints();
		if (numberOfPoints > 0)
		{
			for (unsigned short u = 0; u < numberOfPoints; ++u)
			{
				serializer->writeLine(KEYWORD_POINT, Ogre::StringConverter::toString(affector->getPoint(u)), 12);
			}
		}
				
		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void PathFollowerTokens::PointToken::parse(ParticleScriptDeserializer* deserializer)
	{
		deserializer->context.validateCurrentSectionName(AFFECTOR, "PathFollowerTokens::PointToken::parse");
		PathFollower* affector = static_cast<PathFollower*>(deserializer->context.getCurrentSectionElement());

		// Add point
		Ogre::Vector3 point;
		deserializer->parseVector3(point);
		affector->addPoint(point);
	}

}
