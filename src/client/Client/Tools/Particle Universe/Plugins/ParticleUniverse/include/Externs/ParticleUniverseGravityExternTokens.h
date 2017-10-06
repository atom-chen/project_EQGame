/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_GRAVITY_EXTERN_TOKENS_H__
#define __PU_GRAVITY_EXTERN_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseExternTokens.h"
#include "ParticleUniverseAttachableTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the GravityExtern.
	@remarks
		It is only used for writing to a script (serialization) and not for reading a script (deserialization). 
		Because the properties are the same as the corresponding GravityAffector, it uses the
		GravityAffectorTokens for parsing a script.
    */
	class _ParticleUniverseExport GravityExternTokens : public AttachableTokens, ExternTokens
	{
		public:
			static const Ogre::String KEYWORD_GRAVITY;

			GravityExternTokens(void) {};
			virtual ~GravityExternTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) {};

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);
	};

}
#endif
