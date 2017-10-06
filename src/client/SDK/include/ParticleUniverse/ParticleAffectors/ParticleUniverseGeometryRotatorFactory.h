/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_GEOMETRY_ROTATOR_FACTORY_H__
#define __PU_GEOMETRY_ROTATOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseGeometryRotatorTokens.h"
#include "ParticleUniverseGeometryRotator.h"
#include "ParticleUniverseAffectorFactory.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the GeometryRotator.
    */
	class _ParticleUniverseExport GeometryRotatorFactory : public ParticleAffectorFactory
	{
		public:
			// Constants
			static const Ogre::String KEYWORD_AFFECTOR;
			static const Ogre::String KEYWORD_USE_OWN_ROTATION;
			static const Ogre::String KEYWORD_ROTATION_SPEED;
			static const Ogre::String KEYWORD_ROTATION_AXIS;

			GeometryRotatorFactory(void){};
	        virtual ~GeometryRotatorFactory(void){};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "GeometryRotator";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<GeometryRotator>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mGeometryRotatorTokens
				mGeometryRotatorTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mGeometryRotatorTokens
				mGeometryRotatorTokens.write(serializer, element);
			}

		protected:
		GeometryRotatorTokens mGeometryRotatorTokens;
	};

}
#endif