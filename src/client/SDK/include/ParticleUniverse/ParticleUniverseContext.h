/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_CONTEXT_H__
#define __PU_CONTEXT_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseSection.h"
#include "OgreLogManager.h"

namespace ParticleUniverse
{
	// String representation of the Particle Universe contexts. Numeric values are used to keep the string small.
	const static Ogre::String ALIAS					= "1";
	const static Ogre::String SYSTEM				= "2";
	const static Ogre::String TECHNIQUE				= "3";
	const static Ogre::String RENDERER				= "4";
	const static Ogre::String EMITTER				= "5";
	const static Ogre::String AFFECTOR				= "6";
	const static Ogre::String OBSERVER				= "7";
	const static Ogre::String HANDLER				= "8";
	const static Ogre::String BEHAVIOUR				= "9";
	const static Ogre::String EXTERN				= "10";
	const static Ogre::String DYNAMIC_ATTRIBUTE		= "11";
	const static Ogre::String DEPENDENCY			= "12";
	const static Ogre::String INNER_CONTEXT			= "13"; // To be used for deeper sections

	/** The Context class is used to manage the sections of a particle universe script.
	@remarks
		Each script consists of sections, levels with sublevels and subsublevels, etc. The Context 
		keeps track of each section and during parsing it is clear to which section the script parses 
		is pointing. The Context is used to store crucial data of each section.
	*/
	class _ParticleUniverseExport Context
	{
		protected:
			typedef std::vector<Section*> ContextStack;
			ContextStack mContextStack;

			/** This is the last processed section, which is still available for a 'higher' section
			*/

			Section mPreviousProcessedSection;

			/** 
			*/
			Section* _getCurrentSection(void)
			{
				if (!mContextStack.empty())
					return mContextStack.back();
				else
					return 0;
			}

			/** 
			*/
			Section* _getParentSection(void)
			{
				if (mContextStack.size() > 1)
				{
					ContextStack::iterator it = mContextStack.end();
					--it;
					--it;
					return *it;
				}
				else
					return 0;
			}

		public:
			Context(void) : attributeToken(0)
			{
				mPreviousProcessedSection.sectionName = Ogre::StringUtil::BLANK;
				mPreviousProcessedSection.element = 0;
				mPreviousProcessedSection.elementName = Ogre::StringUtil::BLANK;
			};
			virtual ~Context(void) {};
			
			/** attributeToken is needed to temporary set the last attribute value of the parsed script.
			*/
			IToken* attributeToken;

			/** 
			*/
			void beginSection(
				const Ogre::String& sectionName, 
				IElement* element = 0,
				const Ogre::String& elementName = Ogre::StringUtil::BLANK)
			{
				// Add a new section to the stack
				Section* section = OGRE_NEW_T(Section, Ogre::MEMCATEGORY_SCRIPTING)();
				section->sectionName = sectionName;
				section->elementName = elementName;
				section->element = element;
				mContextStack.push_back(section);
			}
			
			/** 
			*/
			void endSection(void)
			{
				Section* section = _getCurrentSection();

				// Set the values in the mPreviousProcessedSection
				if (section)
				{
					mPreviousProcessedSection.sectionName = section->sectionName;
					mPreviousProcessedSection.element = section->element;
					mPreviousProcessedSection.elementName = section->elementName;
				}
				
				if (!mContextStack.empty())
					mContextStack.pop_back();
				
				if (section)
					OGRE_DELETE_T(section, Section, Ogre::MEMCATEGORY_SCRIPTING);
			}
			
			/** 
			*/
			const Ogre::String& getCurrentSectionName(void)
			{
				Section* section = _getCurrentSection();
				if (section)
					return section->sectionName;
				else
					return Ogre::StringUtil::BLANK;
			}

			/** 
			*/
			const Ogre::String& getPreviousSectionName(void)
			{
				return mPreviousProcessedSection.sectionName;
			}

			/** 
			*/
			const Ogre::String& getParentSectionName(void)
			{
				Section* parent = _getParentSection();
				if (parent)
					return parent->sectionName;
				else
					return Ogre::StringUtil::BLANK;
			}

			/** 
			*/
			IElement* getCurrentSectionElement(void)
			{
				Section* section = _getCurrentSection();
				if (section)
					return section->element;
				else
					return 0;
			}

			/** 
			*/
			IElement* getPreviousSectionElement(void)
			{
				return mPreviousProcessedSection.element;
			}

			/** 
			*/
			IElement* getParentSectionElement(void)
			{
				Section* parent = _getParentSection();
				if (parent)
					return parent->element;
				else
					return 0;
			}

			/** 
			*/
			const Ogre::String& getCurrentSectionElementName(void)
			{
				Section* section = _getCurrentSection();
				if (section)
					return section->elementName;
				else
					return Ogre::StringUtil::BLANK;
			}

			/** 
			*/
			const Ogre::String& getPreviousSectionElementName(void)
			{
				return mPreviousProcessedSection.elementName;
			}

			/** 
			*/
			const Ogre::String& getParentSectionElementName(void)
			{
				Section* parent = _getParentSection();
				if (parent)
					return parent->elementName;
				else
					return Ogre::StringUtil::BLANK;
			}

			/** 
			*/
			IElement* getSectionElement(const Ogre::String& sName)
			{
				if (mContextStack.empty())
					return 0;

				ContextStack::iterator it;
				for (it = mContextStack.begin(); it != mContextStack.end(); ++it)
				{
					if ((*it)->sectionName == sName)
					{
						return (*it)->element;
					}
				}
				return 0;
			}

			/** 
			*/
			void validateCurrentSectionName(const Ogre::String& sectionName, 
				const Ogre::String& calledFromFunction = Ogre::StringUtil::BLANK)
			{
				Section* section = _getCurrentSection();
				if (!section || section->sectionName != sectionName)
				{
					OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Invalid section, expected "  + section->sectionName + 
						" but validated section is " + sectionName, calledFromFunction);
				}
			}
	};

}
#endif
