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
#include "ParticleUniverseDynamicAttribute.h"
#include "ParticleUniverseDynamicAttributeFactory.h"
#include "ParticleUniverseDynamicAttributeTokens.h"

namespace ParticleUniverse
{
	// Constants
	const Ogre::String DynamicAttributeTokens::KEYWORD_CONTROL_POINT = "control_point";
	const Ogre::String DynamicAttributeTokens::KEYWORD_MIN = "min";
	const Ogre::String DynamicAttributeTokens::KEYWORD_MAX = "max";
	const Ogre::String DynamicAttributeTokens::KEYWORD_OSCILLATE_FREQUENCY = "oscillate_frequency";
	const Ogre::String DynamicAttributeTokens::KEYWORD_OSCILLATE_PHASE = "oscillate_phase";
	const Ogre::String DynamicAttributeTokens::KEYWORD_OSCILLATE_BASE = "oscillate_base";
	const Ogre::String DynamicAttributeTokens::KEYWORD_OSCILLATE_AMPLITUDE = "oscillate_amplitude";
	const Ogre::String DynamicAttributeTokens::KEYWORD_OSCILLATE_TYPE = "oscillate_type";
	const Ogre::String DynamicAttributeTokens::KEYWORD_DYN_OSCILLATE = "dyn_oscillate";
	const Ogre::String DynamicAttributeTokens::KEYWORD_DYN_RANDOM = "dyn_random";
	const Ogre::String DynamicAttributeTokens::KEYWORD_DYN_CURVED_LINEAR = "dyn_curved_linear";
	const Ogre::String DynamicAttributeTokens::KEYWORD_DYN_CURVED_SPLINE = "dyn_curved_spline";
	const Ogre::String DynamicAttributeTokens::KEYWORD_SINE = "sine";
	const Ogre::String DynamicAttributeTokens::KEYWORD_SQUARE = "square";

	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::setupTokenDefinitions(ITokenRegister* tokenRegister)
	{
		tokenRegister->addLexemeAction(KEYWORD_CONTROL_POINT, &mControlPointToken);
		tokenRegister->addLexemeAction(KEYWORD_MIN, &mMinimumToken);
		tokenRegister->addLexemeAction(KEYWORD_MAX, &mMaximumToken);
		tokenRegister->addLexemeAction(KEYWORD_OSCILLATE_FREQUENCY, &mFrequencyToken);
		tokenRegister->addLexemeAction(KEYWORD_OSCILLATE_PHASE, &mPhaseToken);
		tokenRegister->addLexemeAction(KEYWORD_OSCILLATE_BASE, &mBaseToken);
		tokenRegister->addLexemeAction(KEYWORD_OSCILLATE_AMPLITUDE, &mAmplitudeToken);
		tokenRegister->addLexemeAction(KEYWORD_OSCILLATE_TYPE, &mTypeToken);

		tokenRegister->addTokenId(KEYWORD_DYN_OSCILLATE, ITokenRegister::ID_DA_OSCILLATE);
		tokenRegister->addTokenId(KEYWORD_DYN_RANDOM, ITokenRegister::ID_DA_RANDOM);
		tokenRegister->addTokenId(KEYWORD_DYN_CURVED_LINEAR, ITokenRegister::ID_DA_CURVED_LINEAR);
		tokenRegister->addTokenId(KEYWORD_DYN_CURVED_SPLINE, ITokenRegister::ID_DA_CURVED_SPLINE);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// The DynamicAttribute doesn't know its context (which property), so that information is passed from another level.
		const DynamicAttribute* dynAttribute = static_cast<const DynamicAttribute*>(element);
		switch(dynAttribute->getType())
		{
			case DynamicAttribute::DAT_FIXED:
				{
					const DynamicAttributeFixed* dynFixed = static_cast<const DynamicAttributeFixed*>(element);
					DynamicAttributeFixed df(*dynFixed); // Needed to get rid of the const, because the getValue function is not a const!
					serializer->writeLine(
						serializer->getKeyword(), 
						Ogre::StringConverter::toString(df.getValue()),
						serializer->getIndentation());
				}
				break;

			case DynamicAttribute::DAT_RANDOM:
				{
					serializer->writeLine(serializer->getKeyword(), KEYWORD_DYN_RANDOM, serializer->getIndentation());
					serializer->writeLine("{", serializer->getIndentation());
					const DynamicAttributeRandom* dynRandom = static_cast<const DynamicAttributeRandom*>(element);
					serializer->writeLine(KEYWORD_MIN, Ogre::StringConverter::toString(dynRandom->getMin()), serializer->getIndentation() + 4);
					serializer->writeLine(KEYWORD_MAX, Ogre::StringConverter::toString(dynRandom->getMax()), serializer->getIndentation() + 4);
					serializer->writeLine("}", serializer->getIndentation());
				}
				break;

			case DynamicAttribute::DAT_CURVED:
				{
					const DynamicAttributeCurved* dynCurved = static_cast<const DynamicAttributeCurved*>(element);
					Ogre::String type = KEYWORD_DYN_CURVED_LINEAR;
					if (dynCurved->getInterpolationType() == IT_SPLINE)
					{
						type = KEYWORD_DYN_CURVED_SPLINE;
					}
					serializer->writeLine(serializer->getKeyword(), type, serializer->getIndentation());
					serializer->writeLine("{", serializer->getIndentation());

					// Write controlpoints
					const DynamicAttributeCurved::ControlPointList controlPoints = dynCurved->getControlPoints();
					if (!controlPoints.empty())
					{
						DynamicAttributeCurved::ControlPointList::const_iterator it;
						DynamicAttributeCurved::ControlPointList::const_iterator itEnd = controlPoints.end();
						for(it = controlPoints.begin(); it != itEnd; ++it)
						{
							serializer->writeLine(KEYWORD_CONTROL_POINT, Ogre::StringConverter::toString(*it), serializer->getIndentation() + 4);
						}
					}

					serializer->writeLine("}", serializer->getIndentation());
				}
				break;

			case DynamicAttribute::DAT_OSCILLATE:
				{
					serializer->writeLine(serializer->getKeyword(), KEYWORD_DYN_OSCILLATE, serializer->getIndentation());
					serializer->writeLine("{", serializer->getIndentation());
					const DynamicAttributeOscillate* dynOscillate = static_cast<const DynamicAttributeOscillate*>(element);
					serializer->writeLine(KEYWORD_OSCILLATE_FREQUENCY, Ogre::StringConverter::toString(dynOscillate->getFrequency()), serializer->getIndentation() + 4);
					serializer->writeLine(KEYWORD_OSCILLATE_PHASE, Ogre::StringConverter::toString(dynOscillate->getPhase()), serializer->getIndentation() + 4);
					serializer->writeLine(KEYWORD_OSCILLATE_BASE, Ogre::StringConverter::toString(dynOscillate->getBase()), serializer->getIndentation() + 4);
					serializer->writeLine(KEYWORD_OSCILLATE_AMPLITUDE, Ogre::StringConverter::toString(dynOscillate->getAmplitude()), serializer->getIndentation() + 4);
					Ogre::String type = KEYWORD_SINE;
					if (dynOscillate->getOscillationType() == DynamicAttributeOscillate::OSCT_SQUARE)
					{
						type = KEYWORD_SQUARE;
					}
					serializer->writeLine(KEYWORD_OSCILLATE_TYPE, type, serializer->getIndentation() + 4);
					serializer->writeLine("}", serializer->getIndentation());
				}
				break;
		}
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::ControlPointToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeCurved* dynamicAttribute = static_cast<DynamicAttributeCurved*>(deserializer->context.getCurrentSectionElement());
		Ogre::Vector2 vector;
		deserializer->parseVector2(vector);
		dynamicAttribute->addControlPoint(vector.x, vector.y);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::MinimumToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeRandom* dynamicAttribute = static_cast<DynamicAttributeRandom*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real min;
		deserializer->parseReal(min);
		dynamicAttribute->setMin(min);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::MaximumToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeRandom* dynamicAttribute = static_cast<DynamicAttributeRandom*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real max;
		deserializer->parseReal(max);
		dynamicAttribute->setMax(max);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::TypeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeOscillate* dynamicAttribute = static_cast<DynamicAttributeOscillate*>(deserializer->context.getCurrentSectionElement());
		Ogre::String type = deserializer->getNextTokenLabel();
		if (type == KEYWORD_SINE)
		{
			dynamicAttribute->setOscillationType(DynamicAttributeOscillate::OSCT_SINE);
		}
		else if (type == KEYWORD_SQUARE)
		{
			dynamicAttribute->setOscillationType(DynamicAttributeOscillate::OSCT_SQUARE);
		}
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::FrequencyToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeOscillate* dynamicAttribute = static_cast<DynamicAttributeOscillate*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real frequency;
		deserializer->parseReal(frequency);
		dynamicAttribute->setFrequency(frequency);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::PhaseToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeOscillate* dynamicAttribute = static_cast<DynamicAttributeOscillate*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real phase;
		deserializer->parseReal(phase);
		dynamicAttribute->setPhase(phase);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::BaseToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeOscillate* dynamicAttribute = static_cast<DynamicAttributeOscillate*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real base;
		deserializer->parseReal(base);
		dynamicAttribute->setBase(base);
	}
	//-----------------------------------------------------------------------
	void DynamicAttributeTokens::AmplitudeToken::parse(ParticleScriptDeserializer* deserializer)
	{
		DynamicAttributeOscillate* dynamicAttribute = static_cast<DynamicAttributeOscillate*>(deserializer->context.getCurrentSectionElement());
		Ogre::Real amplitude;
		deserializer->parseReal(amplitude);
		dynamicAttribute->setAmplitude(amplitude);
	}

}
