/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PARTICLE_SCRIPT_DESERIALIZER_H__
#define __PU_PARTICLE_SCRIPT_DESERIALIZER_H__

#include "OgreCompiler2Pass.h"
#include "ParticleUniverseIGrammar.h"
#include "ParticleUniverseITokenRegister.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseTechnique.h"
#include "ParticleUniverseRenderer.h"
#include "ParticleUniverseEmitter.h"
#include "ParticleUniverseAffector.h"
#include "ParticleUniverseObserver.h"
#include "ParticleUniverseEventHandler.h"
#include "ParticleUniverseExtern.h"
#include "ParticleUniverseBehaviour.h"
#include "ParticleUniverseDynamicAttribute.h"
#include "ParticleUniverseContext.h"

namespace ParticleUniverse
{
	/*	The ParticleScriptDeserializers' purpose is to parse a particle script and create a ParticleSystem from it.  It makes
		some protected parsing functions of the Compiler2Pass available to other components that are involved in parsing the script.
	*/
	class _ParticleUniverseExport ParticleScriptDeserializer : public Ogre::Compiler2Pass, ITokenRegister
    {
		public:
			Context context;

			ParticleScriptDeserializer();
			virtual ~ParticleScriptDeserializer();

			/** gets BNF Grammer for Particle Universe script.
			*/
			virtual const Ogre::String& getClientBNFGrammer(void) const;

			/** get the name of the Particle Universe script BNF grammer.
			*/
			virtual const Ogre::String& getClientGrammerName(void) const { static const Ogre::String grammerName("Particle Universe Script"); return grammerName; }

			/** Register a pointer to a Grammar object, which is placed under the control of the deserializer.
			*/
			void addGrammar (IGrammar* grammar);

			/** Compile a particle universe script from a data stream using a specific resource group name.
				@param stream Weak reference to a data stream which is the source of the material script
				@param groupName The name of the resource group that resources which are parsed are to become 
				a member of. If this group is loaded or unloaded, then the resources discovered in this script 
				will be loaded / unloaded with it.
			*/
			void parseScript(Ogre::DataStreamPtr& stream, const Ogre::String& groupName)
			{
				mGroupName = groupName; // Set it here and not after the next line, since mGroupName is used deep in the compile() function.
				Ogre::Compiler2Pass::compile(stream->getAsString(),  stream->getName());
			}

			/** 
			*/
			virtual void addLexemeTokenAction(const Ogre::String& lexeme, const size_t tokenId, IToken* token = 0);

			/** 
			*/
			virtual size_t addTokenId(const Ogre::String& lexeme, const size_t tokenId, const bool hasAction = false, const bool caseSensitive = false);

			/** 
			*/
			void logParseError(const Ogre::String& error){};

			/** 
			*/
			const Ogre::String& getNextTokenLabel(void) const
			{
				return Ogre::Compiler2Pass::getNextTokenLabel();
			}

			/** 
			*/
			size_t getRemainingTokensForAction(void) const
			{
				return Ogre::Compiler2Pass::getRemainingTokensForAction();
			}

			/** 
			*/
			const Ogre::String& getCurrentTokenLexeme(void) const
			{
				return Ogre::Compiler2Pass::getCurrentTokenLexeme();
			}

			/** 
			*/
			size_t getNextTokenID(void) const
			{
				return Ogre::Compiler2Pass::getNextTokenID();
			}

			/** 
			*/
			const Ogre::String& getNextTokenLexeme(void) const
			{
				return Ogre::Compiler2Pass::getNextTokenLexeme();
			}

			/** 
			*/
			void skipToken(void) const
			{
				Ogre::Compiler2Pass::skipToken();
			}
			
			/** 
			*/
			float getNextTokenValue(void) const
			{
				return Ogre::Compiler2Pass::getNextTokenValue();
			}
			
			/** 
			*/
			bool testNextTokenID(const size_t expectedTokenID) const
			{
				return Ogre::Compiler2Pass::testNextTokenID(expectedTokenID);
			}

			/** 
			*/
			const Ogre::String& getGroupName(void) const
			{
				return mGroupName;
			}

			/** This function returns the value, because DynamicAttribute can differ in type
			*/
			DynamicAttribute* parseDynamicAttribute(void);

			/** 
			*/
			void parseVector2(Ogre::Vector2& vector);

			/** 
			*/
			void parseVector3(Ogre::Vector3& vector);

			/** 
			*/
			void parseVector4(Ogre::Vector4& vector);

			/** 
			*/
			void parseQuaternion(Ogre::Quaternion& quaternion);

			/** 
			*/
			void parseColourValue(Ogre::ColourValue& colourValue);

			/** 
			*/
			void parseRadian(Ogre::Radian& rad);

			/** 
			*/
			void parseReal(Ogre::Real& real);

			/** 
			*/
			void parseBool(bool& b);

			/** 
			*/
			void parseUshort(unsigned short& shortValue);

			/** 
			*/
			void parseUint(Ogre::uint& uint);

			/** 
			*/
			void parseUint8(Ogre::uint8& uint8);

			/** 
			*/
			void parseUint16(Ogre::uint16& uint16);

			/** 
			*/
			void parseUint32(Ogre::uint32& uint32);

			/** 
			*/
			void parse4XUint32(Ogre::uint32& bits0, Ogre::uint32& bits1, Ogre::uint32& bits2, Ogre::uint32& bits3);

		protected:
			// simplified Backus - Naur Form (BNF) grammer for particle universe scripts
			static Ogre::String particleUniverseScriptGrammar;
			std::vector<IGrammar*> mGrammarList;

			// Resourcegroup
			Ogre::String mGroupName;

			// Instance of a TokenActionMap
			static TokenActionMap mTokenActionMap;

			/** Execute an Action associated with a token. Gets called when the compiler finishes tokenizing a
				section of the source that has been parsed.
			**/
			virtual void executeTokenAction(const size_t tokenID);

	        /** Get the start position of auto generated token IDs.
		    */
			virtual size_t getAutoTokenIDStart() const {return ID_AUTOTOKENSTART;}

			/** Associate all the lexemes used in a particle universe script with their corresponding tokens 
				and actions.
			**/
			virtual void setupTokenDefinitions(void);
	};

}
#endif
