/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BILLBOARD_RENDERER_FACTORY_H__
#define __PU_BILLBOARD_RENDERER_FACTORY_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseRendererFactory.h"
#include "ParticleUniverseBillboardRenderer.h"
#include "ParticleUniverseBillboardRendererTokens.h"

namespace ParticleUniverse
{
	/** Factory class responsible for creating a BillboardRenderer.
    */
	class _ParticleUniverseExport BillboardRendererFactory : public ParticleRendererFactory
	{
		public:
			BillboardRendererFactory(void) {};
	        virtual ~BillboardRendererFactory(void) {};

			/** See ParticleRendererFactory */
			Ogre::String getRendererType() const
			{
				return "Billboard";
			}

			/** See ParticleRendererFactory */
			ParticleRenderer* createRenderer(void)
			{
				return _createRenderer<BillboardRenderer>();
			}

			/** See ParticleRendererFactory */
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister)
			{
				// Delegate to mBillboardRendererTokens
				mBillboardRendererTokens.setupTokenDefinitions(tokenRegister);
			}

			/** See ParticleRendererFactory */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate to mOnVelocityObserverTokens
				mBillboardRendererTokens.write(serializer, element);
			}


		protected:
			BillboardRendererTokens mBillboardRendererTokens;
	};

}
#endif
