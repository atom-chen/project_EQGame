/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_I_GRAMMAR_H__
#define __PU_I_GRAMMAR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenRegister.h"

namespace ParticleUniverse
{
	/*	Abstract Grammar class
	@remarks
		Each plugin containing emitters, affectors, etc. should have its own Backus - Naur grammar and has
		to implement some virtual functions.
		The plugin must provide the rules list, which looks like "| {<r1>} | {<r2>} | {<r3>} | {<r4>}", and also
		the details (properties) of each emitter, affector, etc. Both the rules list and the details are combined
		with the overall BNF grammar.
	*/
	class _ParticleUniverseExport IGrammar
    {
		public:
			IGrammar(void) {};
			virtual ~IGrammar(void) {};
			virtual const Ogre::String& getParticleRendererPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleRendererDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleEmitterPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleEmitterDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleAffectorPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleAffectorDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleObserverPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleObserverDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleEventHandlerPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleEventHandlerDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleBehaviourPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getParticleBehaviourDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getExternPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
			virtual const Ogre::String& getExternDetailedPropertiesGrammar(void){return Ogre::StringUtil::BLANK;};
	};

}
#endif
