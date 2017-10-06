/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PHYSX_LOGGING_H__
#define __PU_PHYSX_LOGGING_H__

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX

namespace ParticleUniverse
{
	class PhysXLogging : public NxUserOutputStream
	{
		public:
			/** 
			*/
			void reportError(NxErrorCode e, const char* message, const char* file, int line)
			{
				Ogre::LogManager::getSingleton().logMessage("ParticleUniverse PhysX: " + 
					Ogre::String(file) + " " + 
					Ogre::StringConverter::toString(line));
				switch (e)
				{
					case NXE_INVALID_PARAMETER:
					Ogre::LogManager::getSingleton().logMessage("Invalid parameter");
					break;

					case NXE_INVALID_OPERATION:
					Ogre::LogManager::getSingleton().logMessage("Invalid operation");
					break;

					case NXE_OUT_OF_MEMORY:
					Ogre::LogManager::getSingleton().logMessage("Out of memory");
					break;

					case NXE_DB_INFO:
					Ogre::LogManager::getSingleton().logMessage("Info");
					break;

					case NXE_DB_WARNING:
					Ogre::LogManager::getSingleton().logMessage("Warning");
					break;

					default:
					Ogre::LogManager::getSingleton().logMessage("Unknown error");
				}

				Ogre::LogManager::getSingleton().logMessage(Ogre::String(message));
			}

			/** 
			*/
			NxAssertResponse reportAssertViolation(const char* message, const char* file, int line)
			{
				Ogre::LogManager::getSingleton().logMessage("Particle Universe PhysX - Access Violation: " + 
					Ogre::String(message) + " " + 
					Ogre::String(file) + " " + 
					Ogre::StringConverter::toString(line));
#ifdef WIN32
				switch (MessageBox(0, message, "AssertViolation, see console for details.", MB_ABORTRETRYIGNORE))
				{
					case IDRETRY:
						return NX_AR_CONTINUE;
					case IDIGNORE:
						return NX_AR_IGNORE;
					case IDABORT:
					default:
					return NX_AR_BREAKPOINT;
				}
#elif LINUX
				assert(0);
#elif _XBOX
				return NX_AR_BREAKPOINT;
#elif __CELLOS_LV2__
				return NX_AR_BREAKPOINT;
#endif
			}

			/** 
			*/
			void print(const char* message)
			{
				Ogre::LogManager::getSingleton().logMessage(Ogre::String(message));
			}
	};

}
#endif //PU_PHYSICS_PHYSX
#endif