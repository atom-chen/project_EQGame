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
#include "ParticleAffectors/ParticleUniverseGravityAffector.h"
#include "ParticleAffectors/ParticleUniverseGravityAffectorTokens.h"
#include "Externs/ParticleUniverseGravityExtern.h"


namespace ParticleUniverse
{
	// Constants
	const Ogre::String GravityAffectorTokens::KEYWORD_GRAVITY = "gravity";

	//-----------------------------------------------------------------------
	void GravityAffectorTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_GRAVITY, &mGravityToken);
	}
	//-----------------------------------------------------------------------
	void GravityAffectorTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a GravityAffector
		const GravityAffector* affector = static_cast<const GravityAffector*>(element);

		// Write the header of the GravityAffector
		serializer->writeLine(KEYWORD_AFFECTOR, affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorTokens::write(serializer, element);

		// Write own attributes
		if (affector->getGravity() != GravityAffector::DEFAULT_GRAVITY) serializer->writeLine(
			KEYWORD_GRAVITY, Ogre::StringConverter::toString(affector->getGravity()), 12);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}
	//-----------------------------------------------------------------------
	void GravityAffectorTokens::GravityToken::parse(ParticleScriptDeserializer* deserializer)
	{
		/** The GravityExtern (mis)uses the scripting posibilities of the GravityAffector. That is why
			the validation is removed from this function (because mulitple sectionnames are possible now).
		*/
		Ogre::Real gravity;
		deserializer->parseReal(gravity);

		if (deserializer->context.getCurrentSectionName() == EXTERN)
		{
			/** Explicit cast to GravityExtern* is needed apparently. If the section element is a GravityExtern 
				object and casted to a GravityAffector* and the gravity is set, it somehow looses this value.
				If the object later on is casted to GravityExtern* again, the gravity value is gone.
			*/
			Extern* externObject = static_cast<Extern*>(deserializer->context.getCurrentSectionElement());
			GravityExtern* gravityExtern = static_cast<GravityExtern*>(externObject);
			gravityExtern->setGravity(gravity);
		}
		else
		{
			GravityAffector* affector = static_cast<GravityAffector*>(deserializer->context.getCurrentSectionElement());
			affector->setGravity(gravity);
		}
	}

}
