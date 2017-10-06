/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ALIGN_AFFECTOR_FACTORY_H__
#define __PU_ALIGN_AFFECTOR_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffectorFactory.h"
#include "ParticleUniverseAlignAffector.h"
#include "ParticleUniverseAlignAffectorTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating the AlignAffector.
    */
	class _ParticleUniverseExport AlignAffectorFactory : public ParticleAffectorFactory
	{
		public:
			AlignAffectorFactory(void) {};
	        virtual ~AlignAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			Ogre::String getAffectorType() const
			{
				return "Align";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<AlignAffector>();
			}

			/** See ParticleAffectorFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mAlignAffectorTokens
				mAlignAffectorTokens.setupTokenDefinitions(tokenRegister);
			};

			/** See ParticleAffectorFactory */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate to mAlignAffectorTokens
				mAlignAffectorTokens.write(serializer, element);
			}

		protected:
			AlignAffectorTokens mAlignAffectorTokens;
	};

}
#endif
