/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_LISTENER_H__
#define __PU_SYSTEM_LISTENER_H__

#include "ParticleUniversePrerequisites.h"

namespace ParticleUniverse
{
	/*  ParticleSystemListener establishes a channel between a ParticleSystem and some other object, which is a 
	    type of ParticleSystemListener.
	*/
	class _ParticleUniverseExport ParticleSystemListener
	{
		public:
			ParticleSystemListener(void) {};
			virtual ~ParticleSystemListener(void) {};

			/*  Callback function, which is called as soon as a particle system is started.
			*/
			virtual void started(void) = 0;

			/*  Callback function, which is called as soon as a particle system is stopped.
			*/
			virtual void stopped(void) = 0;

	};

}
#endif
