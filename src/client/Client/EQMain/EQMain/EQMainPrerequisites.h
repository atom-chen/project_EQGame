/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-09
* ����: EQMain��ҪԤ�Ȱ������ļ�, ��ͷ�ļ����������.
		ֻ���ڿ⹤�̲������ļ�
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

//�ڴ�й¶������
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif