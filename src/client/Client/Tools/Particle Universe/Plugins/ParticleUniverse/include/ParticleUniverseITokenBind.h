/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_I_TOKEN_INITIALISER_H__
#define __PU_I_TOKEN_INITIALISER_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseITokenRegister.h"
#include "ParticleUniverseIElement.h"

namespace ParticleUniverse
{
	class ParticleScriptSerializer;

	/*	The ITokenBind is used to access tokens. A token is a part in a particle script that represents a class or an 
		attribute.
	@remarks
		A class that implements the ITokenBind, is responsible for parsing (deserialization) a particle script 
		or writing (serialization) to a particle script.
	*/
	class _ParticleUniverseExport ITokenBind
    {

		public:
			ITokenBind(void) {};
			virtual ~ITokenBind(void) {};

			/** Child classes must implement this pure virtual function, in which the relation between the 'token'
				in the script and a particular function that is used to parse that part of the script, is defined.
			**/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister) = 0;

			/** Child classes must implement this pure virtual function, which must be used to write an object or attribute to
				a particle script.
			**/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element) = 0;
	};

}
#endif
