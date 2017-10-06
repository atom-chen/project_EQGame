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

#include "ParticleUniverseScriptSerializer.h"
#include <iostream>

namespace ParticleUniverse
{
	//-----------------------------------------------------------------------
	ParticleScriptSerializer::ParticleScriptSerializer(void) :
		mPath(Ogre::StringUtil::BLANK),
		mIndentation(0),
		mKeyword(Ogre::StringUtil::BLANK),
		mStreamToFile(true),
		mScriptString(Ogre::StringUtil::BLANK)
	{
		mTab[0] = 0;
		mTab[1] = 48;
		mTab[2] = 52;
		mTab[3] = 56;
		mTab[4] = 60;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeScript(const ParticleSystem* particleSystem, const Ogre::String& fileName)
	{
		// File open
		mStreamToFile = true;
		mScriptFile.open ((mPath + fileName).c_str());

		// Write
		mParticleSystemTokens.write(this, particleSystem);

		// File close
		mScriptFile.close();
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleScriptSerializer::writeScript(const ParticleSystem* particleSystem)
	{
		// Write
		mStreamToFile = false;
		mScript.clear();
		mScript.str("");
		mParticleSystemTokens.write(this, particleSystem);
		mScriptString = mScript.str();
		return mScriptString;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::setDefaultTabs (
		short tab0, 
		short tab1, 
		short tab2, 
		short tab3,
		short tab4)
	{
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::setPath (const Ogre::String& path)
	{
		mPath = path;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeLine(
		const Ogre::String& s0, 
		const Ogre::String& s1, 
		const Ogre::String& s2,
		const Ogre::String& s3,
		const Ogre::String& s4,
		short indentation0,
		short indentation1,
		short indentation2,
		short indentation3,
		short indentation4)
	{
		Ogre::String base = Ogre::StringUtil::BLANK;
		base.reserve(256);
		_appendTabbedString(0, base, s0, indentation0);
		_appendTabbedString(1, base, s1, indentation1);
		_appendTabbedString(2, base, s2, indentation2);
		_appendTabbedString(3, base, s3, indentation3);
		_appendTabbedString(4, base, s4, indentation4);
		_stream(base + "\n");
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeLine(
		const Ogre::String& s0, 
		const Ogre::String& s1, 
		const Ogre::String& s2,
		const Ogre::String& s3,
		short indentation0,
		short indentation1,
		short indentation2,
		short indentation3)
	{
		Ogre::String base = Ogre::StringUtil::BLANK;
		_appendTabbedString(0, base, s0, indentation0);
		_appendTabbedString(1, base, s1, indentation1);
		_appendTabbedString(2, base, s2, indentation2);
		_appendTabbedString(3, base, s3, indentation3);
		_stream(base + "\n");
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeLine(
		const Ogre::String& s0, 
		const Ogre::String& s1, 
		const Ogre::String& s2,
		short indentation0,
		short indentation1,
		short indentation2)
	{
		Ogre::String base = Ogre::StringUtil::BLANK;
		_appendTabbedString(0, base, s0, indentation0);
		_appendTabbedString(1, base, s1, indentation1);
		_appendTabbedString(2, base, s2, indentation2);
		_stream(base + "\n");
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeLine(
		const Ogre::String& s0, 
		const Ogre::String& s1,
		short indentation0,
		short indentation1)
	{
		Ogre::String base = Ogre::StringUtil::BLANK;
		_appendTabbedString(0, base, s0, indentation0);
		_appendTabbedString(1, base, s1, indentation1);
		_stream(base + "\n");
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::writeLine(
		const Ogre::String& s0,
		short indentation0)
	{
		Ogre::String base = Ogre::StringUtil::BLANK;
		_appendTabbedString(0, base, s0, indentation0);
		_stream(base + "\n");
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::_appendTabbedString(
		size_t index, 
		Ogre::String& base, 
		const Ogre::String& s, 
		short indentation)
	{
		if (indentation > 0)
		{
			// Append gives garbage, so use for-loop instead
			for (short i = 0; i < indentation; ++i) base += " ";
		}
		else if (indentation < 0)
		{
			short pos = mTab[index] - (short)(base.length());
			pos = pos >= 0 ? pos : 1;
			pos = pos < 0 ? 1 : pos;
			for (short i = 0; i < pos; ++i) base += " ";
		}

		base.append(s);
	}
	//-----------------------------------------------------------------------
	Ogre::String ParticleScriptSerializer::toString(std::vector<Ogre::Real> vector, bool applySqrt)
	{
        Ogre::StringUtil::StrStreamType stream;
		if (!vector.empty())
		{
			for (size_t i = 0; i < vector.size(); ++i)
			{
				if (applySqrt)
				{
					stream << Ogre::Math::Sqrt(vector[i]) << " ";
				}
				else
				{
					stream << vector[i] << " ";
				}
			}
		}
        return stream.str();
	}
	//-----------------------------------------------------------------------
	const short ParticleScriptSerializer::getIndentation(void) const
	{
		return mIndentation;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::setIndentation(const short indentation)
	{
		mIndentation = indentation;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleScriptSerializer::getKeyword(void) const
	{
		return mKeyword;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::setKeyword(const Ogre::String& keyword)
	{
		mKeyword = keyword;
	}
	//-----------------------------------------------------------------------
	void ParticleScriptSerializer::_stream (Ogre::String& string)
	{
		if (mStreamToFile)
		{
			mScriptFile << string;
		}
		else
		{
			mScript << string;
		}
	}

}
