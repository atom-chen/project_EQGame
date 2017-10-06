/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PREREQUISITES_H__
#define __PU_PREREQUISITES_H__

#include "Ogre.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#	if defined( PARTICLE_UNIVERSE_EXPORTS )
#		define _ParticleUniverseExport __declspec( dllexport )
#	else
#		define _ParticleUniverseExport __declspec( dllimport )
#	endif
#else
#	define _ParticleUniverseExport
#endif 

/** If PU_PHYSICS is defined, a physics engine is used. Uncomment the line if a physics engine is used. 
	Comment it if you don't want to use a physics engine.
*/
//#define PU_PHYSICS

/** If the PhysX engine of NVidia is NOT used, comment the line "#define PU_PHYSICS_PHYSX", so it skips compilation of the PhysX-specific
	code and linking of PhysXLoader.lib. Of course you have to provide the code to include another physics engine yourself.
*/
#ifdef PU_PHYSICS
	#define PU_PHYSICS_PHYSX
#endif // PU_PHYSICS

/** If you want to log additional debug info, uncomment the line below (PU_LOG_DEBUG). Comment it if you don't want to use debug logging.
*/
//#define PU_LOG_DEBUG

// 4800 - 'Ogre::uint32' : forcing value to bool 'true' or 'false' (performance warning)
// 4217 - reference to an imported symbol that has been defined locally
#pragma warning (disable : 4800 4217)

namespace ParticleUniverse
{
	class ParticleSystem;
	class ParticleTechnique;
	class ParticleEmitter;
	class ParticleAffector;
	class ParticleObserver;
	class Particle;
	class VisualParticle;
	class Extern;
	class IDependency;
	class IToken;
	class BoxColliderExternFactory;
}
#endif
