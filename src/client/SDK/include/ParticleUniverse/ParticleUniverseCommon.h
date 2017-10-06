/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_COMMON_H__
#define __PU_COMMON_H__

#include "ParticleUniversePrerequisites.h"

namespace ParticleUniverse
{
	enum ComparisionOperator
	{
		CO_LESS_THAN,
		CO_GREATER_THAN
	};

	enum InterpolationType
	{
		IT_LINEAR,
		IT_SPLINE
	};

}
#endif
