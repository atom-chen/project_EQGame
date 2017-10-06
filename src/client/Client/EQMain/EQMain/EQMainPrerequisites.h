/******************************************************************************
* 作者: Y3
* 时间: 2009-10-09
* 描述: EQMain需要预先包含的文件, 在头文件都包含这个.
		只有在库工程才有这文件
******************************************************************************/
#ifndef EQMainPrerequisites_H
#define EQMainPrerequisites_H


#include <Ogre/Ogre.h>

#include "tinyXml/tinyxml.h"


using Ogre::Real;

using Ogre::uchar;
using Ogre::ushort;
using Ogre::uint;
using Ogre::ulong;

using Ogre::uint8;
using Ogre::uint16;
using Ogre::uint32;

//内存泄露调试用
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif