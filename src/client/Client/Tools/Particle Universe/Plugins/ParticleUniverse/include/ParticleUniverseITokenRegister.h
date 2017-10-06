/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_I_TOKEN_REGISTER_H__
#define __PU_I_TOKEN_REGISTER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseIToken.h"

namespace ParticleUniverse
{
	/** The ITokenRegister acts as an interface between the script deserializer and grammars. A class of type
		ITokenRegister is used to register reserved words or keywords.
	*/
	class _ParticleUniverseExport ITokenRegister
    {
		protected:
			/** Map of Token value as key to an Action.  An Action converts tokens into the final format.
			*/
			typedef std::map<size_t, IToken*> TokenActionMap;
			typedef TokenActionMap::iterator TokenActionIterator;

		public:
			// Token ID enumeration
			enum TokenID
			{
				// Terminal Tokens section
				ID_UNKOWN = 0,

				// Dynamic attribute section
				ID_DA_RANDOM, ID_DA_CURVED_LINEAR, ID_DA_CURVED_SPLINE, ID_DA_OSCILLATE,
	
				// Particle section
				ID_PT_VISUAL, ID_PT_TECHNIQUE, ID_PT_EMITTER, ID_PT_AFFECTOR, ID_PT_SYSTEM,

				// Component section
				ID_CT_VISUAL_PARTICLE, ID_CT_TECHNIQUE, ID_CT_EMITTER, ID_CT_AFFECTOR, ID_CT_OBSERVER,

				// Billboard Renderer section
				ID_BBT_ORIENTED_COMMON, ID_BBT_ORIENTED_SELF, ID_BBT_ORIENTED_SHAPE, ID_BBT_PERPENDICULAR_COMMON, 
				ID_BBT_PERPENDICULAR_SELF, 

				ID_BBT_TOP_LEFT, ID_BBT_TOP_CENTER, ID_BBT_TOP_RIGHT, ID_BBT_CENTER_LEFT, ID_BBT_CENTER, ID_BBT_CENTER_RIGHT,
				ID_BBT_BOTTOM_LEFT, ID_BBT_BOTTOM_CENTER, ID_BBT_BOTTOM_RIGHT,

				ID_BBT_VERTEX, ID_BBT_TEX_COORD,

				// Affector
				ID_FA_ADD, ID_FA_AVERAGE, ID_AFSP_DEFAULT, ID_AFSP_TTL_INCREASE, ID_AFSP_TTL_DECREASE,

				// Shapes
				ID_SHAPE_POINT, ID_SHAPE_BOX, ID_SHAPE_SPHERE, ID_SHAPE_PLANE,

				// Collision
				ID_COT_NONE, ID_COT_BOUNCE, ID_COT_FLOW,

				// Comparison operators
				ID_COMPARE_LESS_THAN, ID_COMPARE_GREATER_THAN, ID_COMPARE_EQUALS,
	
				// General
				ID_CAMERA_DISTANCE, ID_ON, ID_OFF, ID_TRUE, ID_FALSE, ID_STREAM,

				// where auto generated tokens start so do not remove
				ID_AUTOTOKENSTART
			};

			ITokenRegister(void) {};
			virtual ~ITokenRegister(void) {};

			/** Associate all the lexemes used in a particle universe script with their corresponding tokens 
				and actions.
			**/
			virtual void addLexemeTokenAction(const Ogre::String& lexeme, const size_t tokenId, IToken* token = 0) = 0;
			void addLexemeAction(const Ogre::String& lexeme, IToken* token) { addLexemeTokenAction(lexeme, 0, token); }
			virtual size_t addTokenId(const Ogre::String& lexeme, const size_t tokenId, const bool hasAction = false, const bool caseSensitive = false) = 0;
	};

}
#endif
