/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_COLOUR_AFFECTOR_H__
#define __PU_COLOUR_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	/** This affector is typically used to change the colour of a particle during its lifetime.
    */
	class _ParticleUniverseExport ColourAffector : public ParticleAffector
	{
		public:
			typedef std::map<Ogre::Real, Ogre::ColourValue> ColourMap;
			typedef std::map<Ogre::Real, Ogre::ColourValue>::iterator ColourMapIterator;
			enum ColourOperation
			{
				CAO_MULTIPLY,
				CAO_SET
			};

			// Constants
			static const ColourOperation DEFAULT_COLOUR_OPERATION;

			// Constructor
			ColourAffector(void);

			// Destructor
	        virtual ~ColourAffector(void) {};

			/** 
	        */
			void addColour(Ogre::Real timeFraction, const Ogre::ColourValue& colour);

			/** 
	        */
			const ColourMap& getTimeAndColour(void) const;

			/** 
	        */
			void clearColourMap (void);
				
			/** 
	        */
			inline ColourMapIterator _findNearestColourMapIterator(Ogre::Real timeFraction);

			/** 
	        */
			const ColourOperation& getColourOperation (void) const;

			/** 
	        */
			void setColourOperation (const ColourOperation& colourOperation);

			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Ogre::Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);

		protected:
			ColourMap mColourMap;
			ColourOperation mColourOperation;
	};

}
#endif
