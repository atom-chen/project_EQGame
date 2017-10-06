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
#include "ParticleUniverseSystemManager.h"
#include "ParticleUniverseITokenRegister.h"
#include "ParticleUniverseGrammar.h"

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	// Static definitions
	//-----------------------------------------------------------------------
	ParticleScriptDeserializer::TokenActionMap ParticleScriptDeserializer::mTokenActionMap;

	// ----------------------------------- Top level -----------------------------------
	Ogre::String ParticleScriptDeserializer::particleUniverseScriptGrammar = Ogre::StringUtil::BLANK;

	//-----------------------------------------------------------------------
	ParticleScriptDeserializer::ParticleScriptDeserializer(void) : mGroupName(Ogre::StringUtil::BLANK)
	{
	}
	//-----------------------------------------------------------------------
	ParticleScriptDeserializer::~ParticleScriptDeserializer(void)
	{
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleScriptDeserializer::getClientBNFGrammer(void) const
	{
		std::vector<IGrammar*>::const_iterator it;

		// Set begin part of the grammar
		particleUniverseScriptGrammar += Grammar::particleUniverseBeginGrammar;

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Renderers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add renderer header
		particleUniverseScriptGrammar += Grammar::particleRendererHeaderGrammar;

		// Add all renderer properties (from all registered plugins, including the core renderers)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleRendererPropertiesGrammar();
		}

		// Add all renderer detailed properties (from all registered plugins, including the core renderer)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleRendererDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Emitters ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add emitter header
		particleUniverseScriptGrammar += Grammar::particleEmitterHeaderGrammar;

		// Add all emitter properties (from all registered plugins, including the core emitters)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleEmitterPropertiesGrammar();
		}

		// Add all emitter detailed properties (from all registered plugins, including the core emitters)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleEmitterDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Affectors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add affector header
		particleUniverseScriptGrammar += Grammar::particleAffectorHeaderGrammar;

		// Add all affector properties (from all registered plugins, including the core affectors)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleAffectorPropertiesGrammar();
		}

		// Add all affector detailed properties (from all registered plugins, including the core affectors)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleAffectorDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Observers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add observer header
		particleUniverseScriptGrammar += Grammar::particleObserverHeaderGrammar;

		// Add all observer properties (from all registered plugins, including the core observers)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleObserverPropertiesGrammar();
		}

		// Add all observer detailed properties (from all registered plugins, including the core observers)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleObserverDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Event Handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add event handler header
		particleUniverseScriptGrammar += Grammar::particleEventHandlerHeaderGrammar;

		// Add all event handler properties (from all registered plugins, including the core event handlers)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleEventHandlerPropertiesGrammar();
		}

		// Add all event handler detailed properties (from all registered plugins, including the core event handlers)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleEventHandlerDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Behaviours ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add behaviour header
		particleUniverseScriptGrammar += Grammar::particleBehaviourHeaderGrammar;

		// Add all behaviour properties (from all registered plugins, including the core behaviours)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleBehaviourPropertiesGrammar();
		}

		// Add all behaviour detailed properties (from all registered plugins, including the core behaviours)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getParticleBehaviourDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Extern ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add extern header
		particleUniverseScriptGrammar += Grammar::externHeaderGrammar;

		// Add all extern properties (from all registered plugins, including the core externs)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getExternPropertiesGrammar();
		}

		// Add all extern detailed properties (from all registered plugins, including the core externs)
		for (it = mGrammarList.begin(); it != mGrammarList.end(); ++it)
		{
			particleUniverseScriptGrammar += (*it)->getExternDetailedPropertiesGrammar();
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Rest ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add the rest
		particleUniverseScriptGrammar += Grammar::particleUniverseEndGrammar;

		return particleUniverseScriptGrammar;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::addGrammar(IGrammar* grammar)
	{
		mGrammarList.push_back(grammar);
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::setupTokenDefinitions(void)
	{
		// Delegate back to the particle system manager
		ParticleSystemManager* particleSystemManager = ParticleSystemManager::getSingletonPtr();
		particleSystemManager->setupTokenDefinitions(this);
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::addLexemeTokenAction(const Ogre::String& lexeme,
		const size_t tokenId, IToken* token)
	{
		size_t newtokenID = addLexemeToken(lexeme, tokenId, token != 0);
        
		// only add actions to the map if they exist
		if (token)
			mTokenActionMap[newtokenID] = token;
	}
	//-----------------------------------------------------------------------
	size_t ParticleScriptDeserializer::addTokenId(const Ogre::String& lexeme, const size_t tokenId, const bool hasAction, const bool caseSensitive)
	{
		return addLexemeToken(lexeme, tokenId, hasAction, caseSensitive);
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::executeTokenAction(const size_t tokenID)
	{
		TokenActionIterator tokenIterator = mTokenActionMap.find(tokenID);

		if (tokenIterator == mTokenActionMap.end())
		{
			// BAD command. BAD!
			logParseError("ParticleUniverse: Unrecognised particle universe script command action");
			return;
		}
		else
		{
			try
			{
				(tokenIterator->second)->parse(this);
			}
			catch (Ogre::Exception& ogreException)
			{
				// an unknown token found or BNF Grammer rule was not successful
				// in finding a valid terminal token to complete the rule expression.
				logParseError("ParticleUniverse:" + ogreException.getDescription());
			}
		}
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* ParticleScriptDeserializer::parseDynamicAttribute(void)
	{
		DynamicAttribute* dynamicAttribute = 0;
		if (testNextTokenID(ITokenRegister::ID_DA_RANDOM))
		{
			// Need to set the context, because we are going a level deeper
			dynamicAttribute = OGRE_NEW_T(DynamicAttributeRandom, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			context.beginSection(DYNAMIC_ATTRIBUTE, dynamicAttribute);
		}
		else if (testNextTokenID(ITokenRegister::ID_DA_CURVED_LINEAR))
		{
			// Need to set the context, because we are going a level deeper
			dynamicAttribute = OGRE_NEW_T(DynamicAttributeCurved, Ogre::MEMCATEGORY_SCENE_OBJECTS)(IT_LINEAR);
			context.beginSection(DYNAMIC_ATTRIBUTE, dynamicAttribute);
		}
		else if (testNextTokenID(ITokenRegister::ID_DA_CURVED_SPLINE))
		{
			// Need to set the context, because we are going a level deeper
			dynamicAttribute = OGRE_NEW_T(DynamicAttributeCurved, Ogre::MEMCATEGORY_SCENE_OBJECTS)(IT_SPLINE);
			context.beginSection(DYNAMIC_ATTRIBUTE, dynamicAttribute);
		}
		else if (testNextTokenID(ITokenRegister::ID_DA_OSCILLATE))
		{
			// Need to set the context, because we are going a level deeper
			dynamicAttribute = OGRE_NEW_T(DynamicAttributeOscillate, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			context.beginSection(DYNAMIC_ATTRIBUTE, dynamicAttribute);
		}
		else
		{
			// It is a fixed value, no need to set the context to a deeper level
			DynamicAttributeFixed* dynamicAttributeFixed = OGRE_NEW_T(DynamicAttributeFixed, Ogre::MEMCATEGORY_SCENE_OBJECTS)();
			dynamicAttributeFixed->setValue(getNextTokenValue());
			return dynamicAttributeFixed;
		}

		return dynamicAttribute;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseVector2(Ogre::Vector2& vector)
	{
		const size_t paramCount = getRemainingTokensForAction();
		if (paramCount == 2)
		{
			vector.x = getNextTokenValue();
			vector.y = getNextTokenValue();
		}
		else
		{
			logParseError("ParticleUniverse: Bad vector2 attribute, wrong number of parameters (expected 2)");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseVector3(Ogre::Vector3& vector)
	{
		const size_t paramCount = getRemainingTokensForAction();
		if (paramCount == 3)
		{
			vector.x = getNextTokenValue();
			vector.y = getNextTokenValue();
			vector.z = getNextTokenValue();
		}
		else
		{
			logParseError("ParticleUniverse: Bad vector3 attribute, wrong number of parameters (expected 3)");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseVector4(Ogre::Vector4& vector)
	{
		const size_t paramCount = getRemainingTokensForAction();
		if (paramCount == 4)
		{
			vector.x = getNextTokenValue();
			vector.y = getNextTokenValue();
			vector.z = getNextTokenValue();
			vector.w = getNextTokenValue();
		}
		else
		{
			logParseError("ParticleUniverse: Bad vector4 attribute, wrong number of parameters (expected 4)");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseQuaternion(Ogre::Quaternion& quaternion)
	{
		const size_t paramCount = getRemainingTokensForAction();
		if (paramCount == 4)
		{
			quaternion.w = getNextTokenValue();
			quaternion.x = getNextTokenValue();
			quaternion.y = getNextTokenValue();
			quaternion.z = getNextTokenValue();
		}
		else
		{
			logParseError("ParticleUniverse: Bad quaternion attribute, wrong number of parameters (expected 4)");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseColourValue(Ogre::ColourValue& colourValue)
	{
		const size_t paramCount = getRemainingTokensForAction();
		if (paramCount == 3)
		{
			colourValue.r = getNextTokenValue();
			colourValue.g = getNextTokenValue();
			colourValue.b = getNextTokenValue();
			colourValue.a = 1.0f;
		}
		else if (paramCount == 4)
		{
			colourValue.r = getNextTokenValue();
			colourValue.g = getNextTokenValue();
			colourValue.b = getNextTokenValue();
			colourValue.a = getNextTokenValue();
		}
		else
		{
			logParseError("ParticleUniverse: Bad colourValue attribute, wrong number of parameters (expected 3 or 4)");
		}
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseRadian(Ogre::Radian& rad)
	{
		// Script uses degrees
		rad = Ogre::Radian (getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseReal(Ogre::Real& real)
	{
		real = getNextTokenValue();
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseBool(bool& b)
	{
		b = testNextTokenID(ITokenRegister::ID_TRUE);
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseUshort(unsigned short& shortValue)
	{
		shortValue = static_cast<unsigned short>(getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseUint(Ogre::uint& uint)
	{
		uint = static_cast<Ogre::uint>(getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseUint8(Ogre::uint8& uint8)
	{
		uint8 = static_cast<Ogre::uint8>(getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseUint16(Ogre::uint16& uint16)
	{
		uint16 = static_cast<Ogre::uint16>(getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parseUint32(Ogre::uint32& uint32)
	{
		uint32 = static_cast<Ogre::uint32>(getNextTokenValue());
	}
	//-----------------------------------------------------------------------
	void ParticleScriptDeserializer::parse4XUint32(Ogre::uint32& bits0, Ogre::uint32& bits1, Ogre::uint32& bits2, Ogre::uint32& bits3)
	{
		bits0 = static_cast<Ogre::uint32>(getNextTokenValue());
		bits1 = static_cast<Ogre::uint32>(getNextTokenValue());
		bits2 = static_cast<Ogre::uint32>(getNextTokenValue());
		bits3 = static_cast<Ogre::uint32>(getNextTokenValue());
	}

}
