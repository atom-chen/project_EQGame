/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PARTICLE_SCRIPT_SERIALIZER_H__
#define __PU_PARTICLE_SCRIPT_SERIALIZER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSystem.h"
#include "ParticleUniverseSystemTokens.h"
#include "ParticleUniverseContext.h"

namespace ParticleUniverse
{
	/** The ParticleScriptSerializer class is responsible for writing objects and attributes to a particle system script.
	*/
	class _ParticleUniverseExport ParticleScriptSerializer : public Ogre::SerializerAlloc
	{
		public:
			Context context;

			ParticleScriptSerializer(void);
			virtual ~ParticleScriptSerializer(void) {};

			/** Writes a ParticleSystem to a file in the script format.
			*/
			void writeScript(const ParticleSystem* particleSystem, const Ogre::String& fileName);

			/** If no filename has been given, serialisation is done to a string.
			*/
			const Ogre::String& writeScript(const ParticleSystem* particleSystem);

			/** Writes a line to a script file.
			*/
			void writeLine(
				const Ogre::String& s0, 
				const Ogre::String& s1, 
				const Ogre::String& s2,
				const Ogre::String& s3,
				const Ogre::String& s4,
				short indentation0 = -1,
				short indentation1 = -1,
				short indentation2 = -1,
				short indentation3 = -1,
				short indentation4 = -1);

			/** Writes a line to a script file.
			*/
			void writeLine(
				const Ogre::String& s0, 
				const Ogre::String& s1, 
				const Ogre::String& s2,
				const Ogre::String& s3,
				short indentation0 = -1,
				short indentation1 = -1,
				short indentation2 = -1,
				short indentation3 = -1);

			/** Writes a line to a script file.
			*/
			void writeLine(
				const Ogre::String& s0, 
				const Ogre::String& s1, 
				const Ogre::String& s2,
				short indentation0 = -1,
				short indentation1 = -1,
				short indentation2 = -1);

			/** Writes a line to a script file.
			*/
			void writeLine(
				const Ogre::String& s0, 
				const Ogre::String& s1,
				short indentation0 = -1,
				short indentation1 = -1);

			/** Writes a line to a script file.
			*/
			void writeLine(
				const Ogre::String& s0,
				short indentation0 = -1);

			/** Set the default tab values. If a writeline doesn't contain any indentation value, the default tab values are 
				used.
			*/
			void setDefaultTabs (
				short tab0 = 0, 
				short tab1 = 48, 
				short tab2 = 52, 
				short tab3 = 56,
				short tab4 = 60);

			/** Set the directory to which files are serialized.
			*/
			void setPath (const Ogre::String& path);

			/** Creates a string from a vector with Reals. This function contains an additional argument. If set to true, the
				square root is applied to all real values (don't ask!).
			*/
			static Ogre::String toString(std::vector<Ogre::Real> vector, bool applySqrt = false);

			/** Get/set the indentation. To be used in cases where it is unkown what the indentation is.
			*/
			const short getIndentation(void) const;
			void setIndentation(const short indentation);

			/** Get/set the keyword. To be used in cases where it is unkown what the keyword is or in case serialisation is spread across
				multiple objects.
			*/
			const Ogre::String& getKeyword(void) const;
			void setKeyword(const Ogre::String& keyword);

		protected:
			ParticleSystemTokens mParticleSystemTokens;
			Ogre::String mPath;
			std::ofstream mScriptFile;
			Ogre::StringUtil::StrStreamType mScript;
			Ogre::String mScriptString;
			short mTab[5];
			short mIndentation;
			Ogre::String mKeyword;
			bool mStreamToFile;

			/** Concatenate base string and string 's', with the proper indentation.
			*/
			void _appendTabbedString(
				size_t index, 
				Ogre::String& base, 
				const Ogre::String& s, 
				short indentation);

			/** Writes to a file or to a string
			*/
			void _stream (Ogre::String& string);
	};

}
#endif
