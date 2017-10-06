/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_MANAGER_TOKENS_H__
#define __PU_SYSTEM_MANAGER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenBind.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed on a ParticleSystemManager level.
    */
	class _ParticleUniverseExport ParticleSystemManagerTokens : public ITokenBind
	{
		protected:

		public:
			// Constants
			static const Ogre::String KEYWORD_USE_ALIAS;
			static const Ogre::String KEYWORD_ALIAS;
			static const Ogre::String KEYWORD_SYSTEM;
			static const Ogre::String KEYWORD_ENABLED;
			static const Ogre::String KEYWORD_POSITION;
			static const Ogre::String KEYWORD_KEEP_LOCAL;
			static const Ogre::String KEYWORD_CAMERA_DEPENDENCY;
			static const Ogre::String KEYWORD_VISUAL_PARTICLE;
			static const Ogre::String KEYWORD_TECHNIQUE_PARTICLE;
			static const Ogre::String KEYWORD_EMITTER_PARTICLE;
			static const Ogre::String KEYWORD_AFFECTOR_PARTICLE;
			static const Ogre::String KEYWORD_SYSTEM_PARTICLE;
			static const Ogre::String KEYWORD_VISUAL_PARTICLE_COMPONENT;
			static const Ogre::String KEYWORD_TECHNIQUE_COMPONENT;
			static const Ogre::String KEYWORD_EMITTER_COMPONENT;
			static const Ogre::String KEYWORD_AFFECTOR_COMPONENT;
			static const Ogre::String KEYWORD_OBSERVER_COMPONENT;
			static const Ogre::String KEYWORD_LESS_THAN;
			static const Ogre::String KEYWORD_GREATER_THAN;
			static const Ogre::String KEYWORD_EQUALS;
			static const Ogre::String KEYWORD_CAMERA_DISTANCE;
			static const Ogre::String KEYWORD_POINT;
			static const Ogre::String KEYWORD_BOX;
			static const Ogre::String KEYWORD_SPHERE;
			static const Ogre::String KEYWORD_PLANE;
			static const Ogre::String KEYWORD_TRUE;
			static const Ogre::String KEYWORD_FALSE;
			static const Ogre::String KEYWORD_ON;
			static const Ogre::String KEYWORD_OFF;
			static const Ogre::String KEYWORD_STREAM;

			ParticleSystemManagerTokens(void) {};
			virtual ~ParticleSystemManagerTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element) {}

			// Tokens which get called during parsing.
			class OpenBraceToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mOpenBraceToken;
			class CloseBraceToken : public IToken {	virtual void parse(ParticleScriptDeserializer* deserializer);} mCloseBraceToken;
			class AliasToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mAliasToken;
			class UseAliasToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mUseAliasToken;
			class SystemToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mSystemToken;
			class EnabledToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mEnabledToken;
			class PositionToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mPositionToken;
			class KeepLocalToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mKeepLocalToken;
			class CameraDependencyToken : public IToken {virtual void parse(ParticleScriptDeserializer* deserializer);} mCameraDependencyToken;
	};

}
#endif
