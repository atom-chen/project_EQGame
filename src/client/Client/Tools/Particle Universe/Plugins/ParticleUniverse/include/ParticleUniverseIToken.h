/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_I_TOKEN_H__
#define __PU_I_TOKEN_H__

namespace ParticleUniverse
{
	class ParticleScriptDeserializer;

	/*	Abstract (pure virtual) token class.
	@remarks
		A token is the smallest part of a script that still has a meaning. Each class that inherits from IToken 
		has the purpose to interpret a part of the script that corresponds with that token. While the 
		ITokenBind defines which function should be called if a certain ´token´ in a script is parsed, the 
		object that inherits from IToken contains the actual function that performs the parsing/interpreting.
	*/
	class _ParticleUniverseExport IToken
	{
		public:
			IToken(void) {};
			virtual ~IToken(void) {};

			/** Pure virtual function that parses a part of the script that is associated with the Token.
			*/
			virtual void parse(ParticleScriptDeserializer* deserializer) = 0;

			/** Peform post-parsing actions if needed.
			@remarks
				In most cases parsing the part of the script that is associated with the Token is enough, but in
				some cases a post-parse action must be performed. An example is a dependency on an attribute.
				In the script this has the syntax: attribute #value [<< dependency {dependency properties}]
				The 'attribute' part is parsed with an optional stream, identified with <<
				After the 'attribute' is parsed, the 'dependency' part is parsed, but if that is done, both
				parts must be related (the 'dependency' data forms input for the 'attribute' data). This can be
				realised in the postParse() function.
			*/
			virtual void postParse(ParticleScriptDeserializer* deserializer){/* To be overridden when needed */};
	};

}
#endif
